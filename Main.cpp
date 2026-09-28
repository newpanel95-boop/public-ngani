#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/AhookGate.h"
#include "ImGui/Call_ImGui.h"
#include "IL2CppSDKGenerator/BasicStructs/Call_BasicStructs.h"
#include "IL2CppSDKGenerator/IL2Cpp/Call_IL2Cpp.h"
#include "Hacks/Hacks.h"
#include "IL2CppSDKGenerator/KittyMemory/MemoryPatch.h"

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libgen.h>
#include <limits.h>
#include <string>
#include <functional>
#include <cstring>
#include <cfloat>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

#include "XD/hiderecord.h"
#include "ctorHook/ConstructorHook.hpp"
#include "ctorHook/ctorOffset.h"

class _BYTE;
class _BOOL4;
class _BOOL8;
class _WORD;
class _DWORD;
class _QWORD;

#define CREATE_COLOR(r, g, b, a) new float[4] {(float)(r) / 255.0f, (float)(g) / 255.0f, (float)(b) / 255.0f, (float)(a) / 255.0f}
bool ClearDisplay = true;
bool SnowB = false;
float SnowBsize = 0.0f;
bool isSpeedHackEnabled = false;
float speedHackMultiplier = 1.0f;
bool isJumpAdjustmentEnabled = false;
float jumpHeightMultiplier = 1.0f;
bool RedWallhackShow = false;
char logintext[4096];
float menu[4] = {103.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f, 1.0f};

float g_LastLogoOpacity = 1.0f;
float g_LastLogoSize = 1.0f;

bool fromOverlay = false;
bool g_OverlayDown = false;
float g_OverlayX = 0.0f;
float g_OverlayY = 0.0f;
float menuScale = 1.0f;
static ImVec2 MapMenuInput(float x, float y, float screenWidth, float screenHeight, float scale)
{
    (void)screenWidth;
    (void)screenHeight;

    if (scale <= 0.0f)
        scale = 1.0f;

    return ImVec2(x / scale, y / scale);
}

#define _BYTE uint8_t
#define _WORD  uint8_t
#define _DWORD uint64_t
#define _QWORD uint64_t
#define _BOOL4 uint8_t

#include <fstream>
using namespace std;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#include "Includes/Includes.h"
#include "ImGui/imgui_core.h"
#include "System/UI/TextureLoader.h"
#include "ImGui/Assets/Image/codm.h"

ImFont* F50 = nullptr;
ImFont* F107 = nullptr;
ImFont* SOCIAL = nullptr;
ImFont* Bold = nullptr;
JavaVM* jvm = nullptr;
JavaVM* VM = nullptr;

namespace font {
    ImFont* inter_semibold = nullptr;
    ImFont* auto_techno = nullptr;
    ImFont* techno_hideo = nullptr;
    ImFont* angas_ultra = nullptr;
}

static int g_GlWidth, g_GlHeight;
static bool g_App = false;
static constexpr int kEglWidthAttribute = 0x3057;
static constexpr int kEglHeightAttribute = 0x3056;

struct My_Patches
{
    MemoryPatch A1;
} Patches;


bool showKeyboard = false;
static bool g_RuntimeClearDisplayInit = false;

struct ClearDisplayDefaultInit {
    ClearDisplayDefaultInit()
    {
        Config.ExtraMenu.ClearDisplay = true;       
    }
} g_ClearDisplayDefaultInit;

struct sRegion
{
    uintptr_t start, end;
};

std::chrono::steady_clock::time_point appStartTime = std::chrono::steady_clock::now();

static bool windowCollapsed = false;
static double collapseBarLastActiveTime = 0.0;
static float collapseBarOpacityAnim = 1.0f;
static float collapseBarPressAnim = 0.0f;
static float collapseBarEnterAnim = 1.0f;
static float collapseBarRestoreAnim = 1.0f;
static bool collapseBarRestoreActive = false;
static bool collapseBarWasCollapsed = false;
static float uncollapseOpenAnim = 1.0f;
static bool dark = true;
static float tabAlpha = 1.0f;
static float tabAdd = 0.0f;
static int page = 1;
static int activeTab = 1;
static bool isLogin = false;
static bool g_MainUiWasVisible = false;
static std::string err;
static std::string storedKey = "";
static char s[256];
static bool g_LoginTextLoaded = false;
bool g_LogoPreviewMode = false;

std::vector<sRegion> trapRegions;
std::string md5(std::string s);
uintptr_t g_il2cpp;
static bool isMenuVisible = true;

void *g_EglBridgeHandle = nullptr;
QuerySurfaceFn g_QuerySurfaceFn = nullptr;
CallOldSwapFn g_CallOldSwapFn = nullptr;
InstallHookFn g_InstallHookFn = nullptr;

int hook_eglSwapBuffers(void *dpy, void *surface)
{
    if (g_QuerySurfaceFn != nullptr) {
        g_QuerySurfaceFn(dpy, surface, kEglWidthAttribute, &g_GlWidth);
        g_QuerySurfaceFn(dpy, surface, kEglHeightAttribute, &g_GlHeight);
    }

    if (!g_App)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        io.LogFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.MouseDoubleClickTime = 0.3f;
        io.MouseDragThreshold = 2.f;
        ImGui_ImplOpenGL3_Init("#version 300 es");

        ImFontConfig inter_config;
        inter_config.MergeMode = false;
        inter_config.PixelSnapH = true;
        inter_config.FontDataOwnedByAtlas = false;
        font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(
            (void*)inter_semibold,
            sizeof(inter_semibold),
            16.f,
            &inter_config
        );

        ImFontConfig techno_config;
        techno_config.MergeMode = false;
        techno_config.PixelSnapH = true;
        techno_config.FontDataOwnedByAtlas = false;
        font::auto_techno = io.Fonts->AddFontFromMemoryTTF(
            (void*)auto_techno,
            sizeof(auto_techno),
            21.f,
            &techno_config
        );

        ImFontConfig hideo_config;
        hideo_config.MergeMode = false;
        hideo_config.PixelSnapH = true;
        hideo_config.FontDataOwnedByAtlas = false;
        font::techno_hideo = io.Fonts->AddFontFromMemoryTTF(
            (void*)techno_hideo,
            sizeof(techno_hideo),
            22.f,
            &hideo_config
        );

        ImFontConfig angas_config;
        angas_config.MergeMode = false;
        angas_config.PixelSnapH = true;
        angas_config.FontDataOwnedByAtlas = false;
        font::angas_ultra = io.Fonts->AddFontFromMemoryTTF(
            (void*)angas_v3_ultra,
            sizeof(angas_v3_ultra),
            22.f,
            &angas_config
        );

        static const ImWchar icons_ranges[] = { 0XE000, 0XF8FF, 0 };
        ImFontConfig iconsConfig;
        iconsConfig.MergeMode = true;
        iconsConfig.PixelSnapH = true;
        iconsConfig.OversampleH = 2.5f;
        iconsConfig.OversampleV = 2.5f;
        iconsConfig.FontDataOwnedByAtlas = false;
        F107 = io.Fonts->AddFontFromMemoryCompressedTTF(
            (void*)font_awesome_data1,
            (int)font_awesome_size1,
            25.0f,
            &iconsConfig,
            icons_ranges
        );
        F50 = io.Fonts->AddFontFromMemoryTTF((void *)F50_data, F50_size, 30.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        if (!F107) {
            F107 = font::inter_semibold;
        }
        if (font::inter_semibold) {
            io.FontDefault = font::inter_semibold;
        }
        io.Fonts->Build();

        ImGui_ImplOpenGL3_CreateFontsTexture();

        memset(&Config, 0, sizeof(sConfig));

        Config.sColorsESPPLAYER.LinePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.BoxPLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.NamePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.DistancePLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.HealthPLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPPLAYER.SkeletonPLAYER = CREATE_COLOR(255, 0, 0, 255);
        Config.sColorsESPBOT.LineBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.BoxBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.NameBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.HealthBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.DistanceBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPBOT.SkeletonBOT = CREATE_COLOR(0, 255, 0, 180);
        Config.sColorsESPOTHERS.PovOTHERS = CREATE_COLOR(225, 0, 0, 180);

        Config.Aim.AimAssistSize = 0.0f;
        Config.Aim.Cross = 50.0f;
        Config.Aim.Target = EAimTarget::Heads;
        Config.Aim.Trigger = EAimTrigger::None;
        Config.Aim.By = EAim::Distance;

        Config.Bline = 2.0f;
        Config.Pline = 2.0f;

        g_App = true;
    }

    ImGuiIO *io = &ImGui::GetIO();
    screenWidth = (float)g_GlWidth;
    screenHeight = (float)g_GlHeight;
    io->DisplaySize = ImVec2((float)g_GlWidth, (float)g_GlHeight);
    fps.update();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImDrawList *draw = ImGui::GetBackgroundDrawList();

    DrawESP(ImGui::GetBackgroundDrawList(), screenWidth, screenHeight, get_dpi());
    floating_info::Render(draw, screenWidth, screenHeight);

    if (windowCollapsed)
    {
        if (!collapseBarWasCollapsed) {
            collapseBarEnterAnim = 0.0f;
            collapseBarRestoreAnim = 1.0f;
            collapseBarRestoreActive = false;
            collapseBarOpacityAnim = 1.0f;
            collapseBarLastActiveTime = ImGui::GetTime();
        }
        collapseBarWasCollapsed = true;
        const float dt = ImGui::GetIO().DeltaTime;
        collapseBarEnterAnim = ImClamp(collapseBarEnterAnim + dt * 6.0f, 0.0f, 1.0f);
        if (collapseBarRestoreActive)
            collapseBarRestoreAnim = ImClamp(collapseBarRestoreAnim + dt * 8.0f, 0.0f, 1.0f);
        const float enterEase = collapseBarEnterAnim * collapseBarEnterAnim * (3.0f - 2.0f * collapseBarEnterAnim);
        const float restoreEase = collapseBarRestoreAnim * collapseBarRestoreAnim * (3.0f - 2.0f * collapseBarRestoreAnim);
        const float collapsedAlphaSetting = ImClamp(GetLogoOpacity(), 0.0f, 1.0f);
        const float collapsedScaleSetting = ImClamp(GetLogoSizeMultiplier(), 0.1f, 2.0f);
        const ImVec2 display = ImGui::GetIO().DisplaySize;
        const float baseLineW = 152.0f * c::scale * collapsedScaleSetting;
        const float line_w = collapseBarRestoreActive ? ImLerp(baseLineW * 1.72f, baseLineW, restoreEase) : ImLerp(baseLineW * 1.72f, baseLineW, enterEase);
        const float line_h = collapseBarRestoreActive ? ImLerp(10.0f, 6.0f, restoreEase) : ImLerp(6.0f, 10.0f, enterEase);
        const float lineH = line_h * c::scale * collapsedScaleSetting;
        const float click_w = line_w + 72.0f * c::scale;
        const float click_h = ImMax(28.0f * c::scale, lineH + 20.0f * c::scale);
        // Keep the Show Menu handle at the TOP instead of the bottom.
        const float startY = 6.0f * c::scale;
        const float endY = display.y * 0.15f;
        const float handleY = collapseBarRestoreActive ? ImLerp(startY, endY, restoreEase) : ImLerp(endY, startY, enterEase);
        const ImVec2 handlePos((display.x - line_w) * 0.5f, handleY);
        const ImVec2 clickPos(handlePos.x - (click_w - line_w) * 0.5f, handlePos.y - (click_h - lineH) * 0.5f);

        ImGui::SetNextWindowPos(clickPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(click_w, click_h), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        if (ImGui::Begin("##indicator", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings))
        {
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::InvisibleButton("##restoreclick", ImVec2(click_w, click_h));

            bool barHeld = ImGui::IsItemActive();
            bool barHovered = ImGui::IsItemHovered();
            if (barHovered || barHeld)
                collapseBarLastActiveTime = ImGui::GetTime();
            const bool idle = (ImGui::GetTime() - collapseBarLastActiveTime) > 5.0;
            const float targetOpacity = idle ? 0.20f : 1.0f;
            collapseBarOpacityAnim = ImLerp(collapseBarOpacityAnim, targetOpacity, ImGui::GetIO().DeltaTime * 9.0f);
            collapseBarPressAnim = ImLerp(collapseBarPressAnim, barHeld ? 1.0f : 0.0f, ImGui::GetIO().DeltaTime * 22.0f);

            if (ImGui::IsItemClicked())
            {
                collapseBarRestoreActive = true;
                collapseBarRestoreAnim = 0.0f;
                collapseBarLastActiveTime = ImGui::GetTime();
            }

            float drawAlpha = ImClamp(collapseBarOpacityAnim * collapsedAlphaSetting, 0.0f, 1.0f);
            ImDrawList* indicatorDraw = ImGui::GetWindowDrawList();
            const ImVec2 lineMin(windowPos.x + (click_w - line_w) * 0.5f, windowPos.y + (click_h - lineH) * 0.5f);
            const ImVec2 lineMax(lineMin.x + line_w, lineMin.y + lineH);
            const float press = collapseBarPressAnim;
            indicatorDraw->AddRectFilled(
                lineMin,
                lineMax,
                IM_COL32(255, 255, 255, (int)((225.0f + 30.0f * press) * drawAlpha)),
                lineH * 0.5f
            );

            if (collapseBarRestoreActive && collapseBarRestoreAnim >= 1.0f)
            {
                windowCollapsed = false;
                isMenuVisible = true;
                uncollapseOpenAnim = 0.0f;
                collapseBarRestoreActive = false;
                collapseBarEnterAnim = 1.0f;
            }
        }
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);
    }
    else
    {
        collapseBarWasCollapsed = false;
        collapseBarEnterAnim = 1.0f;
    }

    if (isMenuVisible && !windowCollapsed)
    {
        if (!g_RuntimeClearDisplayInit) {
            Config.ExtraMenu.ClearDisplay = true;
            g_RuntimeClearDisplayInit = true;
        }

        if (!g_LoginTextLoaded && VM != nullptr)
        {
            if (LoadTextFromFile() && logintext[0] != '\0')
            {
                strncpy(s, logintext, sizeof(s) - 1);
                s[sizeof(s) - 1] = '\0';
                g_LoginTextLoaded = true;
            }
        }

        runtime_preview_menu::EnsureTexturesLoaded();
        Theme::ApplyThemeState();

        ImVec2 viewportCenter = ImGui::GetMainViewport()->GetCenter();
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        if (!isLogin && ui_loading::IsActive())
        {
            g_MainUiWasVisible = false;
            if (ui_loading::RenderWindow()) {
                isLogin = true;
            }
        }
        else if (!isLogin)
        {
            g_MainUiWasVisible = false;
            ImGui::SetNextWindowPos(viewportCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            const float loginW = ImMin(560.0f, ImMax(480.0f, displaySize.x * 0.43f));
            const float loginH = ImMin(660.0f, ImMax(610.0f, displaySize.y - 44.0f));
            ImGui::SetNextWindowSize(ImVec2(loginW, loginH), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.0f);

            if (ImGui::Begin(OBFUSCATE("Login Menu"), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
            {
                const ImVec2 pos = ImGui::GetWindowPos();
                ImDrawList* draw = ImGui::GetWindowDrawList();

                const bool loggingIn = ui_loading::IsActive();
                ImFont *textFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
                ImFont *iconFont = F107 ? F107 : ImGui::GetFont();
                const ImVec2 panelMin(pos.x + 24.0f, pos.y + 24.0f);
                const ImVec2 panelMax(pos.x + loginW - 24.0f, pos.y + loginH - 24.0f);
                const ImVec2 panelSize = panelMax - panelMin;
                draw->AddRectFilled(pos, pos + ImVec2(loginW, loginH), IM_COL32(7, 7, 14, 112), 18.0f);
                draw->AddRectFilledMultiColor(pos, pos + ImVec2(loginW, loginH), Theme::GetAccentTintU32(0.80f, 0.12f), IM_COL32(8, 8, 16, 30), IM_COL32(7, 7, 14, 80), Theme::GetAccentTintU32(0.55f, 0.09f));
                draw->AddRectFilled(panelMin, panelMax, IM_COL32(15, 16, 24, 222), 24.0f);
                draw->AddRect(panelMin, panelMax, Theme::GetAccentTintU32(1.0f, 0.74f), 24.0f, 0, 1.8f);

                ImFont *titleFont = font::techno_hideo ? font::techno_hideo : (font::auto_techno ? font::auto_techno : (F50 ? F50 : textFont));
                const ImVec2 logoCenter(panelMin.x + panelSize.x * 0.5f, panelMin.y + 76.0f);
                draw->AddText(titleFont, titleFont->FontSize * 3.1f, logoCenter - ImVec2(44.0f, 42.0f), Theme::GetAccentU32(), "M");

                const char *brandA = "Novara ";
                const char *brandB = "Supremacy";
                const float brandFs = titleFont->FontSize * 1.18f;
                const ImVec2 brandASz = titleFont->CalcTextSizeA(brandFs, FLT_MAX, 0.0f, brandA);
                const ImVec2 brandBSz = titleFont->CalcTextSizeA(brandFs, FLT_MAX, 0.0f, brandB);
                const ImVec2 brandPos(panelMin.x + (panelSize.x - brandASz.x - brandBSz.x) * 0.5f, panelMin.y + 132.0f);
                draw->AddText(titleFont, brandFs, brandPos, IM_COL32(248, 249, 252, 255), brandA);
                draw->AddText(titleFont, brandFs, brandPos + ImVec2(brandASz.x, 0.0f), Theme::GetAccentU32(), brandB);
                const char *tag = "Powerful. Intuitive. Seamless.";
                const ImVec2 tagSz = textFont->CalcTextSizeA(textFont->FontSize * 0.96f, FLT_MAX, 0.0f, tag);
                draw->AddText(textFont, textFont->FontSize * 0.96f, ImVec2(panelMin.x + (panelSize.x - tagSz.x) * 0.5f, panelMin.y + 180.0f), IM_COL32(166, 168, 190, 245), tag);

                const float decoY = panelMin.y + 220.0f;
                draw->AddLine(ImVec2(panelMin.x + panelSize.x * 0.36f, decoY), ImVec2(panelMin.x + panelSize.x * 0.49f, decoY), IM_COL32(62, 56, 82, 130), 1.0f);
                draw->AddCircleFilled(ImVec2(panelMin.x + panelSize.x * 0.5f, decoY), 3.0f, Theme::GetAccentU32(), 16);
                draw->AddLine(ImVec2(panelMin.x + panelSize.x * 0.51f, decoY), ImVec2(panelMin.x + panelSize.x * 0.64f, decoY), IM_COL32(62, 56, 82, 130), 1.0f);

                const char *loginTitle = "LOGIN";
                const float loginTitleFs = textFont->FontSize * 1.02f;
                const ImVec2 loginTitleSz = textFont->CalcTextSizeA(loginTitleFs, FLT_MAX, 0.0f, loginTitle);
                draw->AddText(textFont, loginTitleFs, ImVec2(panelMin.x + (panelSize.x - loginTitleSz.x) * 0.5f, panelMin.y + 252.0f), Theme::GetAccentU32(), loginTitle);
                const char *loginSub = "Sign in to continue";
                const ImVec2 loginSubSz = textFont->CalcTextSizeA(textFont->FontSize * 0.92f, FLT_MAX, 0.0f, loginSub);
                draw->AddText(textFont, textFont->FontSize * 0.92f, ImVec2(panelMin.x + (panelSize.x - loginSubSz.x) * 0.5f, panelMin.y + 290.0f), IM_COL32(168, 170, 192, 245), loginSub);

                const float inputWidth = panelSize.x - 96.0f;
                const float inputHeight = 56.0f;
                const ImVec2 authMin(panelMin.x + 48.0f, panelMin.y + 332.0f);
                ImGui::SetCursorScreenPos(authMin);
                ImGui::AstralInput("##login", s, sizeof(s), ImVec2(inputWidth, inputHeight));
                draw->AddRect(authMin, authMin + ImVec2(inputWidth, inputHeight), Theme::GetAccentTintU32(0.92f, 0.50f), 9.0f, 0, 1.35f);
                runtime_preview_menu::IconCenter(draw, iconFont, F107 ? 15.0f : 12.0f, authMin + ImVec2(22.0f, inputHeight * 0.5f), Theme::GetAccentU32(), ICON_FA_KEY);
                bool loginInputClicked = ImGui::IsItemClicked();
                bool loginInputActive = ImGui::IsItemActive();
                bool loginInputHovered = ImGui::IsItemHovered();

                if (loginInputClicked || loginInputActive)
                    showKeyboard = true;

                if (showKeyboard && !loginInputActive && !loginInputHovered && ImGui::IsMouseClicked(0))
                {
                    ImGuiIO& io = ImGui::GetIO();
                    float screenHeight = io.DisplaySize.y;
                    float keyboardHeight = screenHeight * 0.60f;
                    if (ImGui::GetMousePos().y < screenHeight - keyboardHeight)
                        showKeyboard = false;
                }

                const float buttonY = authMin.y + 78.0f;
                const float gap = 26.0f;
                const float pasteW = (inputWidth - gap) * 0.48f;
                const float btnW = inputWidth - pasteW - gap;
                if (runtime_preview_menu::LoginPasteButton(draw, iconFont, textFont, ImVec2(authMin.x, buttonY), ImVec2(pasteW, 56.0f), loggingIn)) {
                    const std::string clip = getClipboard();
                    if (!clip.empty()) {
                        std::snprintf(s, sizeof(s), "%s", clip.c_str());
                        showKeyboard = false;
                    }
                }
                if (runtime_preview_menu::LoginActionButton(draw, iconFont, textFont, "##Enter", loggingIn ? "VERIFYING" : "LOGIN", ImVec2(authMin.x + pasteW + gap, buttonY), ImVec2(btnW, 56.0f), loggingIn))
                {
                    err = Login(s);
                    if (err == "OK")
                    {
                        showKeyboard = false;
                        strncpy(logintext, s, sizeof(logintext) - 1);
                        logintext[sizeof(logintext) - 1] = '\0';
                        SaveLoginTextToFile(s);
                        g_LoginTextLoaded = true;
                        err.clear();
                        ui_loading::Start(3.0f);
                    }
                }

                if (!err.empty() && err != "OK")
                {
                    ImGui::SetCursorScreenPos(ImVec2(authMin.x, buttonY + 62.0f));
                    ImGui::TextColored(ImColor(255, 90, 90, 255), "Error: %s", err.c_str());
                }
                if (showKeyboard)
                    Keyboard("##Keyboard", s, sizeof(s), &showKeyboard);

            }
            ImGui::End();
        }
        else
        {
            uncollapseOpenAnim = ImClamp(uncollapseOpenAnim + ImGui::GetIO().DeltaTime * 5.0f, 0.0f, 1.0f);
            float openEase = uncollapseOpenAnim * uncollapseOpenAnim * (3.0f - 2.0f * uncollapseOpenAnim);
            float openAlpha = 0.2f + 0.8f * openEase;
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, openAlpha);

            const float previewMenuW = 905.0f;
            const float previewMenuH = 530.0f;
            const float previewMenuAspect = previewMenuW / previewMenuH;
            const float maxMenuW = ImMax(360.0f, displaySize.x - 72.0f);
            const float maxMenuH = ImMax(340.0f, displaySize.y - 14.0f);
            ImVec2 mainWindowSize;
            mainWindowSize.y = ImMin(previewMenuH, maxMenuH);
            mainWindowSize.x = mainWindowSize.y * previewMenuAspect;
            if (mainWindowSize.x > maxMenuW) {
                mainWindowSize.x = maxMenuW;
                mainWindowSize.y = mainWindowSize.x / previewMenuAspect;
            }
            mainWindowSize.y = ImMin(maxMenuH, mainWindowSize.y);
            ImGui::SetNextWindowPos(viewportCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(mainWindowSize, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGui::Begin("@rspctd", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
            {
                runtime_preview_menu::StateRefs State{dark, tabAlpha, tabAdd, page, activeTab, windowCollapsed, isMenuVisible, collapseBarLastActiveTime, collapseBarOpacityAnim, collapseBarPressAnim};
                {
                    using namespace runtime_preview_menu;
                    using namespace ImGui;
                    int &Page = State.page;
                    int &Tab = State.activeTab;
                    Theme::ApplyAccentFromHue();

                    ImGuiStyle *Style = &ImGui::GetStyle();
                    c::ApplyMainWindowStyle(*Style);
                    c::UpdateTheme(State.dark, menu, ImGui::GetIO().DeltaTime);
                    Theme::ApplyThemeState();

                    const ImVec2 WinSize = ImGui::GetWindowSize();
                    const ImVec2 WinPos = ImGui::GetWindowPos();
                    const float uiScale = ImClamp(WinSize.y / 530.0f, 0.68f, 1.0f);
                    ImDrawList *DrawList = ImGui::GetWindowDrawList();

                    WindowShell(DrawList, WinPos, WinSize, uiScale);

                    const float outerPad = 0.0f * uiScale;
                    const float sidebarWidth = 65.0f * uiScale;
                    const float contentPad = 0.0f;
                    const float headerHeight = 62.0f * uiScale;
                    const float headerActionGap = 10.0f * uiScale;
                    const ImVec2 headerActionSize(161.0f * uiScale, 40.0f * uiScale);
                    const float columnGap = 20.0f * uiScale;
                    const float contentInsetX = 20.0f * uiScale;
                    const float contentInsetY = 10.0f * uiScale;

                    const Geometry Geom = BuildGeometry(WinPos, WinSize, uiScale, outerPad, sidebarWidth, contentPad, headerHeight, headerActionSize, headerActionGap);
                    const ImVec2 &sidebarMin = Geom.sidebarMin;
                    const ImVec2 &sidebarMax = Geom.sidebarMax;
                    const ImVec2 &contentMin = Geom.contentMin;
                    const ImVec2 &contentMax = Geom.contentMax;
                    const ImVec2 &titleCardMin = Geom.titleCardMin;
                    const ImVec2 &titleCardSize = Geom.titleCardSize;
                    const ImVec2 &hostMin = Geom.hostMin;
                    const ImVec2 &hostMax = Geom.hostMax;
                    const ImVec2 InnerMin = Geom.contentInnerMin + ImVec2(contentInsetX, contentInsetY);
                    const ImVec2 InnerSize(
                        ImMax(0.0f, Geom.contentInnerSize.x - contentInsetX * 2.0f),
                        ImMax(0.0f, Geom.contentInnerSize.y - contentInsetY * 2.0f)
                    );

                    Page = ImClamp(Page, 1, 6);
                    Tab = ImClamp(Tab, 1, 6);

                    ImFont *headerFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
                    const ImVec2 rightPanelMin(WinPos.x + sidebarWidth, WinPos.y);
                    const ImVec2 rightPanelMax(WinPos.x + WinSize.x, WinPos.y + WinSize.y);
                    DrawList->AddRectFilled(rightPanelMin, rightPanelMax, IM_COL32(17, 17, 22, 255), 0.0f);
                    DrawList->AddRectFilled(sidebarMin, sidebarMax, IM_COL32(17, 17, 22, 118), 16.0f * uiScale, ImDrawFlags_RoundCornersLeft);
                    ImFont *iconFont = F107 ? F107 : headerFont;
                    ImFont *brandFont = font::inter_semibold ? font::inter_semibold : headerFont;
                    ImFont *logoFont = font::techno_hideo ? font::techno_hideo : (font::auto_techno ? font::auto_techno : brandFont);
                    const ImVec2 logoCenter(sidebarMin.x + sidebarWidth * 0.5f, sidebarMin.y + 33.0f * uiScale);
                    const char *logoText = "M";
                    const float logoTime = (float)ImGui::GetTime();
                    const float pulse = 0.5f + 0.5f * sinf(logoTime * 3.1f);
                    const float orbit = logoTime * 1.55f;
                    const float baseR = 18.0f * uiScale;
                    const float glowR = (21.0f + 2.4f * pulse) * uiScale;
                    const float logoSize = logoFont->FontSize * 1.18f * uiScale;
                    const ImVec2 logoTextSize = logoFont->CalcTextSizeA(logoSize, FLT_MAX, 0.0f, logoText);
                    DrawList->AddCircleFilled(logoCenter, glowR, ImGui::GetColorU32(ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.08f + 0.08f * pulse)), 40);
                    DrawList->AddCircleFilled(logoCenter, baseR, IM_COL32(35, 35, 46, 216), 40);
                    DrawList->AddCircle(logoCenter, baseR + 1.2f * uiScale * pulse, ImGui::GetColorU32(ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.42f + 0.20f * pulse)), 40, 1.1f * uiScale);
                    for (int s = 0; s < 2; ++s) {
                        const float a0 = orbit + s * IM_PI;
                        const ImVec2 p0(logoCenter.x + cosf(a0) * (baseR + 2.8f * uiScale), logoCenter.y + sinf(a0) * (baseR + 2.8f * uiScale));
                        DrawList->AddCircleFilled(p0, (2.0f + 0.6f * pulse) * uiScale, ImGui::GetColorU32(c::raspody::accent), 12);
                    }
                    DrawList->PathClear();
                    for (int i = 0; i <= 16; ++i) {
                        const float a = orbit + (float)i / 16.0f * 1.45f;
                        DrawList->PathLineTo(ImVec2(logoCenter.x + cosf(a) * (baseR + 3.3f * uiScale), logoCenter.y + sinf(a) * (baseR + 3.3f * uiScale)));
                    }
                    DrawList->PathStroke(ImGui::GetColorU32(ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.38f)), false, 1.2f * uiScale);
                    DrawList->AddText(logoFont, logoSize, logoCenter - logoTextSize * 0.5f + ImVec2(0.0f, 1.0f * uiScale), ImGui::GetColorU32(c::raspody::accent), logoText);
                    DrawList->AddLine(ImVec2(sidebarMin.x + 22.0f * uiScale, sidebarMin.y + 68.0f * uiScale),
                                      ImVec2(sidebarMin.x + sidebarWidth - 22.0f * uiScale, sidebarMin.y + 68.0f * uiScale),
                                      IM_COL32(54, 55, 66, 75), 1.0f * uiScale);

                    const int sidebarButtonCount = SidebarItemCount();
                    const float sidebarButtonGap = 14.0f * uiScale;
                    const float sidebarButtonHeight = 35.0f * uiScale;
                    const float sidebarStartY = sidebarMin.y + 86.0f * uiScale;
                    const ImVec2 sidebarButtonSize(sidebarWidth, sidebarButtonHeight);
                    static float sidebarSelY = 0.0f;
                    static float sidebarSelVel = 0.0f;
                    const float selectorTile = 33.0f * uiScale;
                    const float selectorTargetY = (sidebarStartY - sidebarMin.y) + (Page - 1) * (sidebarButtonHeight + sidebarButtonGap) + (sidebarButtonHeight - selectorTile) * 0.5f;
                    if (sidebarSelY <= 0.0f) {
                        sidebarSelY = selectorTargetY;
                    }
                    const float selectorDt = ImMin(ImGui::GetIO().DeltaTime, 0.033f);
                    const float selectorForce = 72.0f * (selectorTargetY - sidebarSelY) - 13.5f * sidebarSelVel;
                    sidebarSelVel += selectorForce * selectorDt;
                    sidebarSelY += sidebarSelVel * selectorDt;
                    const ImVec2 selectorMin(sidebarMin.x + (sidebarWidth - selectorTile) * 0.5f, sidebarMin.y + sidebarSelY);
                    DrawList->AddRectFilled(selectorMin, selectorMin + ImVec2(selectorTile, selectorTile), IM_COL32(44, 44, 52, 150), 4.0f * uiScale);
                    DrawList->AddRect(selectorMin, selectorMin + ImVec2(selectorTile, selectorTile), ImGui::GetColorU32(ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.32f)), 4.0f * uiScale, 0, 1.0f * uiScale);
                    for (int i = 0; i < sidebarButtonCount; ++i) {
                        const ImVec2 buttonMin(sidebarMin.x, sidebarStartY + i * (sidebarButtonHeight + sidebarButtonGap));
                        const std::string buttonId = "sidebar" + std::to_string(i);
                        if (SidebarButton(DrawList, buttonId.c_str(), kSidebarItems[i].icon, kSidebarItems[i].label, buttonMin, sidebarButtonSize, Page == (i + 1), uiScale)) {
                            Page = i + 1;
                        }
                    }

                    const ImVec2 brandPos(titleCardMin.x + 22.0f * uiScale, titleCardMin.y + 17.0f * uiScale);
                    ImFont *titleFont = font::techno_hideo ? font::techno_hideo : (font::auto_techno ? font::auto_techno : brandFont);
                    const float brandSize = titleFont->FontSize * 0.96f * uiScale;
                    const char *brandMain = "Novara";
                    const char *brandVer = "Supremacy";
                    float brandX = brandPos.x;
                    for (const char *p = brandMain; *p; ++p) {
                        char ch[2] = {*p, 0};
                        DrawList->AddText(titleFont, brandSize, ImVec2(brandX, brandPos.y), IM_COL32(246, 247, 250, 255), ch);
                        brandX += titleFont->CalcTextSizeA(brandSize, FLT_MAX, 0.0f, ch).x + 2.6f * uiScale;
                    }
                    brandX += 12.0f * uiScale;
                    DrawList->AddText(titleFont, brandSize, ImVec2(brandX, brandPos.y), ImGui::GetColorU32(c::raspody::accent), brandVer);

                    const ImVec2 collapseSize(40.0f * uiScale, 40.0f * uiScale);
                    const ImVec2 collapseMin(titleCardMin.x + titleCardSize.x - collapseSize.x - 20.0f * uiScale, titleCardMin.y + 11.0f * uiScale);
                    if (CloseButton(DrawList, collapseMin, collapseSize, uiScale)) {
                        runtime_preview_menu::CollapseMenu(State);
                    }

                    const float tabSpeed = 7.5f * ImGui::GetIO().DeltaTime;
                    State.tabAlpha = ImClamp(State.tabAlpha + (Page == Tab ? tabSpeed : -tabSpeed), 0.0f, 1.0f);
                    if (Page != Tab && State.tabAlpha <= 0.02f) {
                        Tab = Page;
                        State.tabAlpha = 0.0f;
                    }
                    static int contentAnimTab = Tab;
                    static float contentAnimTime = 1.0f;
                    const bool mainUiJustAppeared = !g_MainUiWasVisible;
                    g_MainUiWasVisible = true;
                    if (contentAnimTab != Tab || mainUiJustAppeared) {
                        contentAnimTab = Tab;
                        contentAnimTime = 0.0f;
                    }
                    contentAnimTime += ImGui::GetIO().DeltaTime;
                    const float contentT = contentAnimTime * 1.35f;
                    const float contentBounce = 16.0f * uiScale * expf(-5.8f * contentT) * cosf(9.5f * contentT);
                    const float contentFade = ImClamp(contentAnimTime * 3.2f, 0.0f, 1.0f);
                    State.tabAdd = ImLerp(State.tabAdd, (1.0f - State.tabAlpha) * 10.0f * uiScale + contentBounce, ImMin(1.0f, 12.0f * ImGui::GetIO().DeltaTime));

                    ImGui::SetCursorScreenPos(InnerMin + ImVec2(0.0f, State.tabAdd));
                    const bool needsScroll = false;
                    const ImGuiWindowFlags contentFlags = ImGuiWindowFlags_NoBackground | (!needsScroll ? (ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse) : 0);
                    char contentId[32];
                    std::snprintf(contentId, sizeof(contentId), "Content##tab_%d", Tab);
                    ImGui::BeginChild(contentId, InnerSize, false, contentFlags);
                    {
                        const bool pushedContentFont = (font::inter_semibold != nullptr);
                        if (pushedContentFont) {
                            ImGui::PushFont(font::inter_semibold);
                        }

                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, State.tabAlpha * contentFade * Style->Alpha);

                        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
                        const float childHeight = ImMax(0.0f, contentRegion.y);
                        const float childWidth = ImMax(0.0f, (contentRegion.x - columnGap) * 0.5f);
                        const RevealAnim reveal{contentAnimTime, uiScale};

                        if (Tab == 1)
                        {
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            const float gap = columnGap;
                            const ImU32 white = IM_COL32(246, 247, 250, 255);
                            const ImU32 muted = IM_COL32(151, 151, 179, 230);
                            const ImU32 accent = ImGui::GetColorU32(c::raspody::accent);
                            const ImU32 cardBg = IM_COL32(24, 24, 31, 214);
                            const ImU32 cardHover = IM_COL32(31, 31, 40, 235);
                            const ImU32 stroke = IM_COL32(48, 48, 61, 160);
                            const float leftW = ImMax(0.0f, contentRegion.x * 0.58f - gap * 0.5f);
                            const float rightW = ImMax(0.0f, contentRegion.x - leftW - gap);
                            const float headerH = 74.0f * uiScale;
                            const float bottomH = 32.0f * uiScale;
                            const float panelH = ImMax(120.0f * uiScale, (childHeight - headerH - bottomH - gap * 2.0f) * 0.5f);
                            const float leftPanelW = leftW;
                            const float rightPanelH = ImMax(150.0f * uiScale, (childHeight - gap) * 0.5f);

                            ImGui::SetCursorScreenPos(reveal.Left(pageStart));
                            DrawHomeTitle(DrawList, iconFont, headerFont, font::inter_semibold, pageStart, uiScale, accent, white, muted, "Welcome, Guest", "Manage your configurations and system information", ICON_FA_HOME);

                            const ImVec2 cfgPos = reveal.Left(ImVec2(pageStart.x, pageStart.y + headerH), 0.0f);
                            BeginHomePanel("home_cfg_panel", DrawList, headerFont, cfgPos, ImVec2(leftPanelW, panelH), "CONFIGURATION MANAGER", uiScale, stroke);
                            {
                                const ImVec2 base = ImGui::GetCursorScreenPos();
                                const float btnGap = 14.0f * uiScale;
                                const float btnW = (ImGui::GetContentRegionAvail().x - btnGap) * 0.5f;
                                const ImVec2 btnSize(btnW, ImMax(62.0f * uiScale, ImGui::GetContentRegionAvail().y));
                                if (HomeCardButton(DrawList, iconFont, font::inter_semibold, "home_save", base, btnSize, ICON_FA_SAVE, "Save Config", "Save current settings", uiScale, accent, white, muted, cardBg, cardHover, stroke)) {
                                    SaveConfig();
                                    SaveConfiguration("meija_config");
                                }
                                if (HomeCardButton(DrawList, iconFont, font::inter_semibold, "home_load", ImVec2(base.x + btnW + btnGap, base.y), btnSize, ICON_FA_FOLDER_OPEN, "Load Config", "Load saved settings", uiScale, accent, white, muted, cardBg, cardHover, stroke)) {
                                    LoadConfig();
                                    LoadConfiguration("meija_config");
                                }
                            }
                            ImGui::EndChild();

                            const ImVec2 displayPos = reveal.Left(ImVec2(pageStart.x, pageStart.y + headerH + panelH + gap), 0.16f);
                            BeginHomePanel("home_display_panel", DrawList, headerFont, displayPos, ImVec2(leftPanelW, panelH), "DISPLAY CONTROLS", uiScale, stroke);
                            {
                                const ImVec2 base = ImGui::GetCursorScreenPos();
                                const float btnGap = 14.0f * uiScale;
                                const float btnW = (ImGui::GetContentRegionAvail().x - btnGap) * 0.5f;
                                const ImVec2 btnSize(btnW, ImMax(62.0f * uiScale, ImGui::GetContentRegionAvail().y));
                                if (HomeCardButton(DrawList, iconFont, font::inter_semibold, "home_reset", base, btnSize, ICON_FA_SYNC, "Reset Config", "Reset to defaults", uiScale, accent, white, muted, cardBg, cardHover, stroke)) {
                                    ResetBannedGuestAccount();
                                }
                                if (HomeCardButton(DrawList, iconFont, font::inter_semibold, "home_clear", ImVec2(base.x + btnW + btnGap, base.y), btnSize, ICON_FA_EYE_SLASH, "Clear Display", "Hide text overlay", uiScale, accent, white, muted, cardBg, cardHover, stroke)) {
                                    Config.ExtraMenu.ClearDisplay = !Config.ExtraMenu.ClearDisplay;
                                }
                            }
                            ImGui::EndChild();

                            const ImVec2 rightPos = reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y), 0.0f);
                            const ImVec2 devicePos = rightPos;
                            BeginHomePanel("home_device_panel", DrawList, headerFont, devicePos, ImVec2(rightW, rightPanelH), "DEVICE INFORMATION", uiScale, stroke);
                            {
                                const std::string devName = DeviceName();
                                const std::string os = OsVersion();
                                const std::string mem = MemoryUsage();
                                char res[64];
                                std::snprintf(res, sizeof(res), "%.0f x %.0f", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_MOBILE, "Device", devName.c_str(), uiScale, accent, muted, white);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_INFO_CIRCLE, "Android", os.c_str(), uiScale, accent, muted, white);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_MICROCHIP, "Memory", mem.c_str(), uiScale, accent, muted, white);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_DESKTOP, "Screen", res, uiScale, accent, muted, white);
                            }
                            ImGui::EndChild();

                            const ImVec2 licPos = reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y + rightPanelH + gap), 0.16f);
                            BeginHomePanel("home_license_panel", DrawList, headerFont, licPos, ImVec2(rightW, rightPanelH), "LICENSE INFORMATION", uiScale, stroke);
                            {
                                const std::string maskedKey = MaskKey(usedKey.c_str());
                                const std::string exp = ExpiryLabel();
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_KEY, "License Type", maskedKey.empty() ? "Guest" : maskedKey.c_str(), uiScale, accent, muted, accent);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_CHECK_CIRCLE, "Status", maskedKey.empty() ? "Premium User" : "Active", uiScale, accent, muted, accent);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_CLOCK, "Expiry", exp.empty() ? "N/A" : exp.c_str(), uiScale, accent, muted, white);
                                HomeInfoRow(DrawList, iconFont, font::inter_semibold, ICON_FA_SYNC, "Updates", "N/A", uiScale, accent, muted, white);
                            }
                            ImGui::EndChild();

                            const ImVec2 stripPos = reveal.Bottom(ImVec2(pageStart.x, pageStart.y + childHeight - bottomH), 0.22f);
                            const ImVec2 stripSize(leftW, bottomH);
                            DrawList->AddRectFilled(stripPos, stripPos + stripSize, IM_COL32(14, 14, 19, 218), 8.0f * uiScale);
                            DrawList->AddRect(stripPos, stripPos + stripSize, stroke, 8.0f * uiScale, 0, 1.0f * uiScale);
                            DrawList->AddCircleFilled(stripPos + ImVec2(18.0f, bottomH * 0.5f), 4.0f * uiScale, IM_COL32(83, 84, 103, 255), 12);
                            DrawList->AddText(headerFont, headerFont->FontSize * 0.72f * uiScale, stripPos + ImVec2(34.0f, 9.0f) * uiScale, muted, "STATUS:  Active");
                            DrawList->AddText(iconFont, (F107 ? 12.0f : 10.0f) * uiScale, stripPos + ImVec2(stripSize.x * 0.44f, 10.0f * uiScale), muted, ICON_FA_USER);
                            DrawList->AddText(headerFont, headerFont->FontSize * 0.72f * uiScale, stripPos + ImVec2(stripSize.x * 0.47f, 9.0f * uiScale), muted, "MODE:  GUEST");
                            DrawList->AddText(iconFont, (F107 ? 12.0f : 10.0f) * uiScale, stripPos + ImVec2(stripSize.x * 0.78f, 10.0f * uiScale), muted, ICON_FA_CUBE);
                            DrawList->AddText(headerFont, headerFont->FontSize * 0.72f * uiScale, stripPos + ImVec2(stripSize.x * 0.83f, 9.0f * uiScale), muted, "BUILD:   v1.0.0");
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + childHeight));
                        }

                        if (Tab == 3)
                        {
                            const ImGuiWindowFlags aimFlags = 0;
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            const float gap = columnGap;
                            const float leftW = childWidth;
                            const float rightW = childWidth;
                            const float capH = 42.0f * c::scale;
                            const float leftH = childHeight;
                            const float rightH = childHeight;

                            ImGui::SetCursorScreenPos(reveal.Left(pageStart));
                            if (ImGui::BeginChild("AIMBOT", ImVec2(leftW, leftH), true, aimFlags)) {
                                ImGui::Checkbox("Aimbot 360", &Config.Aim.Aimbot360);
                                ImGui::Checkbox("Bullet Track", &Config.Aim.AimSilent);
                                ImGui::SliderFloat("Aim Assist Size", &Config.Aim.AimAssistSize, 0.0f, 100.0f, "%.0f");
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorScreenPos(reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y)));
                            if (ImGui::BeginChild("COMBAT SETUP", ImVec2(rightW, rightH), true, aimFlags)) {
                                static const char *priorities[] = {"Closest", "Lowest Health", "Nearest"};
                                int by = (int)Config.Aim.By;                               
                                if (ImGui::Combo("Target Priority", "", &by, priorities, IM_ARRAYSIZE(priorities))) Config.Aim.By = (EAim)by;

                                static const char *targets[] = {"Head", "Chest", "Body"};
                                int target = (int)Config.Aim.Target;                               
                                if (ImGui::Combo("Location", "", &target, targets, IM_ARRAYSIZE(targets))) Config.Aim.Target = (EAimTarget)target;

                                static const char *points[] = {"Neck", "Chest", "Pelvis"}; 
                                int trigger = (int)Config.Aim.Trigger;                                                  
                                if (ImGui::Combo("Multi-Point", "", &trigger, points, IM_ARRAYSIZE(points))) Config.Aim.Trigger = (EAimTrigger)trigger;
                                ImGui::SliderFloat("FOV Size", &Config.Aim.Cross, 0.0f, 180.0f, "%.0f");
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + ImMax(leftH, rightH)));
                        }

                        if (Tab == 2)
                        {
                            const ImGuiWindowFlags visualFlags = 0;
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            const float gap = columnGap;
                            const float leftW = childWidth;
                            const float rightW = childWidth;
                            const float espH = childHeight;
                            const float previewH = ImMax(220.0f * uiScale, childHeight * 0.56f);
                            const float optionH = ImMax(0.0f, childHeight - previewH - gap);

                            ImGui::SetCursorScreenPos(reveal.Left(pageStart));
                            if (ImGui::BeginChild("ESP MENU", ImVec2(leftW, espH), true, visualFlags)) {
                                struct EspItem { const char *label; bool *value; };
                                EspItem items[] = {
                                    {"Line", &Config.ESPMenu.isPlayerLine},
                                    {"Box", &Config.ESPMenu.Box},
                                    {"Skeleton", &Config.ESPMenu.Skeleton},
                                    {"Health", &Config.ESPMenu.Health},
                                    {"Name", &Config.ESPMenu.Name},
                                    {"Distance", &Config.ESPMenu.Distance},
                                    {"Enemy Count", &Config.ESPMenu.Count},
                                    {"360 Alert", &Config.ESPMenu.Alert},
                                    {"Yellow Wallhack", &Config.ExtraMenu.WallHack},
                                    {"Red Wallhack", &Config.ExtraMenu.RedWallhack},
                                };
                                for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
                                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::Checkbox(items[i].label, items[i].value);
                                    ImGui::PopItemWidth();
                                }
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorScreenPos(reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y)));
                            if (ImGui::BeginChild("ESP PREVIEW", ImVec2(rightW, previewH), true, visualFlags)) {
                                VisualPreview(Config.ESPMenu.Box,
                                              Config.ESPMenu.Skeleton,
                                              Config.ESPMenu.Health,
                                              Config.ESPMenu.Name,
                                              Config.ESPMenu.Distance,
                                              (int)Config.ESPMenu.BoxType,
                                              (int)Config.ESPMenu.Target,
                                              (int)Config.ESPMenu.HealthPosition);
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorScreenPos(reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y + previewH + gap), 0.16f));
                            if (ImGui::BeginChild("VISUAL OPTIONS", ImVec2(rightW, optionH), true, visualFlags)) {
                                int boxType = (int)Config.ESPMenu.BoxType;
                                static const char *boxTypes[] = {"Fill", "Outline", "Corner", "3D"};
                                if (ImGui::Combo("Box Type", "", &boxType, boxTypes, IM_ARRAYSIZE(boxTypes))) Config.ESPMenu.BoxType = (EspBoxType)boxType;

                                int lineTarget = (int)Config.ESPMenu.Target;
                                static const char *linePositions[] = {"Top", "Mid", "Bottom"};
                                if (ImGui::Combo("Line Anchor", "", &lineTarget, linePositions, IM_ARRAYSIZE(linePositions))) Config.ESPMenu.Target = (LineTarget)lineTarget;

                                int healthPos = (int)Config.ESPMenu.HealthPosition;
                                static const char *healthPositions[] = {"Top", "Side"};
                                if (ImGui::Combo("Health Layout", "", &healthPos, healthPositions, IM_ARRAYSIZE(healthPositions))) Config.ESPMenu.HealthPosition = (EspHealthPosition)healthPos;
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + childHeight));
                        }

                        if (Tab == 4)
                        {
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            const float gap = columnGap;
                            const float leftW = childWidth;
                            const float rightW = childWidth;
                            const float toolsH = childHeight;
                            const float moveH = ImMax(148.0f * uiScale, childHeight * 0.39f);
                            const float survivalH = ImMax(0.0f, childHeight - moveH - gap);

                            ImGui::SetCursorScreenPos(reveal.Left(pageStart));
                            const ImGuiWindowFlags memToolsFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
                            if (ImGui::BeginChild("MEMORY TOOLS", ImVec2(leftW, toolsH), true, memToolsFlags)) {
                                struct MemItem { const char *label; bool *value; };
                                MemItem items[] = {
                                    {"Stream Hide", &Config.ExtraMenu.StreamHide},
                                    {"Hitbox", &Config.ExtraMenu.Hit},
                                    {"No Recoil", &Config.ExtraMenu.Recoil},
                                    {"No Spread", &Config.ExtraMenu.Spread},
                                    {"No Shake", &Config.ExtraMenu.Shake},
                                    {"Firerate", &Config.ExtraMenu.Fire},
                                    {"Fast Reload", &Config.ExtraMenu.Reload},
                                    {"Fast Scope", &Config.ExtraMenu.Scope},
                                    {"Quick Switch", &Config.ExtraMenu.Switch},
                                    {"Weapon Kinetic", &Config.ExtraMenu.Kinetic},
                                };
                                for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
                                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                                    ImGui::Checkbox(items[i].label, items[i].value);
                                    ImGui::PopItemWidth();
                                }
                            }
                            ImGui::EndChild();

                            ImGui::SetCursorScreenPos(reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y)));
                            const ImGuiWindowFlags memChildFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
                            if (ImGui::BeginChild("MOVEMENT TUNING", ImVec2(rightW, moveH), true, memChildFlags)) {
                                ImGui::SliderFloat("Snow Board", &SnowBsize, 0.0f, 200.0f, "%.1f");
                                ImGui::SliderFloat("SpeedHack", &speedHackMultiplier, 0.5f, 2.0f, "%.1fx");
                                ImGui::SliderFloat("High Jump", &jumpHeightMultiplier, 0.5f, 5.0f, "%.2fx");
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(reveal.Right(ImVec2(pageStart.x + leftW + gap, pageStart.y + moveH + gap), 0.16f));
                            if (ImGui::BeginChild("SURVIVAL TOOLS", ImVec2(rightW, survivalH), true, memChildFlags)) {
                                ImGui::Checkbox("No Parachute", &Config.ExtraMenu.Parachute);
                                ImGui::Checkbox("Fast Armor Plate", &Config.ExtraMenu.FastArmorPlate);
                                ImGui::Checkbox("Fast Dive", &Config.ExtraMenu.Diving);
                                ImGui::Checkbox("Anti Flashbang", &Config.ExtraMenu.Flash);
                                ImGui::Checkbox("No Overheat", &Config.ExtraMenu.Rpd);
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + childHeight));
                        }

                        if (Tab == 5)
                        {
                            const float fullW = contentRegion.x;
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            ImGui::SetCursorScreenPos(reveal.Bottom(pageStart));
                            if (ImGui::BeginChild("SKIN LOADOUT##weapon", ImVec2(fullW, childHeight), true)) {
                                SkinContent(0);
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + childHeight));
                        }

                        if (Tab == 6)
                        {
                            const float fullW = contentRegion.x;
                            const ImVec2 pageStart = ImGui::GetCursorScreenPos();
                            ImGui::SetCursorScreenPos(reveal.Bottom(pageStart));
                            if (ImGui::BeginChild("CHARACTER LOADOUT##character", ImVec2(fullW, childHeight), true)) {
                                SkinContent(1);
                            }
                            ImGui::EndChild();
                            ImGui::SetCursorScreenPos(ImVec2(pageStart.x, pageStart.y + childHeight));
                        }

                        ImGui::PopStyleVar();
                        
                        if (pushedContentFont) {
                            ImGui::PopFont();
                        }

                    }
                    ImGui::EndChild();
                    
                    runtime_preview_menu::ResetPopupFocusWindow();
                    runtime_preview_menu::DrawPopupBackdropFocusLayer(ImGui::GetForegroundDrawList());

                }

                if (Config.ExtraMenu.WallHack) {
                    Patches.A1.Modify();
                } else {
                    Patches.A1.Restore();
                } 

            }
            ImGui::PopStyleVar(2);
            ImGui::End();
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar();
    }

    if (fromOverlay)
    {
        io->MouseDown[0] = g_OverlayDown;
        io->MousePos = MapMenuInput(g_OverlayX, g_OverlayY, screenWidth, screenHeight, menuScale);
    }
    else
    {

auto Input_get_touchCount = (int (*)())(Class_Input_get_touchCount);
if (Input_get_touchCount() > 0)
{
    auto Input_GetTouch = (Touch(*)(uintptr_t, int))(Class_Input_GetTouch);
    auto Input_get_mousePosition = (Vector3(*)(uintptr_t))(Class_Input_get_mousePosition);

    Touch t = Input_GetTouch(Config.ImGuiMenu.thiz, 0);
    Vector3 mp = Input_get_mousePosition(Config.ImGuiMenu.thiz);

    // 1.6.57 safe: cache once, use everywhere
    float mx = mp.x;
    float my = (float)get_height() - mp.y;

    switch (t.m_Phase)
    {
    case TouchPhase::Began:
    case TouchPhase::Stationary:
        io->MouseDown[0] = true;
        io->MousePos     = ImVec2(mx, my);
        break;

    case TouchPhase::Moved:
        io->MouseDown[0] = true;
        io->MousePos     = ImVec2(mx, my);
        break;

    case TouchPhase::Ended:
    case TouchPhase::Canceled:
        io->MouseDown[0]             = false;
        Config.ImGuiMenu.clearMousePos = true;
        break;

    default:
        break;
    }
}
else
{
    // No fingers down — guarantee clean state
    if (Config.ImGuiMenu.clearMousePos)
    {
        io->MousePos               = ImVec2(-FLT_MAX, -FLT_MAX);
        Config.ImGuiMenu.clearMousePos = false;
    }
    io->MouseDown[0] = false;
}

ImGui::EndFrame();
    ImGui::Render();
    
    if (Config.ExtraMenu.StreamHide && g_CallOldSwapFn != nullptr) {
        EGLDisplay eglDpy = (EGLDisplay)dpy;
        EGLSurface eglSurface = (EGLSurface)surface;
        EGLContext gameCtx = eglGetCurrentContext();
        ZEL_Render(ImGui::GetDrawData(), eglDpy, eglSurface, gameCtx,
                   (android::HideRecAImGui::SwapBuffersFn)g_CallOldSwapFn);
    } else {
        ZEL_Shutdown();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    if (g_CallOldSwapFn != nullptr) return g_CallOldSwapFn(dpy, surface);
    return 0;
}
}


void Anogs_Thread()
{
    InitializeProtection();
}

void* ASkinThread(void*)
{
    BP_TH(nullptr);
    return nullptr;
}

void StartCoreThreads();

void Unity_Thread()
{
    while (!m_unity)
    {
        m_unity = Tools::GetBaseAddress("libunity.so");
        sleep(1);
    }
    LOGI("libunity.so: %p", m_unity);

    UpdateAllOffset();

    MemoryPatch::createWithHex("libunity.so", 0x5755800, "00 00 80 D2 C0 03 5F D6").Modify();
    MemoryPatch::createWithHex("libunity.so", 0x9FEC8AC, "00 00 80 D2 C0 03 5F D6").Modify();

    Patches.A1 = MemoryPatch::createWithHex("libunity.so", 0x548A67C, "1F 20 03 D5 E0 03 13 AA");

    DobbyHook((void *)getAbsoluteAddress("libunity.so", 0xC9B6F90), (void *)&WeaponFireComponent_Instant_CreateBulletLine, (void **)&oWeaponFireComponent_Instant_CreateBulletLine);
    DobbyHook((void *)getAbsoluteAddress("libunity.so", 0xC9C33A4), (void *)&WeaponFireComponent_Instant_CreateBulletProjectile, (void **)&oWeaponFireComponent_Instant_CreateBulletProjectile);
    InitializeAllHooks();
    TryInitEglBridge();

  
}

__attribute__((constructor))
void lib_main()
{
    StartCoreThreads();

    pthread_t ASkin;
    pthread_create(&ASkin, 0, ASkinThread, 0);
}

void StartCoreThreads()
{
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static bool started = false;

    pthread_mutex_lock(&lock);
    if (started) {
        pthread_mutex_unlock(&lock);
        return;
    }
    started = true;
    pthread_mutex_unlock(&lock);

    std::thread(Unity_Thread).detach();
    std::thread(Anogs_Thread).detach();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jvm = vm;
    VM = vm;
    ZEL_SetVM(vm);

    StartCoreThreads();

    return JNI_VERSION_1_6;
}
