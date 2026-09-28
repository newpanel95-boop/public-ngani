#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma once
#include "../XPremium/Skeleton.h"
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <array>
#include <vector>

#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / IM_PI) )
#define DEG2RAD( x ) ( (float)(x) * (float)(IM_PI / 180.f) )

extern ImFont* F50;
extern float speedHackMultiplier;

void (*oWeaponFireComponent_Instant_CreateBulletLine)(uintptr_t thiz, Vector3 startPos, Vector3 dir, bool isDualFire) = nullptr;
void (*oWeaponFireComponent_Instant_CreateBulletProjectile)(void* thiz, Vector3 startPos, Vector3 dir, void* weaponImpact, int itemID, int flySmokeAssetID, bool enableVirtualStartPos, Vector3 virtualStartPos) = nullptr;

void DrawText1(ImDrawList *draw, const std::string &text, const Vector2 &position, ImU32 color, float fontSize) {
    draw->AddText(NULL, fontSize, {position.x, position.y}, color, text.c_str());
}

void DrawTextWithBorder1(ImDrawList *draw, const std::string &text, const Vector2 &position, ImU32 textColor, ImU32 borderColor, float fontSize) {
    float borderSize = 1.0f;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            if (x == 0 && y == 0)
                continue;
            DrawText1(draw, text, {position.x + x * borderSize, position.y + y * borderSize}, borderColor, fontSize);
        }
    }
    DrawText1(draw, text, position, textColor, fontSize);
}

#include "System/Hooks/Feats.h"

void RotateTriangle(std::array<Vector3, 3> & points, float rotation) {
    const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
    for (auto & point : points) {
        point = point - points_center;
        const auto temp_x = point.x;
        const auto temp_y = point.y;
        const auto theta = DEG2RAD(rotation);
        const auto c = cosf(theta);
        const auto s = sinf(theta);
        point.x = temp_x * c - temp_y * s;
        point.y = temp_x * s + temp_y * c;
        point = point + points_center;
    }
}

void VectorAnglesRadar(Vector3 & forward, Vector3 & angles) {
    if (forward.x == 0.f && forward.y == 0.f) {
        angles.x = forward.z > 0.f ? -90.f : 90.f;
        angles.y = 0.f;
    } else {
        angles.x = RAD2DEG(atan2(-forward.z, forward.Magnitude(forward)));
        angles.y = RAD2DEG(atan2(forward.y, forward.x));
    }
    angles.z = 0.f;
}

char extra[30];
int atas, kanan;

static inline int ColorToU8(float v) {

    if (v <= 1.0f) v *= 255.0f;
    if (v < 0.0f) v = 0.0f;
    if (v > 255.0f) v = 255.0f;
    return (int)(v + 0.5f);
}

int32_t ToColor(float *col) {
    if (!col) return IM_COL32(255, 255, 255, 255);
    return IM_COL32(
        ColorToU8(col[0]),
        ColorToU8(col[1]),
        ColorToU8(col[2]),
        ColorToU8(col[3])
    );
}

int totalBots, totalEnemies;
bool isEspReady;

static inline void ApplySpeedhack() {
    static float lastAppliedScale = 1.0f;
    const float targetScale = (speedHackMultiplier <= 0.0f) ? 1.0f : speedHackMultiplier;
    if (fabsf(targetScale - lastAppliedScale) <= 0.001f) return;

    auto Time_set_timeScale = reinterpret_cast<void (*)(float)>(Class_Time_set_timeScale);
    if (!Time_set_timeScale) return;

    Time_set_timeScale(targetScale);
    lastAppliedScale = targetScale;
}

uintptr_t GetClosestTarget() {
    uintptr_t result = 0;
    float MaxDist = std::numeric_limits<float>::infinity();
    auto Gameplay_get_MatchGame = (uintptr_t (*)()) (Class_Gameplay_get_MatchGame);
    auto get_MatchGame = Gameplay_get_MatchGame();
    if (Tools::IsPtrValid((void *) get_MatchGame)) {
        auto Gameplay_get_LocalPawn = (uintptr_t (*)()) (Class_Gameplay_get_LocalPawn);
        auto LocalPawn = Gameplay_get_LocalPawn();
        if (LocalPawn) {
            Vector3 MyPos{0, 0, 0};
            auto local_m_Mesh = *(Transform **) (LocalPawn + Class_Pawn_m_Mesh);
            if (local_m_Mesh) {
                MyPos = local_m_Mesh->get_position();
            }
            auto EnemyPawns = *(List<uintptr_t> **) (get_MatchGame + Class_BaseGame_EnemyPawns);
            if (EnemyPawns) {
                auto Items = EnemyPawns->getItems();
                if (Items) {
                    for (int i = 0; i < EnemyPawns->getSize(); i++) {
                        auto Pawn = Items[i];
                        if (Pawn) {
                            if (!*(bool *) (Pawn + Class_Pawn_m_IsAlive))
                                continue;
                            auto m_Mesh = *(Transform **) (Pawn + Class_Pawn_m_Mesh);
                            if (!m_Mesh)
                                continue;
                            auto RootPos = m_Mesh->get_position();
                            float Distance = Vector3::Distance(MyPos, RootPos);
                            if (Distance < MaxDist) {
                                result = Pawn;
                                MaxDist = Distance;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

inline float GetAimFov()
{
    return Config.Aim.Cross > 0.0f ? Config.Aim.Cross : Config.Aim.size;
}

inline bool IsAimVectorUsable(Vector3 v)
{
    return v.x == v.x && v.y == v.y && v.z == v.z &&
           fabsf(v.x) < 1000000.0f && fabsf(v.y) < 1000000.0f && fabsf(v.z) < 1000000.0f &&
           Vector3::SqrMagnitude(v) > 0.0001f;
}

bool isInsideFOV(int x, int y) {
    const float fov = GetAimFov();
    if (!fov)
        return true;
    int circle_x = get_width() / 2;
    int circle_y = get_height() / 2;
    int rad = (int)fov;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

uintptr_t GetInsideFOVTarget() {
    uintptr_t result = 0;
    float MaxDist = std::numeric_limits<float>::infinity();
    auto Gameplay_get_MatchGame = (uintptr_t (*)()) (Class_Gameplay_get_MatchGame);
    auto get_MatchGame = Gameplay_get_MatchGame();
    if (Tools::IsPtrValid((void *) get_MatchGame)) {
        auto Gameplay_get_LocalPawn = (uintptr_t (*)()) (Class_Gameplay_get_LocalPawn);
        auto LocalPawn = Gameplay_get_LocalPawn();
        if (LocalPawn) {
            Vector3 MyPos{0, 0, 0};
            auto local_m_Mesh = *(Transform **) (LocalPawn + Class_Pawn_m_Mesh);
            if (local_m_Mesh) {
                MyPos = local_m_Mesh->get_position();
            }
            auto EnemyPawns = *(List<uintptr_t> **) (get_MatchGame + Class_BaseGame_EnemyPawns);
            if (EnemyPawns) {
                auto Items = EnemyPawns->getItems();
                if (Items) {
                    for (int i = 0; i < EnemyPawns->getSize(); i++) {
                        auto Pawn = Items[i];
                        if (Pawn) {
                            auto isAlive = *(bool*) ((uintptr_t)Pawn + Class_Pawn_m_IsAlive);
                            if (!isAlive)
                                continue;
                            auto m_HeadBone = *(Transform **) (Pawn + Class_Pawn_m_HeadBone);
                            if (!Tools::IsPtrValid(m_HeadBone))
                                continue;
                            auto main = Camera::get_main();
                            if (!Tools::IsPtrValid(main))
                                return result;
                            auto HeadSc = main->WorldToScreenPoint(m_HeadBone->get_position());
                            Vector2 v2Middle = Vector2((float) (get_width() / 2), (float) (get_height() / 2));
                            Vector2 v2Loc = Vector2(HeadSc.x, HeadSc.y);
                            if (isInsideFOV((int) HeadSc.x, (int) HeadSc.y)) {
                                float Distance = Vector2::Distance(v2Middle, v2Loc);
                                if (Distance < MaxDist) {
                                    result = Pawn;
                                    MaxDist = Distance;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

void DrawAimLine(ImDrawList* draw, int sWidth, int sHeight) {
    uintptr_t Target = 0;
    if (Config.Aim.By == EAim::Distance) {
        Target = GetClosestTarget();
    } else if (Config.Aim.By == EAim::Crosshair) {
        Target = GetInsideFOVTarget();
    }
    if (Target) {
        Vector3 targetPos;
        auto m_HeadBone = *(Transform**)(Target + Class_Pawn_m_HeadBone);
        if (!m_HeadBone) return;
        if (Config.Aim.Target == EAimTarget::Heads) {
            targetPos = m_HeadBone->get_position();
        } else if (Config.Aim.Target == EAimTarget::Chests) {
            targetPos = m_HeadBone->get_position();
            targetPos.y -= 0.2f;
        } else if (Config.Aim.Target == EAimTarget::Body) {
            targetPos = m_HeadBone->get_position();
            targetPos.y -= 0.4f;
        }
        auto HeadSc = Camera::get_main()->WorldToScreenPoint(targetPos);
        if (HeadSc.z > 0) {
            ImVec2 center(sWidth / 2, sHeight / 2);
            draw->AddLine(center, ImVec2(HeadSc.x, sHeight - HeadSc.y), ToColor(Config.sColorsESPPLAYER.LinePLAYER), Config.sESPMenuLineScale.lineSize);
        }
    }
}

ImVec2 pushToScreenBorder(ImVec2 Pos, ImVec2 screen, int borders, int offset) {
    float x = Pos.x;
    float y = Pos.y;
    if ((borders & 1) == 1) {
        y = 0 - offset;
    }
    if ((borders & 2) == 2) {
        x = screen.x + offset;
    }
    if ((borders & 4) == 4) {
        y = screen.y + offset;
    }
    if ((borders & 8) == 8) {
        x = 0 - offset;
    }
    return ImVec2(x, y);
}

int isOutsideSafezone(ImVec2 pos, ImVec2 screen) {
    ImVec2 mSafezoneTopLeft(screen.x * 0.04f, screen.y * 0.04f);
    ImVec2 mSafezoneBottomRight(screen.x * 0.96f, screen.y * 0.96f);
    int result = 0;
    if (pos.y < mSafezoneTopLeft.y) {
        result |= 1;
    }
    if (pos.x > mSafezoneBottomRight.x) {
        result |= 2;
    }
    if (pos.y > mSafezoneBottomRight.y) {
        result |= 4;
    }
    if (pos.x < mSafezoneTopLeft.x) {
        result |= 8;
    }
    return result;
}

class FPSCounter {
protected:
    unsigned int m_fps;
    unsigned int m_fpscount;
    long m_fpsinterval;

public:
    FPSCounter() : m_fps(0), m_fpscount(0), m_fpsinterval(0) {
    }

    void update() {
        m_fpscount++;
        if (m_fpsinterval < time(0)) {
            m_fps = m_fpscount;
            m_fpscount = 0;
            m_fpsinterval = time(0) + 1;
        }
    }

    unsigned int get() const {
        return m_fps;
    }
};

FPSCounter fps;

inline unsigned int GetFpsValue()
{
    return fps.get();
}

ImColor outlinecolor = IM_COL32(10, 10, 10, 255);

void HandleEnemyInfo(ImDrawList *draw) {
    Vector3 pLocalPawn_rootPos = Vector3::zero();
    Pawn *get_LocalPawn = GamePlay::get_LocalPawn();
    if (get_LocalPawn != nullptr) {
        pLocalPawn_rootPos = get_LocalPawn->get_LastPawnPos();
    }

    int enemyBots = 0;
    int enenyEnemies = 0;

    BaseGame *get_MatchGame = GamePlay::get_MatchGame();
    if (get_MatchGame != nullptr) {
        List<Pawn *> *EnemyPawns = get_MatchGame->EnemyPawns();
        if (EnemyPawns != nullptr) {
            Pawn **pawns = (Pawn **) EnemyPawns->getItems();
            for (int i = 0; i < EnemyPawns->getSize(); i++) {
                Pawn *pawn = pawns[i];
                if (pawn != nullptr) {
                    Vector3 pEnemyPawn_headPos = pawn->get_HeadPosition();
                    Vector3 pEnemyPawn_rootPos = pawn->get_LastPawnPos();

                    bool isBot = *(bool*)((uintptr_t)pawn + Class_Pawn_m_IsBot);
                    if (isBot) {
                        enemyBots++;
                    } else {
                        enenyEnemies++;
                    }

                    bool isAlive = *(bool*)((uintptr_t)pawn + Class_Pawn_m_IsAlive);
                    if (isAlive) {
                        enemyBots++;
                    } else {
                        enenyEnemies++;
                    }

                    auto mainCamera = Camera::get_main();
                    if (mainCamera) {
                        Vector3 HeadSc = mainCamera->WorldToScreenPoint(pEnemyPawn_headPos);
                        Vector3 RootSc = mainCamera->WorldToScreenPoint(pEnemyPawn_rootPos);

                        AttackableTargetInfo *m_AttackableInfo = pawn->m_AttackableInfo();
                        if (m_AttackableInfo != nullptr) {
                            bool isBot = false;
                            try {
                                isBot = *(bool*)((uintptr_t)pawn + Class_Pawn_m_IsBot);
                            } catch (...) {
                                isBot = true;
                            }

                            bool isAlive = false;
                            try {
                                isAlive = *(bool*)((uintptr_t)pawn + Class_Pawn_m_IsAlive);
                            } catch (...) {
                                isAlive = true;
                            }

                            if (Tools::IsPtrValid(pawn) && Tools::IsPtrValid(pawn->get_PlayerName())) {
                                std::string playerName = pawn->get_PlayerName()->CString();

                                auto textSize = ImGui::CalcTextSize(playerName.c_str(), 0, 18.0f);
                                float namelength = 14.0f;
                                if (playerName.length() <= 13) {
                                    namelength = 14.f;
                                } else if (playerName.length() <= 17) {
                                    namelength = 12.f;
                                } else if (playerName.length() <= 20) {
                                    namelength = 11.f;
                                } else if (playerName.length() <= 25) {
                                    namelength = 10.5f;
                                } else {
                                    namelength = 10.f;
                                }

                                if (HeadSc.z < 0) continue;
                                DrawTextWithBorder1(draw, playerName, {HeadSc.x - 43.0f, glHeight - HeadSc.y - 28.0f}, IM_COL32(255, 255, 255, 255), outlinecolor, 18.0f);

                            }
                        }
                    }
                }
            }
        }
    }
}

void DrawBoxEnemy(ImDrawList *draw, ImVec2 X, ImVec2 Y, float thicc, int color) {
    draw->AddLine({X.x, X.y}, {Y.x, Y.y}, color, thicc);
}

static inline void DrawEspBoxOutline(ImDrawList *draw, float x, float y, float w, float h, ImU32 color, float thickness) {
    draw->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0.0f, 0, thickness);
}

static inline void DrawEspBoxFilled(ImDrawList *draw, float x, float y, float w, float h, ImU32 color, float thickness) {
    ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
    draw->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, 0.16f)));
    draw->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0.0f, 0, thickness);
}

static inline void DrawEspBoxCorner(ImDrawList *draw, float x, float y, float w, float h, ImU32 color, float thickness) {
    float iw = w / 4.0f;
    float ih = h / 4.0f;
    draw->AddLine(ImVec2(x, y), ImVec2(x + iw, y), color, thickness);
    draw->AddLine(ImVec2(x + w - iw, y), ImVec2(x + w, y), color, thickness);
    draw->AddLine(ImVec2(x, y), ImVec2(x, y + ih), color, thickness);
    draw->AddLine(ImVec2(x + w - 1.0f, y), ImVec2(x + w - 1.0f, y + ih), color, thickness);
    draw->AddLine(ImVec2(x, y + h), ImVec2(x + iw, y + h), color, thickness);
    draw->AddLine(ImVec2(x + w - iw, y + h), ImVec2(x + w, y + h), color, thickness);
    draw->AddLine(ImVec2(x, y + h - ih), ImVec2(x, y + h), color, thickness);
    draw->AddLine(ImVec2(x + w - 1.0f, y + h - ih), ImVec2(x + w - 1.0f, y + h), color, thickness);
}

static inline void DrawEspVerticalHealthBar(ImDrawList *draw, float x, float y, float h, int curHP, int maxHP) {
    if (maxHP <= 0) return;
    float ratio = ImClamp(curHP / (float)maxHP, 0.0f, 1.0f);
    ImU32 hpColor = IM_COL32(std::min(510 * (maxHP - curHP) / maxHP, 255), std::min(510 * curHP / maxHP, 255), 0, 255);
    const float barW = 4.0f;
    draw->AddRectFilled(ImVec2(x, y), ImVec2(x + barW, y + h), IM_COL32(0, 0, 0, 120));
    draw->AddRect(ImVec2(x, y), ImVec2(x + barW, y + h), IM_COL32(0, 0, 0, 220), 0.0f, 0, 1.0f);
    draw->AddRectFilled(ImVec2(x, y + h * (1.0f - ratio)), ImVec2(x + barW, y + h), hpColor);
}

static inline void DrawEsp3DBox(ImDrawList *draw, Vector3 rootPos, int glHeight, ImU32 color, float thickness) {
    Camera* mainCamera = Camera::get_main();
    if (!mainCamera) return;

    Vector3 minBounds(-0.5f, 0.0f, -0.5f);
    Vector3 maxBounds(0.5f, 1.8f, 0.5f);
    Vector3 corners[8] = {
        rootPos + Vector3(minBounds.x, minBounds.y, minBounds.z),
        rootPos + Vector3(maxBounds.x, minBounds.y, minBounds.z),
        rootPos + Vector3(maxBounds.x, maxBounds.y, minBounds.z),
        rootPos + Vector3(minBounds.x, maxBounds.y, minBounds.z),
        rootPos + Vector3(minBounds.x, minBounds.y, maxBounds.z),
        rootPos + Vector3(maxBounds.x, minBounds.y, maxBounds.z),
        rootPos + Vector3(maxBounds.x, maxBounds.y, maxBounds.z),
        rootPos + Vector3(minBounds.x, maxBounds.y, maxBounds.z)
    };

    Vector3 screenCorners[8];
    for (int i = 0; i < 8; ++i) {
        screenCorners[i] = mainCamera->WorldToScreenPoint(corners[i]);
        if (screenCorners[i].z <= 0.0f) return;
        screenCorners[i].y = glHeight - screenCorners[i].y;
    }

    auto Draw3DLine = [&](int a, int b) {
        draw->AddLine(ImVec2(screenCorners[a].x, screenCorners[a].y), ImVec2(screenCorners[b].x, screenCorners[b].y), color, thickness);
    };

    Draw3DLine(0, 1); Draw3DLine(1, 5); Draw3DLine(5, 4); Draw3DLine(4, 0);
    Draw3DLine(3, 2); Draw3DLine(2, 6); Draw3DLine(6, 7); Draw3DLine(7, 3);
    Draw3DLine(0, 3); Draw3DLine(1, 2); Draw3DLine(4, 7); Draw3DLine(5, 6);
}

static inline ImU32 TeamIdColor(unsigned int teamSeatId) {
    static const ImU32 colors[] = {
        IM_COL32(245, 0, 0, 255),       // Red
        IM_COL32(255, 222, 82, 255),    // Yellow
        IM_COL32(116, 213, 82, 255),    // Green
        IM_COL32(33, 135, 220, 255),    // Blue
        IM_COL32(194, 94, 224, 255),    // Purple
        IM_COL32(255, 148, 0, 255),     // Orange
        IM_COL32(48, 239, 107, 255),    // Lime
        IM_COL32(82, 204, 211, 255),    // Aqua
        IM_COL32(0, 132, 136, 255),     // Teal
        IM_COL32(0, 0, 0, 255),         // Black
        IM_COL32(255, 255, 255, 255),   // White
        IM_COL32(128, 103, 92, 255),    // Brown
        IM_COL32(255, 158, 154, 255),   // Peach
        IM_COL32(139, 22, 20, 255),     // Maroon
        IM_COL32(172, 172, 172, 255),   // Gray
        IM_COL32(151, 169, 176, 255),   // Blue gray
        IM_COL32(106, 190, 95, 255),    // Pea green
        IM_COL32(38, 227, 225, 255),    // Cyan
        IM_COL32(0, 0, 76, 255),        // Navy blue
        IM_COL32(233, 75, 147, 255),    // Pink
        IM_COL32(222, 167, 0, 255),     // Mustard
        IM_COL32(255, 156, 154, 255),   // Coral
        IM_COL32(13, 0, 112, 255),      // Indigo
        IM_COL32(235, 77, 196, 255)     // Hot pink
    };
    if (teamSeatId == 0) return IM_COL32(115, 0, 0, 255);
    return colors[(teamSeatId - 1) % IM_ARRAYSIZE(colors)];
}

static inline void DrawTeamSeatId(ImDrawList *draw, float x, float y, unsigned int teamSeatId) {
    std::string text = std::to_string((int)teamSeatId);
    DrawBoxEnemy(draw, ImVec2(x - 40.0f, y - 16.0f), ImVec2(x - 80.0f, y - 16.0f), 22.0f, TeamIdColor(teamSeatId));
    DrawTextWithBorder1(draw, text, Vector2(x - 64.0f, y - 24.0f), IM_COL32(255, 255, 255, 255), IM_COL32(0, 0, 0, 255), 15.0f);
}

static inline void DrawPlayerHeader(ImDrawList *draw, Pawn *pawn, float centerX, float headY, float boxWidth, float distanceToMe, int curHP, int maxHP) {
    if (!pawn || maxHP <= 0) return;

    const float scale = ImClamp(1.0f - ((distanceToMe - 10.0f) * 0.012f), 0.62f, 1.0f);
    const float fontSize = 15.0f * scale;
    const float headerH = 22.0f * scale;
    const float healthH = 5.0f * scale;
    const float gap = 3.0f * scale;
    const float idW = 0.0f;
    const float distW = Config.ESPMenu.Distance ? 48.0f * scale : 0.0f;
    const float minNameW = Config.ESPMenu.Name ? 82.0f * scale : 0.0f;
    const float headerW = ImClamp(boxWidth * 1.85f, idW + distW + minNameW, 210.0f * scale);
    const float x = centerX - headerW * 0.5f;
    const float y = headY - ((headerH + gap + healthH) + 14.0f * scale);
    const float nameW = ImMax(0.0f, headerW - idW - distW - (Config.ESPMenu.Name ? 4.0f * scale : 0.0f));

    float cursorX = x;
    if (Config.ESPMenu.Name) {
        draw->AddRectFilled(ImVec2(cursorX, y), ImVec2(cursorX + nameW, y + headerH), IM_COL32(0, 0, 0, 210));
        std::string name = pawn->m_IsBot() ? "BOT" : pawn->get_PlayerName()->CString();
        ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
        nameSize.x *= fontSize / ImGui::GetFontSize();
        DrawTextWithBorder1(draw, name, Vector2(cursorX + (nameW - nameSize.x) * 0.5f, y + 3.0f * scale), IM_COL32(255, 255, 255, 255), IM_COL32(0, 0, 0, 255), fontSize);
        cursorX += nameW + 4.0f * scale;
    }

    if (Config.ESPMenu.Distance) {
        draw->AddRectFilled(ImVec2(cursorX, y), ImVec2(cursorX + distW, y + headerH), IM_COL32(0, 0, 0, 230));
        std::string dist = std::to_string((int)distanceToMe) + "m";
        ImVec2 distSize = ImGui::CalcTextSize(dist.c_str());
        distSize.x *= fontSize / ImGui::GetFontSize();
        DrawTextWithBorder1(draw, dist, Vector2(cursorX + (distW - distSize.x) * 0.5f, y + 3.0f * scale), IM_COL32(255, 255, 255, 255), IM_COL32(0, 0, 0, 255), fontSize);
    }

    if (Config.ESPMenu.Health && Config.ESPMenu.HealthPosition == EspHealthPosition::HealthTop) {
        float ratio = ImClamp(curHP / (float)maxHP, 0.0f, 1.0f);
        ImU32 hpColor = IM_COL32(std::min(510 * (maxHP - curHP) / maxHP, 255), std::min(510 * curHP / maxHP, 255), 0, 255);
        float hpY = y + headerH + gap;
        draw->AddRectFilled(ImVec2(x, hpY), ImVec2(x + headerW, hpY + healthH), IM_COL32(0, 0, 0, 210));
        draw->AddRectFilled(ImVec2(x, hpY), ImVec2(x + headerW * ratio, hpY + healthH), hpColor);
    }
}

static inline void DrawEspCylinder(ImDrawList* draw, Vector3 rootPos, int glHeight, ImU32 color) {
    Camera* mainCamera = Camera::get_main();
    if (!mainCamera) return;

    ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
    ImU32 edgeColor = ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, 0.72f));
    ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, 0.18f));
    const float height = 1.8f;
    const float radius = 0.6f;
    const int segments = 40;

    auto Project = [&](Vector3 world) -> ImVec2 {
        Vector3 screen = mainCamera->WorldToScreenPoint(world);
        if (screen.z <= 0.01f) return ImVec2(-9999.0f, -9999.0f);
        return ImVec2(screen.x, glHeight - screen.y);
    };

    std::vector<ImVec2> bottom;
    std::vector<ImVec2> top;
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * IM_PI * i) / segments;
        float dx = cosf(angle) * radius;
        float dz = sinf(angle) * radius;
        ImVec2 b = Project(rootPos + Vector3(dx, 0.0f, dz));
        ImVec2 t = Project(rootPos + Vector3(dx, height, dz));
        if (b.x < 0.0f || t.x < 0.0f) return;
        bottom.push_back(b);
        top.push_back(t);
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        ImVec2 quad[4] = { bottom[i], bottom[next], top[next], top[i] };
        draw->AddConvexPolyFilled(quad, 4, fillColor);
        draw->AddLine(bottom[i], bottom[next], edgeColor, 1.8f);
        draw->AddLine(top[i], top[next], edgeColor, 1.8f);
        draw->AddLine(bottom[i], top[i], edgeColor, 1.0f);
    }
}

static inline void DrawEspSignal(ImDrawList* draw, Vector3 rootPos, int glHeight, ImU32 color) {
    Camera* mainCamera = Camera::get_main();
    if (!mainCamera) return;

    ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);
    ImU32 outerColor = ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, 0.65f));
    ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, 0.10f));
    const int segments = 56;
    const float groundY = rootPos.y + 0.03f;

    auto BuildRing = [&](float radius, std::vector<ImVec2>& out) -> bool {
        out.clear();
        out.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            float angle = (2.0f * IM_PI * i) / segments;
            Vector3 world(rootPos.x + cosf(angle) * radius, groundY, rootPos.z + sinf(angle) * radius);
            Vector3 screen = mainCamera->WorldToScreenPoint(world);
            if (screen.z <= 0.01f) return false;
            out.emplace_back(screen.x, glHeight - screen.y);
        }
        return out.size() >= 3;
    };

    std::vector<ImVec2> outerRing;
    std::vector<ImVec2> innerRing;
    if (!BuildRing(1.2f, outerRing)) return;
    if (!BuildRing(0.7f, innerRing)) innerRing.clear();

    draw->AddConvexPolyFilled(outerRing.data(), (int)outerRing.size(), fillColor);
    draw->AddPolyline(outerRing.data(), (int)outerRing.size(), outerColor, true, 2.2f);
    if (!innerRing.empty()) {
        draw->AddPolyline(innerRing.data(), (int)innerRing.size(), outerColor, true, 1.6f);
    }
}

void DrawESP(ImDrawList *draw, int sWidth, int sHeight, float density) {
    ApplySpeedhack();
    BaseGame *get_MatchGame = GamePlay::get_MatchGame();
    if (!Tools::IsPtrValid(get_MatchGame)) return;

    bool MatchGame = Tools::IsPtrValid((void *)get_MatchGame);
    if (!MatchGame) return;

    Vector3 pLocalPawn_rootPos = Vector3::zero();
    Pawn *get_LocalPawn = GamePlay::get_LocalPawn();
    if (Tools::IsPtrValid(get_LocalPawn)) {
        pLocalPawn_rootPos = get_LocalPawn->get_LastPawnPos();
    }

    totalBots = 0, totalEnemies = 0;
    List<Pawn *> *EnemyPawns = get_MatchGame->EnemyPawns();
    if (!Tools::IsPtrValid(EnemyPawns) || !EnemyPawns->getSize()) return;

    Pawn **pawns = (Pawn **)EnemyPawns->getItems();
    for (int i = 0; i < EnemyPawns->getSize(); i++) {
        Pawn *pawn = pawns[i];
        if (!Tools::IsPtrValid(pawn)) continue;

        auto m_PlayerInfo = *(uintptr_t *)(pawn + Class_Pawn_m_PlayerInfo);
        if (!Tools::IsPtrValid((void *)m_PlayerInfo)) continue;

        auto m_AttackableInfo = *(uintptr_t *)(pawn + Class_AttackableTarget_m_AttackableInfo);
        int CurHP = (int)*(float *)(m_AttackableInfo + Class_AttackableTarget_m_Health);
        int MaxHP = (int)*(float *)(m_AttackableInfo + Class_AttackableTarget_m_MaxHealth);

        auto m_HeadBone = *(Transform **)(pawn + Class_Pawn_m_HeadBone);
        auto m_Mesh = *(Transform **)(pawn + Class_Pawn_m_Mesh);
        if (!Tools::IsPtrValid(m_HeadBone) || !m_Mesh || !*(bool *)(pawn + Class_Pawn_m_IsAlive)) continue;

        bool isBot = *(bool *)((uintptr_t)pawn + Class_Pawn_m_IsBot);
        bool isAlive = *(bool *)((uintptr_t)pawn + Class_Pawn_m_IsAlive);

        if (isBot) totalBots++; else totalEnemies++;

        ImU32 lineColor, boxColor, nameColor, distanceColor, healthColor, skeletonColor;
        float lineThickness, boxThickness, skeletonThickness;

        if (isBot) {
            lineColor = boxColor = ToColor(Config.sColorsESPBOT.LineBOT);
            nameColor = ToColor(Config.sColorsESPBOT.NameBOT);
            distanceColor = ToColor(Config.sColorsESPBOT.DistanceBOT);
            healthColor = ToColor(Config.sColorsESPBOT.HealthBOT);
            skeletonColor = ToColor(Config.sColorsESPBOT.SkeletonBOT);
            lineThickness = boxThickness = Config.Bline;
            skeletonThickness = Config.BskelLine;
        } else {
            lineColor = boxColor = ToColor(Config.sColorsESPPLAYER.LinePLAYER);
            nameColor = ToColor(Config.sColorsESPPLAYER.NamePLAYER);
            distanceColor = ToColor(Config.sColorsESPPLAYER.DistancePLAYER);
            healthColor = ToColor(Config.sColorsESPPLAYER.HealthPLAYER);
            skeletonColor = ToColor(Config.sColorsESPPLAYER.SkeletonPLAYER);
            lineThickness = boxThickness = Config.Pline;
            skeletonThickness = Config.PskelLine;
        }

        Vector3 pEnemyPawn_headPos = pawn->get_HeadPosition();
        Vector3 pEnemyPawn_rootPos = pawn->get_LastPawnPos();
        float distanceToMe = Vector3::Distance(pLocalPawn_rootPos, pEnemyPawn_rootPos);

        Vector3 HeadSc = Camera::get_main()->WorldToScreenPoint(pEnemyPawn_headPos);
        Vector3 RootSc = Camera::get_main()->WorldToScreenPoint(pEnemyPawn_rootPos);

		float actualHeight = Vector3::Distance(pEnemyPawn_headPos, pEnemyPawn_rootPos);

        if (HeadSc.z > 0 && pawn->m_IsAlive()) {

            AttackableTargetInfo *m_AttackableInfoPtr = pawn->m_AttackableInfo();
            if (m_AttackableInfoPtr != nullptr && isAlive) {
                if (Tools::IsPtrValid(pawn) && Tools::IsPtrValid(pawn->get_PlayerName())) {
                    std::string playerName = pawn->get_PlayerName()->CString();
                }
            }

            Vector2 screen(sWidth, sHeight);
            Vector2 location(RootSc.x, HeadSc.y);
            float magic_number = distanceToMe;
            float boxHeight = abs(HeadSc.y - RootSc.y);
            float boxWidth = boxHeight * 0.65f;

            if (actualHeight < 0.8f) {
                boxHeight = screenHeight * 0.5f;
                boxWidth = screenHeight * 1.2f;
            } else if (actualHeight < 1.2f) {
                boxHeight = screenHeight * 0.75f;
                boxWidth = screenHeight * 0.8f;
            }

            float mx = (glWidth / 6) / magic_number;
            float healthLength = glWidth / 20;
            if (healthLength < mx) healthLength = mx;

            Rect PlayerRect(HeadSc.x - (boxWidth / 2), sHeight - HeadSc.y, boxWidth, boxHeight);

            if (HeadSc.z > 0) {
                if (Config.ESPMenu.Alert) {
                    Vector3 angle = Vector3();
                    Vector3 forward = Vector3((float)(sWidth / 2) - HeadSc.x, (float)(sHeight / 2) - (sHeight - HeadSc.y), 0.0f);
                    VectorAnglesRadar(forward, angle);
                    const auto angle_yaw_rad = DEG2RAD(angle.y + 180.f);
                    const auto new_point_x = (sWidth / 2) + (55) / 2 * 8 * cosf(angle_yaw_rad);
                    const auto new_point_y = (sHeight / 2) + (55) / 2 * 8 * sinf(angle_yaw_rad);
                    std::array<Vector3, 3> points{
                        Vector3(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y - ((55) / 4 + 3.5f) / 2, 0.f),
                        Vector3(new_point_x + ((90) / 4 + 3.5f) / 4, new_point_y, 0.f),
                        Vector3(new_point_x - ((90) / 4 + 3.5f) / 2, new_point_y + ((55) / 4 + 3.5f) / 2, 0.f)
                    };
                    std::string strDistance;
                    auto textSize = ImGui::CalcTextSize(strDistance.c_str(), 0, ((float)density / 20.0f));
                    strDistance += std::to_string((int)distanceToMe) + "m";
                    draw->AddText(NULL, ((float)density / 20.0f), {new_point_x - (textSize.x / 2), new_point_y + 7.f}, IM_COL32(255, 255, 255, 255), strDistance.c_str());
                    RotateTriangle(points, angle.y + 180.f);
                    if (isBot) {
                        draw->AddTriangle(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), IM_COL32(0, 255, 0, 255), 1.5f);
                        draw->AddTriangleFilled(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), IM_COL32(0, 255, 0, 255));
                    } else {
                        draw->AddTriangle(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), IM_COL32(255, 0, 0, 255), 1.5f);
                        draw->AddTriangleFilled(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), IM_COL32(255, 0, 0, 255));
                    }
                }

                if (Config.ESPMenu.isPlayerLine && Config.ESPMenu.Target == LineTarget::Top) {
                    draw->AddLine(ImVec2(sWidth / 2, 80), ImVec2(HeadSc.x, sHeight - HeadSc.y), lineColor, lineThickness);
                }

                if (Config.ESPMenu.isPlayerLine && Config.ESPMenu.Target == LineTarget::Center) {
                    draw->AddLine(ImVec2(sWidth / 2, sHeight / 2), ImVec2(HeadSc.x, sHeight - HeadSc.y), lineColor, lineThickness);
                }

                if (Config.ESPMenu.isPlayerLine && Config.ESPMenu.Target == LineTarget::Bottom) {
                    draw->AddLine(ImVec2(sWidth / 2, sHeight), ImVec2(HeadSc.x, sHeight - RootSc.y), lineColor, lineThickness);
                }

                if (Config.ESPMenu.Crosshair || Config.Aim.By == EAim::Crosshair) {
                    ImVec2 center(get_width() / 2, get_height() / 2);
                    float radius = GetAimFov();
                    ImU32 color = ToColor(Config.sColorsESPOTHERS.PovOTHERS);
                    if (Config.ESPMenu.CrosshairType == CrosshairTarget::Circle) {
                        draw->AddCircle(center, radius, color, 60, 1.5f);
                    } else if (Config.ESPMenu.CrosshairType == CrosshairTarget::Cross) {
                        draw->AddLine(ImVec2(center.x - radius, center.y - radius), ImVec2(center.x + radius, center.y + radius), color, 1.5f);
                        draw->AddLine(ImVec2(center.x + radius, center.y - radius), ImVec2(center.x - radius, center.y + radius), color, 1.5f);
                    } else {
                        draw->AddLine(ImVec2(center.x - radius, center.y), ImVec2(center.x + radius, center.y), color, 1.5f);
                        draw->AddLine(ImVec2(center.x, center.y - radius), ImVec2(center.x, center.y + radius), color, 1.5f);
                    }
                }

                if (Config.ESPMenu.Aimline) {
                    DrawAimLine(draw, sWidth, sHeight);
                }

                if (Config.ESPMenu.Skeleton) {
    				AddSkeletonToDrawESP(draw, pawn, isBot, sHeight);
				}

				bool headerDistance = Config.ESPMenu.Distance && distanceToMe <= 120.0f;
				if ((Config.ESPMenu.Name || headerDistance || (Config.ESPMenu.Health && Config.ESPMenu.HealthPosition == EspHealthPosition::HealthTop)) && distanceToMe <= 120.0f) {
                    DrawPlayerHeader(draw, pawn, HeadSc.x, sHeight - HeadSc.y, boxWidth, distanceToMe, CurHP, MaxHP);
                }

                if (Config.ESPMenu.Health && Config.ESPMenu.HealthPosition == EspHealthPosition::HealthSide) {
                    DrawEspVerticalHealthBar(draw, PlayerRect.x - 8.0f, PlayerRect.y, PlayerRect.height, CurHP, MaxHP);
                }

                if (Config.ESPMenu.Distance && !headerDistance && distanceToMe <= 60.0f) {
                    float scaleFactor = 1.0f;
                    if (distanceToMe >= 19.0f) {
                        scaleFactor = 1.3f;
                    } else if (distanceToMe >= 17.0f) {
                        scaleFactor = 1.2f;
                    } else if (distanceToMe >= 15.0f) {
                        scaleFactor = 1.1f;
                    }

                    std::string s = std::to_string((int)distanceToMe) + "m";
                    float distFontSize = 17.0f * scaleFactor;
                    ImVec2 distTextSize = ImGui::CalcTextSize(s.c_str());
                    distTextSize.x *= (distFontSize / ImGui::GetFontSize());
                    distTextSize.y *= (distFontSize / ImGui::GetFontSize());
                    float distContainerWidth = distTextSize.x + (8.0f * scaleFactor);
                    float distContainerX = HeadSc.x - (distContainerWidth / 2);
                    float distBoxY = sHeight - RootSc.y + (8.0f * scaleFactor);
                    float distBoxHeight = distTextSize.y + (8.0f * scaleFactor);

                    draw->AddRectFilled(ImVec2(distContainerX, distBoxY), ImVec2(distContainerX + distContainerWidth, distBoxY + distBoxHeight), IM_COL32(0, 0, 0, 120));
                    float distTextX = distContainerX + (distContainerWidth - distTextSize.x) / 2;
                    draw->AddText(nullptr, distFontSize, ImVec2(distTextX, distBoxY + (4.0f * scaleFactor)), IM_COL32(255, 255, 255, 255), s.c_str());
                }

                if (Config.ESPMenu.Box) {
                    float x = RootSc.x - (boxWidth / 2.0f);
                    float y = sHeight - HeadSc.y;
                    float w = boxWidth;
                    float h = boxHeight;
                    if (Config.ESPMenu.BoxType == EspBoxType::Fill) {
                        DrawEspBoxFilled(draw, x, y, w, h, boxColor, boxThickness);
                    } else if (Config.ESPMenu.BoxType == EspBoxType::Outline) {
                        DrawEspBoxOutline(draw, x, y, w, h, boxColor, boxThickness);
                    } else if (Config.ESPMenu.BoxType == EspBoxType::ThreeD) {
                        DrawEsp3DBox(draw, pEnemyPawn_rootPos, sHeight, boxColor, boxThickness);
                    } else {
                        DrawEspBoxCorner(draw, x, y, w, h, boxColor, boxThickness);
                    }
                }

                if (Config.ESPMenu.EspStyle == EspStyleTarget::EspStyle3DSphere) {
                    DrawEspCylinder(draw, pEnemyPawn_rootPos, sHeight, boxColor);
                } else if (Config.ESPMenu.EspStyle == EspStyleTarget::EspStylePlayerSignal) {
                    DrawEspSignal(draw, pEnemyPawn_rootPos, sHeight, boxColor);
                }
            }
        }
    }

    if (Config.ESPMenu.Count) {
        int totalEnemyCount = totalBots + totalEnemies;

        float lineStartY = 80.0f;

        if (totalEnemyCount > 0) {
            float fontSize = 20.0f;

            char playerText[64];
            char botText[64];
            sprintf(playerText, "[Players] %d", totalEnemies);
            sprintf(botText, "[AI] %d", totalBots);

            ImVec2 playerSize = ImGui::CalcTextSize(playerText);
            playerSize.x *= (fontSize / ImGui::GetFontSize());
            playerSize.y *= (fontSize / ImGui::GetFontSize());

            ImVec2 botSize = ImGui::CalcTextSize(botText);
            botSize.x *= (fontSize / ImGui::GetFontSize());
            botSize.y *= (fontSize / ImGui::GetFontSize());

            float spacing = 18.0f;
            float totalWidth = playerSize.x + spacing + botSize.x;

            float textPosX = (sWidth - totalWidth) / 2.0f;
            float textPosY = 48.0f;

            ImU32 playerColor = IM_COL32(255, 255, 255, 255);
            ImU32 botColor = IM_COL32(80, 255, 80, 255);

            draw->AddText(NULL, fontSize, ImVec2(textPosX - 1, textPosY - 1), IM_COL32(0, 0, 0, 255), playerText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX + 1, textPosY - 1), IM_COL32(0, 0, 0, 255), playerText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX - 1, textPosY + 1), IM_COL32(0, 0, 0, 255), playerText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX + 1, textPosY + 1), IM_COL32(0, 0, 0, 255), playerText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX, textPosY), playerColor, playerText);

            float botTextPosX = textPosX + playerSize.x + spacing;

            draw->AddText(NULL, fontSize, ImVec2(botTextPosX - 1, textPosY - 1), IM_COL32(0, 0, 0, 255), botText);
            draw->AddText(NULL, fontSize, ImVec2(botTextPosX + 1, textPosY - 1), IM_COL32(0, 0, 0, 255), botText);
            draw->AddText(NULL, fontSize, ImVec2(botTextPosX - 1, textPosY + 1), IM_COL32(0, 0, 0, 255), botText);
            draw->AddText(NULL, fontSize, ImVec2(botTextPosX + 1, textPosY + 1), IM_COL32(0, 0, 0, 255), botText);
            draw->AddText(NULL, fontSize, ImVec2(botTextPosX, textPosY), botColor, botText);

        } else {
            const char* safeText = "[ SAFE ]";
            float fontSize = 25.0f;

            ImVec2 textSize = ImGui::CalcTextSize(safeText);
            textSize.x *= (fontSize / ImGui::GetFontSize());
            textSize.y *= (fontSize / ImGui::GetFontSize());

            float textPosX = (sWidth - textSize.x) / 2;
            float textPosY = lineStartY - textSize.y - 10.0f;

            draw->AddText(NULL, fontSize, ImVec2(textPosX - 1, textPosY - 1), IM_COL32(0, 0, 0, 255), safeText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX + 1, textPosY - 1), IM_COL32(0, 0, 0, 255), safeText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX - 1, textPosY + 1), IM_COL32(0, 0, 0, 255), safeText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX + 1, textPosY + 1), IM_COL32(0, 0, 0, 255), safeText);
            draw->AddText(NULL, fontSize, ImVec2(textPosX, textPosY), IM_COL32(0, 255, 0, 255), safeText);
        }
    }
}

template<typename ThisType, typename... Args>
void WeaponHandlerByAstral(ThisType thiz, Vector3 startPos, Vector3& dir, Args... args) {
    if (!Config.Aim.Aimbot360 && !Config.Aim.AimSilent) return;
    if (!Tools::IsPtrValid((void*)thiz)) return;

    auto get_MatchGame = ((uintptr_t(*)())(Class_Gameplay_get_MatchGame))();
    if (!Tools::IsPtrValid((void*)get_MatchGame)) return;

    auto LocalPawn = ((uintptr_t(*)())(Class_Gameplay_get_LocalPawn))();
    if (!Tools::IsPtrValid((void*)LocalPawn)) return;
    if (!*(bool*)(LocalPawn + Class_Pawn_m_IsAlive)) return;

    bool triggerReady = Config.Aim.Trigger == EAimTrigger::None;
    if (Config.Aim.Trigger == EAimTrigger::Shooting) {
        triggerReady = ((bool(*)(uintptr_t))(Class_Pawn_get_IsFiring))(LocalPawn);
    } else if (Config.Aim.Trigger == EAimTrigger::Scoping) {
        triggerReady = ((bool(*)(uintptr_t))(Class_Pawn_IsAiming))(LocalPawn);
    }

    if (!triggerReady) return;

    uintptr_t target = (Config.Aim.By == EAim::Distance) ? GetClosestTarget() : GetInsideFOVTarget();
    if (!Tools::IsPtrValid((void*)target)) return;
    if (!*(bool*)(target + Class_Pawn_m_IsAlive)) return;

    Vector3 targetPos;
    auto m_HeadBone = *(Transform**)(target + Class_Pawn_m_HeadBone);
    if (!Tools::IsPtrValid(m_HeadBone)) return;

    targetPos = m_HeadBone->get_position();
    if (Config.Aim.Target == EAimTarget::Chests) {
        targetPos.y -= 0.2f;
    } else if (Config.Aim.Target == EAimTarget::Body) {
        targetPos.y -= 0.4f;
    }

    auto main = Camera::get_main();
    if (!Tools::IsPtrValid(main))
        return;

    auto mainView = ((Component *) main)->get_transform();
    if (!Tools::IsPtrValid(mainView))
        return;

    Vector3 aimDir = targetPos - mainView->get_position();
    if (!IsAimVectorUsable(aimDir))
        return;

    if (Config.Aim.AimSilent) {
        dir = aimDir;
    }

    if (Config.Aim.Aimbot360) {
        auto Pawn_set_AimRotation = (void ( *)(uintptr_t, Quaternion))(Class_Pawn_set_AimRotation);
        if (Pawn_set_AimRotation)
            Pawn_set_AimRotation(LocalPawn, Quaternion::LookRotation(aimDir, Vector3::Up()));
    }
}

void WeaponFireComponent_Instant_CreateBulletLine(uintptr_t thiz, Vector3 startPos, Vector3 dir, bool isDualFire) {
    WeaponHandlerByAstral(thiz, startPos, dir, isDualFire);
    oWeaponFireComponent_Instant_CreateBulletLine(thiz, startPos, dir, isDualFire);
}

void WeaponFireComponent_Instant_CreateBulletProjectile(void* thiz, Vector3 startPos, Vector3 dir, void* weaponImpact, int itemID, int flySmokeAssetID, bool enableVirtualStartPos, Vector3 virtualStartPos) {
    WeaponHandlerByAstral(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
    oWeaponFireComponent_Instant_CreateBulletProjectile(thiz, startPos, dir, weaponImpact, itemID, flySmokeAssetID, enableVirtualStartPos, virtualStartPos);
}
