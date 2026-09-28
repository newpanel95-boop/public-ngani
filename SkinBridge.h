#pragma once

#include <dlfcn.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>

#include <atomic>
#include <string>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "../ImGui/imgui.h"
#include "AstralSkinApi.h"

using SkinInstallFn = int (*)();
using SkinSelectFn = int (*)(const AstralSkinSelection*);
using SkinSetUrlFn = void (*)(const char*);
using SkinSetKeyFn = void (*)(const char*);

inline void* g_SkinBridge = nullptr;
inline SkinInstallFn g_SkinInstall = nullptr;
inline SkinSelectFn g_SkinSelect = nullptr;
inline std::atomic<bool> g_SkinReady{false};
inline std::atomic<bool> g_SkinTried{false};

inline void* OpenSkinBridge()
{
    void* handle = dlopen("libAstxvoid.so", RTLD_NOW | RTLD_NOLOAD);
    if (handle == nullptr)
        handle = dlopen("libAstxvoid.so", RTLD_NOW);
    if (handle != nullptr)
        return handle;

    Dl_info info{};
    if (dladdr(reinterpret_cast<void*>(&OpenSkinBridge), &info) == 0 || info.dli_fname == nullptr)
        return nullptr;

    char ownPath[PATH_MAX] = {};
    strncpy(ownPath, info.dli_fname, sizeof(ownPath) - 1);
    char* ownDir = dirname(ownPath);
    if (ownDir == nullptr)
        return nullptr;

    char skinPath[PATH_MAX] = {};
    snprintf(skinPath, sizeof(skinPath), "%s/libAstxvoid.so", ownDir);
    return dlopen(skinPath, RTLD_NOW);
}

inline bool TryInitSkinBridge()
{
    if (g_SkinReady.load(std::memory_order_acquire))
        return true;

    bool expected = false;
    if (!g_SkinTried.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        return false;

    g_SkinBridge = OpenSkinBridge();
    if (g_SkinBridge == nullptr) {
        LOGW("libAstxvoid.so not found; shared attachment and kill cosmetics disabled");
        return false;
    }

    g_SkinInstall = reinterpret_cast<SkinInstallFn>(dlsym(g_SkinBridge, "AstralSkin_Install"));
    g_SkinSelect = reinterpret_cast<SkinSelectFn>(dlsym(g_SkinBridge, "AstralSkin_Select"));
    if (g_SkinInstall == nullptr || g_SkinSelect == nullptr || !g_SkinInstall()) {
        LOGW("libAstxvoid.so is missing exports or failed to start");
        dlclose(g_SkinBridge);
        g_SkinBridge = nullptr;
        g_SkinInstall = nullptr;
        g_SkinSelect = nullptr;
        return false;
    }

    g_SkinReady.store(true, std::memory_order_release);
    LOGI("skin shared library loaded");
    return true;
}

inline void WipeSkinKey(char* key, size_t size)
{
    volatile char* out = key;
    while (size-- > 0)
        *out++ = '\0';
}

inline void ApplySkinSafetyUrl(JNIEnv* env, jstring url)
{
    if (env == nullptr || url == nullptr)
        return;

    const char* raw = env->GetStringUTFChars(url, nullptr);
    if (raw == nullptr)
        return;

    void* handle = OpenSkinBridge();
    if (handle != nullptr) {
        const auto setUrl = reinterpret_cast<SkinSetUrlFn>(dlsym(handle, "AstralSkin_SetDownloadUrl"));
        const auto setKey = reinterpret_cast<SkinSetKeyFn>(dlsym(handle, "AstralSkin_SetSecretKey"));
        const auto install = reinterpret_cast<SkinInstallFn>(dlsym(handle, "AstralSkin_Install"));
        char key[32] = {};
        if (setUrl != nullptr && setKey != nullptr && install != nullptr &&
            ImGui::DecodeStyleSeed(key, sizeof(key))) {
            setKey(key);
            setUrl(raw);
            install();
        }
        WipeSkinKey(key, sizeof(key));
    }

    env->ReleaseStringUTFChars(url, raw);
}

inline int SelectSharedWeaponSkin(
    int baseId, int extraId, int blueprintId, int itemId, int lootId,
    int assetGroupId, int iconId, int defaultBroadcast, bool mythic,
    const std::string& name)
{
    if (!TryInitSkinBridge())
        return defaultBroadcast;

    AstralSkinSelection selection{};
    selection.size = sizeof(selection);
    selection.abi = ASTRAL_SKIN_ABI;
    selection.baseId = baseId;
    selection.extraId = extraId;
    selection.blueprintId = blueprintId;
    selection.itemId = itemId;
    selection.lootId = lootId;
    selection.assetGroupId = assetGroupId;
    selection.iconId = iconId;
    selection.defaultBroadcast = defaultBroadcast;
    selection.mythic = mythic ? 1 : 0;
    selection.name = name.c_str();

    const int resolved = g_SkinSelect(&selection);
    return resolved > 0 ? resolved : defaultBroadcast;
}
