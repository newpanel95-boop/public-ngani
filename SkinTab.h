#pragma once
#include "../../ImGui/imgui.h"
#include "Keyboard.h"
#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstring>
#include <map>
#include "../../ImGui/imgui_settings.h"

extern float menu[4];
extern char searchQuery[256];
extern bool showKeyboard;
extern ImFont* F50;
extern ImFont* JAAT;
namespace font {
    extern ImFont* inter_semibold;
    extern ImFont* angas_ultra;
    extern ImFont* auto_techno;
}

static std::string activeInputID = "";
static std::map<std::string, bool> g_LastWeaponApplyState;

enum class SkinTier {
    None,
    Mythic,
    Legendary,
    Epic
};

static SkinTier GetSkinTier(const std::string& rawName) {
    if (rawName.find("[M]") != std::string::npos) return SkinTier::Mythic;
    if (rawName.find("[L]") != std::string::npos) return SkinTier::Legendary;
    if (rawName.find("[E]") != std::string::npos) return SkinTier::Epic;
    return SkinTier::None;
}

static std::string StripTag(const std::string& rawName) {
    std::string out = rawName;
    const char* tags[] = {"[M]", "[L]", "[E]", "[C]"};
    for (const char* tag : tags) {
        const std::string t(tag);
        const size_t pos = out.find(t);
        if (pos != std::string::npos) {
            out.erase(pos, t.length());
            while (!out.empty() && out.front() == ' ')
                out.erase(out.begin());
            while (!out.empty() && out.back() == ' ')
                out.pop_back();
            break;
        }
    }
    return out;
}

namespace runtime_preview_menu {

inline bool SearchField(const char *id, char *buf, size_t bufSize, const ImVec2 &size, const char *hint) {
    if (buf == nullptr || bufSize < 2) return false;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::PushID(id);
    const ImGuiID activeId = ImGui::GetID("##Active");
    static ImGuiID activeField = 0;
    ImGui::InvisibleButton("##Search", size);
    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = ImGui::IsItemClicked();
    if (clicked) activeField = activeId;
    if (ImGui::IsMouseClicked(0) && !hovered && activeField == activeId) activeField = 0;
    const bool active = activeField == activeId;
    bool changed = false;
    if (active) {
        ImGuiIO &io = ImGui::GetIO();
        for (int n = 0; n < io.InputQueueCharacters.Size; ++n) {
            const ImWchar c = io.InputQueueCharacters[n];
            if (c >= 32 && c < 127) {
                const size_t len = std::strlen(buf);
                if (len + 1 < bufSize) {
                    buf[len] = (char)c;
                    buf[len + 1] = '\0';
                    changed = true;
                }
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace, true)) {
            const size_t len = std::strlen(buf);
            if (len > 0) {
                buf[len - 1] = '\0';
                changed = true;
            }
        }
    }
    ImDrawList *draw = ImGui::GetWindowDrawList();
    const ImVec2 max = p + size;
    const float su = c::scale;
    draw->AddRectFilled(p, max, ImGui::GetColorU32(active ? c::raspody::widget_hover : c::raspody::widget), 8.0f * su);
    draw->AddRect(p, max, ImGui::GetColorU32(c::raspody::separator), 8.0f * su, 0, 1.0f * su);
    ImFont *iconFont = F107 ? F107 : ImGui::GetFont();
    ImFont *textFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
    const float iconSize = (F107 ? 13.0f : 11.0f) * su;
    const float textSize = textFont->FontSize * 0.82f * su;
    draw->AddText(iconFont, iconSize, ImVec2(p.x + 14.0f * su, p.y + (size.y - iconSize) * 0.5f), IM_COL32(151, 151, 179, 255), ICON_FA_SEARCH);
    draw->AddText(textFont, textSize, ImVec2(p.x + 42.0f * su, p.y + (size.y - textSize) * 0.5f), buf[0] ? IM_COL32(255, 255, 255, 245) : ImGui::GetColorU32(c::raspody::text), buf[0] ? buf : (hint ? hint : ""));
    ImGui::PopID();
    return changed;
}

inline bool SkinFilter(const char *id, int *current, const char *const items[], int count, const ImVec2 &size) {
    if (current == nullptr || items == nullptr || count <= 0) return false;
    *current = ImClamp(*current, 0, count - 1);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::PushID(id);
    ImGui::InvisibleButton("##Combo", size);
    const bool active = ImGui::IsItemActive();
    if (ImGui::IsItemClicked()) ImGui::OpenPopup("##Popup");

    ImDrawList *draw = ImGui::GetWindowDrawList();
    const float su = c::scale;
    draw->AddRectFilled(p, p + size, ImGui::GetColorU32(active ? c::raspody::widget_hover : c::raspody::widget), 8.0f * su);
    draw->AddRect(p, p + size, ImGui::GetColorU32(c::raspody::separator), 8.0f * su, 0, 1.0f * su);
    ImFont *iconFont = F107 ? F107 : ImGui::GetFont();
    ImFont *textFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
    const float textSize = textFont->FontSize * 0.82f * su;
    const float iconSize = (F107 ? 12.0f : 11.0f) * su;
    draw->AddText(textFont, textSize, ImVec2(p.x + 16.0f * su, p.y + (size.y - textSize) * 0.5f), IM_COL32(230, 234, 244, 245), items[*current]);
    draw->AddText(iconFont, iconSize, ImVec2(p.x + size.x - 28.0f * su, p.y + (size.y - iconSize) * 0.5f), IM_COL32(151, 151, 179, 255), ICON_FA_CHEVRON_DOWN);

    bool changed = false;
    ImGui::SetNextWindowPos(ImVec2(p.x, p.y + size.y + 5.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(size.x, ImMin(196.0f, 8.0f + count * 32.0f)), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, c::raspody::popup);
    ImGui::PushStyleColor(ImGuiCol_Border, c::raspody::separator);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 3.0f));
    if (ImGui::BeginPopup("##Popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove)) {
        ImDrawList *popupDraw = ImGui::GetWindowDrawList();
        for (int i = 0; i < count; ++i) {
            ImGui::PushID(i);
            const ImVec2 itemPos = ImGui::GetCursorScreenPos();
            const ImVec2 itemSize(ImGui::GetContentRegionAvail().x, 28.0f);
            ImGui::InvisibleButton("##Item", itemSize);
            const bool selected = i == *current;
            if (ImGui::IsItemClicked()) {
                *current = i;
                changed = true;
                ImGui::CloseCurrentPopup();
            }
            if (selected) popupDraw->AddRectFilled(itemPos, itemPos + itemSize, IM_COL32(44, 44, 52, 255), 5.0f);
            popupDraw->AddText(iconFont, F107 ? 9.0f : 8.0f, ImVec2(itemPos.x + 10.0f, itemPos.y + 9.0f), selected ? ImGui::GetColorU32(c::raspody::accent) : ImGui::GetColorU32(c::raspody::text), selected ? ICON_FA_CHECK : ICON_FA_CIRCLE);
            popupDraw->AddText(textFont, textFont->FontSize * 0.74f, ImVec2(itemPos.x + 28.0f, itemPos.y + 7.0f), selected ? IM_COL32(255, 255, 255, 255) : ImGui::GetColorU32(c::raspody::text3), items[i]);
            ImGui::PopID();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    ImGui::PopID();
    return changed;
}

inline const char *SkinLabel(SkinTier tier) {
    if (tier == SkinTier::Mythic) return "MYTHIC";
    if (tier == SkinTier::Legendary) return "LEGENDARY";
    if (tier == SkinTier::Epic) return "EPIC";
    return "COMMON";
}

inline ImU32 SkinBg(SkinTier tier) {
    if (tier == SkinTier::Mythic) return IM_COL32(122, 45, 55, 210);
    if (tier == SkinTier::Legendary) return IM_COL32(124, 96, 48, 210);
    if (tier == SkinTier::Epic) return IM_COL32(86, 70, 128, 210);
    return IM_COL32(48, 106, 84, 205);
}

inline ImU32 SkinText(SkinTier tier) {
    if (tier == SkinTier::Mythic) return IM_COL32(255, 190, 198, 255);
    if (tier == SkinTier::Legendary) return IM_COL32(246, 220, 166, 255);
    if (tier == SkinTier::Epic) return IM_COL32(221, 208, 255, 255);
    return IM_COL32(177, 235, 207, 255);
}

inline bool SkinMatches(const std::string &name, int rarityFilter) {
    const SkinTier tier = GetSkinTier(name);
    if (rarityFilter == 0) return true;
    if (rarityFilter == 1) return tier == SkinTier::Mythic;
    if (rarityFilter == 2) return tier == SkinTier::Legendary;
    if (rarityFilter == 3) return tier == SkinTier::Epic;
    return tier == SkinTier::None;
}

inline bool PopupButton(const char *id, const ImVec2 &size, const char *label, bool primary) {
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(id, size);
    const bool hovered = ImGui::IsItemHovered();
    const bool held = ImGui::IsItemActive();
    const bool clicked = ImGui::IsItemClicked();
    ImDrawList *draw = ImGui::GetWindowDrawList();

    ImU32 fill = primary ? Theme::GetAccentTintU32(0.72f, 0.98f) : ImGui::GetColorU32(c::field_bg);
    ImU32 fillHover = primary ? Theme::GetAccentTintU32(0.80f, 1.0f) : ImGui::GetColorU32(c::field_hover);
    ImU32 fillHeld = primary ? Theme::GetAccentTintU32(0.58f, 0.98f) : ImGui::GetColorU32(c::field_active);
    ImU32 border = primary ? Theme::GetAccentTintU32(0.94f, 0.58f) : IM_COL32(198, 202, 214, 112);
    draw->AddRectFilled(p, p + size, held ? fillHeld : hovered ? fillHover : fill, 8.0f);
    draw->AddRect(p, p + size, border, 8.0f, 0, primary ? 1.4f : 1.0f);

    ImFont *fontFace = font::angas_ultra ? font::angas_ultra : (font::auto_techno ? font::auto_techno : (font::inter_semibold ? font::inter_semibold : ImGui::GetFont()));
    const float textSize = fontFace->FontSize * 0.92f;
    const ImVec2 ts = fontFace->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);
    draw->AddText(fontFace, textSize, ImVec2(p.x + (size.x - ts.x) * 0.5f, p.y + (size.y - ts.y) * 0.5f), IM_COL32(248, 248, 252, 255), label);
    return clicked;
}

inline bool SkinTopButton(const char *id, const char *label, bool active, const ImVec2 &size) {
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(id, size);
    const bool hovered = ImGui::IsItemHovered();
    const bool clicked = ImGui::IsItemClicked();
    ImDrawList *draw = ImGui::GetWindowDrawList();
    const ImU32 fill = active ? ImGui::GetColorU32(c::raspody::widget_hover) : (hovered ? ImGui::GetColorU32(c::raspody::widget_hover) : ImGui::GetColorU32(c::raspody::widget));
    draw->AddRectFilled(p, p + size, fill, 20.0f);
    ImFont *fontFace = font::angas_ultra ? font::angas_ultra : (font::auto_techno ? font::auto_techno : (font::inter_semibold ? font::inter_semibold : ImGui::GetFont()));
    const float textSize = fontFace->FontSize * 0.92f;
    const ImVec2 text = fontFace->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);
    draw->AddText(fontFace, textSize, ImVec2(p.x + (size.x - text.x) * 0.5f, p.y + (size.y - text.y) * 0.5f), active ? IM_COL32(255, 255, 255, 255) : IM_COL32(235, 236, 240, 245), label);
    return clicked;
}

inline void Switch(ImDrawList *draw, const ImVec2 &trackMin, const ImVec2 &trackMax, bool value, float anim) {
    if (draw == nullptr) return;
    const float trackH = trackMax.y - trackMin.y;
    const float box = trackH;
    (void)box;
    draw->AddRectFilled(trackMin, trackMax, ImGui::GetColorU32(c::raspody::widget), trackH * 0.5f);
    if (anim > 0.01f)
        draw->AddRectFilled(trackMin, trackMax, ImGui::GetColorU32(ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.10f * anim)), trackH * 0.5f);
    const float knobR = 7.0f * c::scale;
    const float knobX = ImLerp(trackMin.x + 10.0f * c::scale, trackMax.x - 10.0f * c::scale, anim);
    draw->AddCircleFilled(ImVec2(knobX, trackMin.y + trackH * 0.5f), knobR, value ? ImGui::GetColorU32(c::raspody::accent) : ImGui::GetColorU32(c::raspody::text), 24);
}

inline std::string SkinName(const std::string &rawName) {
    std::string clean = StripTag(rawName);
    std::string out;
    out.reserve(ImMin((int)clean.size(), 56));
    bool lastWasSpace = false;
    for (unsigned char ch : clean) {
        if (out.size() >= 52) break;
        const bool allowed =
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') ||
            ch == ' ' || ch == '-' || ch == '_' || ch == ':' || ch == '.' || ch == '\'' || ch == '(' || ch == ')';
        if (allowed) {
            out.push_back((char)ch);
            lastWasSpace = ch == ' ';
        } else if (!lastWasSpace) {
            out.push_back(' ');
            lastWasSpace = true;
        }
    }
    while (!out.empty() && out.front() == ' ') out.erase(out.begin());
    while (!out.empty() && out.back() == ' ') out.pop_back();
    if (clean.size() > out.size() && out.size() >= 49) out += "...";
    return out.empty() ? "Unknown Skin" : out;
}

inline bool SkinRow(const char *id, const std::string &rawName, bool *value) {
    if (value == nullptr) return false;

    ImGui::PushID(id ? id : "##Result");
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        ImGui::PopID();
        return false;
    }

    ImGuiContext &g = *GImGui;
    const SkinTier tier = GetSkinTier(rawName);
    const std::string cleanLabel = SkinName(rawName);
    const float su = c::scale;
    const float rowH = 36.0f * su;
    const float rowW = ImMax(ImGui::GetContentRegionAvail().x, 1.0f);
    const ImVec2 rowPos = window->DC.CursorPos;
    const ImRect rowRect(rowPos, ImVec2(rowPos.x + rowW, rowPos.y + rowH));
    ImGui::ItemSize(rowRect, 0.0f);
    if (!ImGui::ItemAdd(rowRect, ImGui::GetID("##Row"))) {
        ImGui::PopID();
        return false;
    }

    const ImGuiID rowId = ImGui::GetID("##Row");
    bool hovered = false;
    bool held = false;
    const bool pressed = ImGui::ButtonBehavior(rowRect, rowId, &hovered, &held);
    if (pressed) {
        *value = !(*value);
        ImGui::MarkItemEdited(rowId);
    }

    ImGuiStorage *storage = ImGui::GetStateStorage();
    const float target = *value ? 1.0f : 0.0f;
    float anim = storage->GetFloat(rowId, target);
    const float follow = ImClamp(ImGui::GetIO().DeltaTime * 14.0f, 0.0f, 1.0f);
    anim += (target - anim) * follow;
    storage->SetFloat(rowId, anim);

    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImVec2 rowMin = rowRect.Min;
    const ImVec2 rowMax = rowRect.Max;
    const float labelStartX = rowMin.x + 6.0f * su;
    ImFont *resultFont = font::inter_semibold ? font::inter_semibold : ImGui::GetIO().FontDefault;
    const float badgeTextSizePx = 9.2f * su;
    const float resultTextSizePx = resultFont->FontSize * 0.86f * su;
    dl->AddLine(ImVec2(rowMin.x + 8.0f * su, rowMax.y), ImVec2(rowMax.x - 8.0f * su, rowMax.y), IM_COL32(154, 160, 170, 72), 1.15f * su);

    float textX = labelStartX;
    {
        const char *badgeText = SkinLabel(tier);
        ImU32 badgeBg = SkinBg(tier);
        ImU32 badgeFg = SkinText(tier);
        const ImVec2 badgeTextSize = resultFont->CalcTextSizeA(badgeTextSizePx, FLT_MAX, 0.0f, badgeText);
        const float badgePadX = 9.0f * su;
        const float badgePadY = 3.2f * su;
        const float badgeH = badgeTextSize.y + (badgePadY * 2.0f);
        const ImVec2 badgeMin(labelStartX + 8.0f * su, rowMin.y + (rowH - badgeH) * 0.5f);
        const ImVec2 badgeMax(badgeMin.x + ImMax(54.0f * su, badgeTextSize.x + (badgePadX * 2.0f)), badgeMin.y + badgeH);
        dl->AddRectFilled(badgeMin, badgeMax, badgeBg, 4.0f * su);
        dl->AddRect(badgeMin, badgeMax, IM_COL32(255, 255, 255, 18), 4.0f * su, 0, 1.0f);
        dl->AddText(resultFont, badgeTextSizePx, ImVec2(badgeMin.x + (badgeMax.x - badgeMin.x - badgeTextSize.x) * 0.5f, badgeMin.y + badgePadY), badgeFg, badgeText);
        textX = badgeMax.x + 10.0f * su;
    }

    const float switchW = 35.0f * su;
    const float switchH = 20.0f * su;
    const ImVec2 switchMin(rowMax.x - switchW - 4.0f * su, rowMin.y + (rowH - switchH) * 0.5f);
    const ImVec2 switchMax(switchMin.x + switchW, switchMin.y + switchH);
    {
        ImFont *dotsFont = F107 ? F107 : ImGui::GetFont();
        const char *dots = F107 ? ICON_FA_ELLIPSIS_H : "...";
        const float dotsSize = (F107 ? 12.0f : 13.0f) * su;
        const ImVec2 dotsText = dotsFont->CalcTextSizeA(dotsSize, FLT_MAX, 0.0f, dots);
        dl->AddText(dotsFont, dotsSize, ImVec2(switchMin.x - dotsText.x - 18.0f * su, rowMin.y + (rowH - dotsText.y) * 0.5f), IM_COL32(151, 151, 179, hovered ? 240 : 190), dots);
    }
    Switch(dl, switchMin, switchMax, *value, anim);

    const ImU32 nameColor = hovered ? IM_COL32(240, 232, 248, 255) : IM_COL32(236, 240, 246, 255);
    const ImVec2 nameSize = resultFont->CalcTextSizeA(resultTextSizePx, FLT_MAX, 0.0f, cleanLabel.c_str());
    const float nameY = rowMin.y + (rowH - nameSize.y) * 0.5f;
    dl->PushClipRect(ImVec2(textX, rowMin.y), ImVec2(switchMin.x - 44.0f * su, rowMax.y), true);
    dl->AddText(resultFont, resultTextSizePx, ImVec2(textX, nameY), nameColor, cleanLabel.c_str());
    dl->PopClipRect();

    ImGui::Dummy(ImVec2(0.0f, 2.0f * su));
    ImGui::PopID();
    return pressed;
}

inline void CharSkin(const charInfo &skin) {
    if (g_targetCharacters.empty()) return;
    if (g_selectedTargetCharIndex < 0 || g_selectedTargetCharIndex >= (int)g_targetCharacters.size()) g_selectedTargetCharIndex = 0;
    const TargetChar &target = g_targetCharacters[g_selectedTargetCharIndex];
    const bool targetIsCharly = target.name == "Charly" || target.name == "charly";
    const int selectedTraitor1P = targetIsCharly ? 710001101 : target.traitor1p;
    const int selectedTraitor3P = targetIsCharly ? 710001102 : target.traitor3p;
    const int selectedItemFieldsID = targetIsCharly ? 100301208 : target.itemID;
    const int selectedRoleFID = targetIsCharly ? 100301208 : target.roleID;
    const int selectedRolePackID = target.rolepackID;

    if (CharacterModelConfigInstance.empty() || itemResourceConfigInstance.empty() || RoleConfConfigInstance.empty()) return;

    CharacterModelFields *targetCharacter1P = nullptr;
    CharacterModelFields *targetCharacter3P = nullptr;
    for (auto charModel : CharacterModelConfigInstance) {
        if (!charModel || !Tools::IsPtrValid(charModel)) continue;
        CharacterModelFields *fields = (CharacterModelFields *)((uintptr_t)charModel + 0x10);
        if (!Tools::IsPtrValid(fields)) continue;
        if (fields->Traitor1P == selectedTraitor1P) targetCharacter1P = fields;
        if (fields->Traitor3P == selectedTraitor3P) targetCharacter3P = fields;
        if (!targetCharacter1P && fields->Traitor1P == 710001101) targetCharacter1P = fields;
        if (!targetCharacter3P && fields->Traitor1P == 710001102) targetCharacter3P = fields;
    }

    auto CharModel = [&](CharacterModelFields *fields) {
        if (!fields) return;
        fields->BRBagModel = skin.charModel[0];
        fields->BRHeadModel = skin.charModel[1];
        fields->BRLobby = skin.charModel[2];
        fields->BRModel = skin.charModel[3];
        fields->BindEffect1P = skin.charModel[4];
        fields->ChangeClipEffect1P = skin.charModel[5];
        fields->DefaultModelID = skin.charModel[6];
        fields->Guarder1P = skin.charModel[7];
        fields->Guarder3P = skin.charModel[8];
        fields->GuarderBagModel = skin.charModel[9];
        fields->GuarderHeadModel = skin.charModel[10];
        fields->GuarderLobby = skin.charModel[11];
    };
    CharModel(targetCharacter1P);
    CharModel(targetCharacter3P);

    for (auto itemRes : itemResourceConfigInstance) {
        if (!itemRes || !Tools::IsPtrValid(itemRes)) continue;
        itemFields = (ItemResourceFields *)((uintptr_t)itemRes + 0x10);
        if (!Tools::IsPtrValid(itemFields) || itemFields->ID != selectedItemFieldsID) continue;
        itemFields->FxAssetID = skin.charRes[0];
        itemFields->InventoryModelID = skin.charRes[1];
        itemFields->ModelAssetIDRaw = skin.charRes[2];
        itemFields->UIMiniSpriteName = skin.charRes2[0];
        itemFields->UISmallSpriteName = skin.charRes2[1];
        itemFields->UISpriteName = skin.charRes2[2];
        itemFields->UISquareSpriteName = skin.charRes2[3];
    }

    for (auto roles : RoleConfConfigInstance) {
        if (!roles || !Tools::IsPtrValid(roles)) continue;
        RoleConfFields *roleF = (RoleConfFields *)((uintptr_t)roles + 0x10);
        RoleConfFields *roleFAlt = (RoleConfFields *)((uintptr_t)roles + 0x14);
        auto applyRole = [&](RoleConfFields *role) {
            if (!role || role->ID != selectedRoleFID) return false;
            role->roleLeftArmID = skin.charRole[0];
            role->roleFinalSuitID = skin.charRole[1];
            role->roleBasicHologramID = skin.charRole[2];
            role->ColorID = skin.charRole[3];
            role->ColorSubID = skin.charRole[4];
            role->ShowRare = skin.charRole[5];
            role->RoleLvGroupID = skin.charRole[6];
            role->RolePackID = skin.charRole[7];
            role->LOCID_Name = skin.charRole2[0];
            return true;
        };
        if (!applyRole(roleF)) applyRole(roleFAlt);
    }

    for (auto pack : RolePackConfConfigInstance) {
        if (!pack || !Tools::IsPtrValid(pack)) continue;
        packfields = (RolePackFields *)((uintptr_t)pack + 0x10);
        if (!Tools::IsPtrValid(packfields) || packfields->RolePackID != selectedRolePackID) continue;
        packfields->EntryAnimID = skin.charPack[1];
        packfields->GestureId = skin.charPack[2];
        packfields->HandEffectUI = skin.charPack[3];
        packfields->LoadingFrame = skin.charPack[4];
        packfields->KillStreakSkinID = skin.charPack[5];
    }
}

inline bool SkinPtr(void *ptr) {
    return ptr && Tools::IsPtrValid(ptr);
}

inline void ApplyWeaponSkin(const itemInfo &skin) {
    const int baseWID = skin.WeaponConf[0];
    const int extraWID = skin.WeaponExtra[0];
    const int skinWID = skin.WeaponConf[2];
    const int itemID = skin.Item2Inventory[0];
    const int lootID = skin.Item2Inventory[3];

    auto matchId = [&](int id) {
        return id == baseWID || id == extraWID || id == skinWID || id == itemID || id == lootID;
    };
    auto mapId = [&](std::unordered_map<int, int> &dst, int value) {
        if (value <= 0) return;
        if (baseWID > 0) dst[baseWID] = value;
        if (extraWID > 0) dst[extraWID] = value;
        if (skinWID > 0) dst[skinWID] = value;
        if (itemID > 0) dst[itemID] = value;
        if (lootID > 0) dst[lootID] = value;
    };

    for (auto item : itemInventoryInstance) {
        if (!SkinPtr(item)) continue;
        item2Fields = (Item2InventoryFields*)((uintptr_t)item + 0x20);
        if (!SkinPtr(item2Fields)) continue;
        if (item2Fields->ItemID == itemID || item2Fields->ItemID == lootID) {
            item2Fields->WeaponAssetGroupID = skin.Item2Inventory[1];
            item2Fields->WeaponIconID = skin.Item2Inventory[2];
        }
    }

    for (auto conf : weaponConfInstance) {
        if (!SkinPtr(conf)) continue;
        weaponconfFields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
        if (!SkinPtr(weaponconfFields) || !matchId((int)weaponconfFields->ID)) continue;
        weaponconfFields->ColorID = skin.WeaponConf[1];
        weaponconfFields->DefWeaponSkinID = skin.WeaponConf[2];
        weaponconfFields->DefaultKillBrocast = skin.WeaponConf[3];
        for (auto skinConf : weaponConfInstance) {
            if (!SkinPtr(skinConf)) continue;
            WeaponConfFields *src = (WeaponConfFields*)((uintptr_t)skinConf + 0x20);
            if (SkinPtr(src) && (int)src->ID == skinWID) {
                weaponconfFields->LOCID_Name = src->LOCID_Name;
                break;
            }
        }
    }

    mapId(activeKillEffects, skin.WeaponExtra[4]);
    mapId(activeWeaponBrocast, skin.WeaponConf[3]);
    mapId(activeWeaponFireEffects, skin.WeaponAsset[1]);
    mapId(activeBulletTrackEffects, skin.WeaponAsset[1]);

    for (auto extra : weaponExtraInstance) {
        if (!SkinPtr(extra)) continue;
        weaponextraFields = (WeaponConfExtraFields*)((uintptr_t)extra + 0x10);
        if (!SkinPtr(weaponextraFields) || !matchId((int)weaponextraFields->ID)) continue;
        if (skin.itemName.find("[M]") != std::string::npos) {
            weaponextraFields->DefaultMythicArmor = skin.WeaponExtra[1];
            weaponextraFields->DefaultMythicSig = skin.WeaponExtra[2];
        }
        weaponextraFields->DefaultDeadReplayEffectId = skin.WeaponExtra[3];
        weaponextraFields->DefaultKillEffectId = skin.WeaponExtra[4];
    }

    for (auto asset : weaponAssetGroupInstance) {
        if (!SkinPtr(asset)) continue;
        weaponAssetFields = (WeaponAssetGroupFields*)((uintptr_t)asset + 0x40);
        if (!SkinPtr(weaponAssetFields)) continue;
        if (weaponAssetFields->Id == skin.WeaponAsset[0] && skin.WeaponAsset[1] > 0 && skin.WeaponAsset[1] / 10 == skin.WeaponAsset[2] / 10)
            weaponAssetFields->FireEffectGroupID = skin.WeaponAsset[1];
    }

    if (skin.WeaponAttach[0] > 0 || skin.WeaponAttach[1] > 0 || skin.WeaponAttach[2] > 0 || skin.WeaponAttach[3] > 0) {
        for (auto fireEffect : weaponFireEffectInstance) {
            if (!SkinPtr(fireEffect)) continue;
            const int fireEffectId = *(int*)((uintptr_t)fireEffect + 0x80);
            if (fireEffectId == skin.WeaponAsset[2] || fireEffectId == skin.WeaponAsset[1]) {
                *(int*)((uintptr_t)fireEffect + 0x64) = skin.WeaponAttach[0];
                *(int*)((uintptr_t)fireEffect + 0x68) = skin.WeaponAttach[1];
                *(int*)((uintptr_t)fireEffect + 0x6C) = skin.WeaponAttach[2];
                *(int*)((uintptr_t)fireEffect + 0x70) = skin.WeaponAttach[3];
            }
        }
    }

    for (auto itemResource : itemResourceConfigInstance) {
        if (!SkinPtr(itemResource)) continue;
        itemFields = (ItemResourceFields*)((uintptr_t)itemResource + 0x10);
        if (!SkinPtr(itemFields) || !matchId(itemFields->ID)) continue;
        itemFields->FxAssetID = skin.ItemResInt[0];
        itemFields->InventoryModelID = skin.ItemResInt[1];
        itemFields->ModelAssetIDRaw = skin.ItemResInt[2];
        itemFields->UIMiniSpriteName = skin.ItemRes[0];
        itemFields->UISmallSpriteName = skin.ItemRes[1];
        itemFields->UISpriteName = skin.ItemRes[2];
        itemFields->UISquareSpriteName = skin.ItemRes[3];
    }
}

inline void SelectSnowboardSkin(int skinId)
{
    void *selectedSkin = FindVehicleSkinConfigById(skinId);
    if (!SkinPtr(selectedSkin))
        return;

    auto *fields = (VehicleSkinConfFields *)((uintptr_t)selectedSkin + 0x10);
    if (!SkinPtr(fields))
        return;

    activeVehicleSkins[kSnowboardVehicleId] = skinId;
    activeVehicleSkinsById[(int)fields->VehicleId] = skinId;
    activeVehicleSkinConfs[skinId] = selectedSkin;
    RefreshSkisInstances();
}

inline void SkinContent(int categoryIndex) {
    static int rarityFilter = 0;
    static int vehicleRarityFilter = 0;
    static char charSearch[256] = "";
    static char vehicleSearch[256] = "";
    static char targetSearch[256] = "";
    static const char *filters[] = {"ALL", "MYTHIC", "LEGENDARY", "EPIC", "COMMON"};
    bool skinSearchHot = false;
    bool targetSearchHot = false;

    const float availW = ImGui::GetContentRegionAvail().x;
    const float searchH = 42.0f * c::scale;
    const float gap = 10.0f * c::scale;
    const bool charTab = categoryIndex == 1;
    const bool vehicleTab = categoryIndex == 2;
    const float filterW = ImClamp(availW * 0.19f, 150.0f, 210.0f);
    const float customW = ImClamp(availW * 0.36f, 220.0f, 330.0f);
    const float searchW = charTab ? ImMax(260.0f, availW - customW - filterW - gap * 2.0f) : ImMax(260.0f, availW - filterW - gap);
    char *skinQuery = vehicleTab ? vehicleSearch : (charTab ? charSearch : searchQuery);
    const char *searchHint = vehicleTab ? "Search vehicle skin..." : (charTab ? "Search character skin..." : "Search weapon skin...");

    SearchField("##Search_v39", skinQuery, vehicleTab ? IM_ARRAYSIZE(vehicleSearch) : (charTab ? IM_ARRAYSIZE(charSearch) : IM_ARRAYSIZE(searchQuery)), ImVec2(searchW, searchH), searchHint);
    const bool searchClicked = ImGui::IsItemClicked();
    const bool searchActive = ImGui::IsItemActive();
    const bool searchHovered = ImGui::IsItemHovered();
    skinSearchHot = searchClicked || searchActive || searchHovered;
    if (searchClicked || searchActive) {
        activeInputID = "skin";
        showKeyboard = true;
    }
    if (charTab) {
        ImGui::SameLine(0.0f, gap);
        SearchField("##runtime_target_char_search_v39", targetSearch, IM_ARRAYSIZE(targetSearch), ImVec2(customW, searchH), "custom char");
        const bool targetClicked = ImGui::IsItemClicked();
        const bool targetActive = ImGui::IsItemActive();
        const bool targetHovered = ImGui::IsItemHovered();
        targetSearchHot = targetClicked || targetActive || targetHovered;
        if (targetClicked || targetActive) {
            activeInputID = "target";
            showKeyboard = true;
        }
    }
    ImGui::SameLine(0.0f, gap);
    int *activeRarityFilter = vehicleTab ? &vehicleRarityFilter : &rarityFilter;
    SkinFilter("##Combo", activeRarityFilter, filters, IM_ARRAYSIZE(filters), ImVec2(filterW, searchH));
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    if (charTab && !g_targetCharacters.empty()) {
        static std::vector<int> targetIndices;
        static std::vector<const char *> targetNames;
        targetIndices.clear();
        targetNames.clear();
        std::string targetQuery = targetSearch;
        for (char &ch : targetQuery) ch = (char)std::tolower((unsigned char)ch);
        targetNames.reserve(g_targetCharacters.size());
        targetIndices.reserve(g_targetCharacters.size());
        for (int i = 0; i < (int)g_targetCharacters.size(); ++i) {
            std::string name = g_targetCharacters[i].name;
            for (char &ch : name) ch = (char)std::tolower((unsigned char)ch);
            if (targetQuery.empty() || name.find(targetQuery) != std::string::npos) {
                targetIndices.push_back(i);
                targetNames.push_back(g_targetCharacters[i].name.c_str());
            }
        }
        if (!targetNames.empty()) {
            int currentInFiltered = 0;
            for (int i = 0; i < (int)targetIndices.size(); ++i) {
                if (targetIndices[i] == g_selectedTargetCharIndex) {
                    currentInFiltered = i;
                    break;
                }
            }
            if (SkinFilter("##TargetCombo", &currentInFiltered, targetNames.data(), (int)targetNames.size(), ImVec2(availW, searchH)))
                g_selectedTargetCharIndex = targetIndices[currentInFiltered];
        } else {
            ImGui::TextColored(ImVec4(0.78f, 0.70f, 0.92f, 1.0f), "No matching characters");
        }
        ImGui::Dummy(ImVec2(0.0f, 8.0f));
    }

    if (false && charTab) {
        if (!g_targetCharacters.empty()) {
            const float infoSize = searchH;
            const float targetSearchW = 0.0f;
            ImVec2 infoPos = ImGui::GetCursorScreenPos();
            (void)infoSize;
            (void)targetSearchW;
            /*
            ImGui::InvisibleButton("##Info", ImVec2(infoSize, infoSize));
            const bool infoHovered = ImGui::IsItemHovered();
            const bool infoClicked = ImGui::IsItemClicked();
            ImDrawList *infoDraw = ImGui::GetWindowDrawList();
            infoDraw->AddRectFilled(infoPos, infoPos + ImVec2(infoSize, infoSize), ImGui::GetColorU32(c::field_bg), 7.0f);
            infoDraw->AddRect(infoPos, infoPos + ImVec2(infoSize, infoSize), IM_COL32(154, 160, 170, 100), 7.0f, 0, 1.0f);
            ImFont *infoIconFont = F107 ? F107 : ImGui::GetFont();
            const char *infoIcon = ICON_FA_INFO_CIRCLE;
            const float infoIconSize = F107 ? 15.0f : 13.0f;
            const ImVec2 infoText = infoIconFont->CalcTextSizeA(infoIconSize, FLT_MAX, 0.0f, infoIcon);
            infoDraw->AddText(infoIconFont, infoIconSize, ImVec2(infoPos.x + (infoSize - infoText.x) * 0.5f, infoPos.y + (infoSize - infoText.y) * 0.5f), IM_COL32(178, 184, 196, 255), infoIcon);
            if (infoClicked) ImGui::OpenPopup("##Popup");
            ImGui::SameLine(0.0f, gap);
            */
            ImGuiViewport *vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(520.0f, 420.0f), ImGuiCond_Always);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, c::panel_bg);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(154.0f / 255.0f, 160.0f / 255.0f, 170.0f / 255.0f, 0.58f));
            ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.03f, 0.03f, 0.05f, 0.72f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 18.0f));
            if (ImGui::BeginPopupModal("##Popup", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove)) {
                ImDrawList *popupDraw = ImGui::GetWindowDrawList();
                const ImVec2 popMin = ImGui::GetWindowPos();
                const ImVec2 popSize = ImGui::GetWindowSize();
                popupDraw->AddRect(popMin, popMin + popSize, Theme::GetAccentTintU32(0.8f, 0.34f), 14.0f, 0, 1.2f);
                popupDraw->AddRectFilled(popMin, popMin + popSize, IM_COL32(19, 21, 29, 245), 14.0f);
                popupDraw->AddRect(popMin, popMin + popSize, IM_COL32(255, 255, 255, 18), 14.0f, 0, 1.0f);

                const ImVec2 closePos(popMin.x + popSize.x - 48.0f, popMin.y + 16.0f);
                ImGui::SetCursorScreenPos(closePos);
                ImGui::InvisibleButton("##Close", ImVec2(30.0f, 30.0f));
                const bool closeHover = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) ImGui::CloseCurrentPopup();
                popupDraw->AddCircleFilled(closePos + ImVec2(15.0f, 15.0f), 15.0f, closeHover ? IM_COL32(45, 49, 62, 245) : IM_COL32(24, 28, 38, 235), 24);
                ImFont *closeFont = F107 ? F107 : ImGui::GetFont();
                const char *closeIcon = F107 ? ICON_FA_TIMES : "x";
                const float closeSize = F107 ? 13.0f : 15.0f;
                const ImVec2 closeText = closeFont->CalcTextSizeA(closeSize, FLT_MAX, 0.0f, closeIcon);
                popupDraw->AddText(closeFont, closeSize, closePos + ImVec2((30.0f - closeText.x) * 0.5f, (30.0f - closeText.y) * 0.5f), IM_COL32(210, 216, 228, 245), closeIcon);

                ImFont *popupFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
                ImFont *titleFont = F50 ? F50 : popupFont;
                const float titleSize = F50 ? 24.0f : popupFont->FontSize * 1.34f;
                const char *title = "Tutorial";
                const ImVec2 titleText = titleFont->CalcTextSizeA(titleSize, FLT_MAX, 0.0f, title);
                ImGui::SetCursorScreenPos(ImVec2(popMin.x + (popSize.x - titleText.x) * 0.5f, popMin.y + 24.0f));
                popupDraw->AddText(titleFont, titleSize, ImGui::GetCursorScreenPos(), IM_COL32(248, 248, 252, 255), title);
                const char *subtitle = "Learn the basics before you start";
                const float subtitleSize = popupFont->FontSize * 0.82f;
                const ImVec2 subtitleText = popupFont->CalcTextSizeA(subtitleSize, FLT_MAX, 0.0f, subtitle);
                popupDraw->AddText(popupFont, subtitleSize, ImVec2(popMin.x + (popSize.x - subtitleText.x) * 0.5f, popMin.y + 58.0f), IM_COL32(168, 174, 190, 235), subtitle);

                auto drawStep = [&](int number, const char *heading, const char *body, const char *icon, float y) {
                    const ImVec2 cardMin(popMin.x + 28.0f, popMin.y + y);
                    const ImVec2 cardMax(popMin.x + popSize.x - 28.0f, cardMin.y + 74.0f);
                    popupDraw->AddRectFilled(cardMin, cardMax, IM_COL32(21, 24, 34, 238), 8.0f);
                    popupDraw->AddRect(cardMin, cardMax, IM_COL32(255, 255, 255, 20), 8.0f, 0, 1.2f);
                    const ImVec2 circle(cardMin.x + 35.0f, cardMin.y + 37.0f);
                    popupDraw->AddCircle(circle, 18.0f, Theme::GetAccentTintU32(1.0f, 0.86f), 32, 2.0f);
                    char numBuf[8] = {};
                    std::snprintf(numBuf, sizeof(numBuf), "%d", number);
                    const ImVec2 numSize = popupFont->CalcTextSizeA(19.0f, FLT_MAX, 0.0f, numBuf);
                    popupDraw->AddText(popupFont, 19.0f, circle - ImVec2(numSize.x * 0.5f, numSize.y * 0.5f), Theme::GetAccentTintU32(1.0f, 1.0f), numBuf);
                    popupDraw->AddText(popupFont, popupFont->FontSize * 1.02f, ImVec2(cardMin.x + 72.0f, cardMin.y + 15.0f), IM_COL32(245, 246, 250, 255), heading);
                    popupDraw->AddText(ImGui::GetFont(), ImGui::GetFontSize() * 0.92f, ImVec2(cardMin.x + 72.0f, cardMin.y + 39.0f), IM_COL32(176, 183, 198, 238), body);
                    if (F107 && icon) {
                        const ImVec2 iconSize = F107->CalcTextSizeA(24.0f, FLT_MAX, 0.0f, icon);
                        popupDraw->AddText(F107, 24.0f, ImVec2(cardMax.x - 42.0f - iconSize.x * 0.5f, cardMin.y + 37.0f - iconSize.y * 0.5f), Theme::GetAccentTintU32(1.0f, 0.92f), icon);
                    }
                };

                drawStep(1, "Search Target", "Find the character slot you want to replace.", ICON_FA_SEARCH, 96.0f);
                drawStep(2, "Select Slot", "Pick the matching character from the combo.", ICON_FA_LIST, 178.0f);
                drawStep(3, "Apply Skin", "Enable a skin result to inject it into that slot.", ICON_FA_MAGIC, 260.0f);

                popupDraw->AddLine(ImVec2(popMin.x + 20.0f, popMin.y + 356.0f), ImVec2(popMin.x + popSize.x - 20.0f, popMin.y + 356.0f), IM_COL32(255, 255, 255, 18), 1.0f);
                ImGui::SetCursorScreenPos(ImVec2(popMin.x + 28.0f, popMin.y + 372.0f));
                if (PopupButton("##Skip", ImVec2(190.0f, 42.0f), "Skip", false)) ImGui::CloseCurrentPopup();
                ImGui::SetCursorScreenPos(ImVec2(popMin.x + popSize.x - 28.0f - 190.0f, popMin.y + 372.0f));
                if (PopupButton("##Accept", ImVec2(190.0f, 42.0f), "Got it", true)) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
            ImGui::SameLine(0.0f, gap);

            static std::vector<int> targetIndices;
            static std::vector<const char *> targetNames;
            targetIndices.clear();
            targetNames.clear();
            std::string targetQuery = targetSearch;
            for (char &ch : targetQuery) ch = (char)std::tolower((unsigned char)ch);
            targetNames.reserve(g_targetCharacters.size());
            targetIndices.reserve(g_targetCharacters.size());
            for (int i = 0; i < (int)g_targetCharacters.size(); ++i) {
                std::string name = g_targetCharacters[i].name;
                for (char &ch : name) ch = (char)std::tolower((unsigned char)ch);
                if (targetQuery.empty() || name.find(targetQuery) != std::string::npos) {
                    targetIndices.push_back(i);
                    targetNames.push_back(g_targetCharacters[i].name.c_str());
                }
            }
            if (!targetNames.empty()) {
                int currentInFiltered = 0;
                for (int i = 0; i < (int)targetIndices.size(); ++i) {
                    if (targetIndices[i] == g_selectedTargetCharIndex) {
                        currentInFiltered = i;
                        break;
                    }
                }
                const float comboW = ImMax(260.0f, availW * 0.45f);
                if (SkinFilter("##TargetCombo", &currentInFiltered, targetNames.data(), (int)targetNames.size(), ImVec2(comboW, searchH))) {
                    g_selectedTargetCharIndex = targetIndices[currentInFiltered];
                }
            } else {
                ImGui::TextColored(ImVec4(0.78f, 0.70f, 0.92f, 1.0f), "No matching characters");
            }
            ImGui::Dummy(ImVec2(0.0f, 4.0f));
        } else {
            ImGui::TextColored(ImVec4(0.78f, 0.70f, 0.92f, 1.0f), "Loading target characters...");
            ImGui::Dummy(ImVec2(0.0f, 6.0f));
        }
    }

    char query[64] = {};
    std::snprintf(query, sizeof(query), "%s", skinQuery);
    for (char &ch : query) ch = (char)std::tolower((unsigned char)ch);

    if ((categoryIndex == 0 && itemData.empty()) || (categoryIndex == 1 && charData.empty()) || (categoryIndex == 2 && vehicleData.empty())) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.78f, 0.70f, 0.92f, 1.0f));
        ImGui::TextWrapped("Skin data is still loading. Open the related game inventory once and wait a moment.");
        ImGui::PopStyleColor();
        if (showKeyboard && ImGui::IsMouseClicked(0) && !skinSearchHot && !targetSearchHot && ImGui::GetIO().MousePos.y < ImGui::GetIO().DisplaySize.y * 0.515f) showKeyboard = false;
        if (showKeyboard) {
            if (activeInputID == "target") Keyboard("##Keyboard", targetSearch, IM_ARRAYSIZE(targetSearch), &showKeyboard);
            else if (charTab) Keyboard("##Keyboard", charSearch, IM_ARRAYSIZE(charSearch), &showKeyboard);
            else if (vehicleTab) Keyboard("##Keyboard", vehicleSearch, IM_ARRAYSIZE(vehicleSearch), &showKeyboard);
            else Keyboard("##Keyboard", searchQuery, IM_ARRAYSIZE(searchQuery), &showKeyboard);
        }
        return;
    }

    bool found = false;
    if (categoryIndex == 0) {
        for (int i = 0; i < (int)itemData.size(); ++i) {
            const auto &item = itemData[i];
            std::string lower = item.itemName;
            for (char &ch : lower) ch = (char)std::tolower((unsigned char)ch);
            if (lower.find(query) == std::string::npos || !SkinMatches(item.itemName, *activeRarityFilter)) continue;
            found = true;
            char rowId[48] = {};
            std::snprintf(rowId, sizeof(rowId), "weapon_%d", i);
            const bool changed = SkinRow(rowId, item.itemName, &sBool[item.itemName]);
            const bool selected = sBool[item.itemName];
            const auto stateIt = g_LastWeaponApplyState.find(item.itemName);
            const bool applyChanged = (stateIt == g_LastWeaponApplyState.end()) || stateIt->second != selected;
            if (applyChanged)
                g_LastWeaponApplyState[item.itemName] = selected;
            if (selected && (changed || applyChanged))
                ApplyWeaponSkin(item);
        }
    } else if (categoryIndex == 1) {
        for (int i = 0; i < (int)charData.size(); ++i) {
            const auto &character = charData[i];
            std::string lower = character.charName;
            for (char &ch : lower) ch = (char)std::tolower((unsigned char)ch);
            if (lower.find(query) == std::string::npos || !SkinMatches(character.charName, *activeRarityFilter)) continue;
            found = true;
            char rowId[48] = {};
            std::snprintf(rowId, sizeof(rowId), "character_%d", i);
            const bool changed = SkinRow(rowId, character.charName, &sBool[character.charName]);
            if (changed && sBool[character.charName]) CharSkin(character);
        }
    } else {
        for (int i = 0; i < (int)vehicleData.size(); ++i) {
            const auto &vehicle = vehicleData[i];
            std::string lower = vehicle.vehicleName;
            for (char &ch : lower) ch = (char)std::tolower((unsigned char)ch);
            if (lower.find(query) == std::string::npos || !SkinMatches(vehicle.vehicleName, *activeRarityFilter)) continue;
            found = true;
            char rowId[48] = {};
            std::snprintf(rowId, sizeof(rowId), "vehicle_%d", i);
            const bool changed = SkinRow(rowId, vehicle.vehicleName, &sBool[vehicle.vehicleName]);
            if (changed && sBool[vehicle.vehicleName])
                SelectSnowboardSkin(vehicle.skinId);
        }
    }
    if (!found) ImGui::TextWrapped("No results found for '%s'.", skinQuery);
    if (showKeyboard && ImGui::IsMouseClicked(0) && !skinSearchHot && !targetSearchHot && ImGui::GetIO().MousePos.y < ImGui::GetIO().DisplaySize.y * 0.515f) showKeyboard = false;
    if (showKeyboard) {
        if (activeInputID == "target") Keyboard("##Keyboard", targetSearch, IM_ARRAYSIZE(targetSearch), &showKeyboard);
        else if (vehicleTab) Keyboard("##Keyboard", vehicleSearch, IM_ARRAYSIZE(vehicleSearch), &showKeyboard);
        else if (charTab) Keyboard("##Keyboard", charSearch, IM_ARRAYSIZE(charSearch), &showKeyboard);
        else Keyboard("##Keyboard", searchQuery, IM_ARRAYSIZE(searchQuery), &showKeyboard);
    }
}

} // namespace runtime_preview_menu
