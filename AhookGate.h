#pragma once

#include <dlfcn.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <jni.h>
#include <atomic>
#include <thread>

// xhook
#ifdef __cplusplus
extern "C" {
#endif
#include "xhook.h"
#ifdef __cplusplus
}
#endif

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved);

// Typedefs 
using QuerySurfaceFn = int (*)(void *dpy, void *surface, int attribute, int *value);
using CallOldSwapFn  = int (*)(void *dpy, void *surface);
using InstallHookFn  = int (*)(void *hookFunc);

extern void           *g_EglBridgeHandle;
extern QuerySurfaceFn  g_QuerySurfaceFn;
extern CallOldSwapFn   g_CallOldSwapFn;
extern InstallHookFn   g_InstallHookFn;
extern int hook_eglSwapBuffers(void *dpy, void *surface);

//  eglSwapBuffers 
static CallOldSwapFn s_OrigEglSwapBuffers = nullptr;

// hook_eglSwapBuffer
static inline int CallOrigSwap(void *dpy, void *surface)
{
    if (s_OrigEglSwapBuffers != nullptr)
        return s_OrigEglSwapBuffers(dpy, surface);
  
    using EglSwapFn = int (*)(void*, void*);
    void *libEGL = dlopen("libEGL.so", RTLD_NOW | RTLD_NOLOAD);
    if (!libEGL) libEGL = dlopen("libEGL.so", RTLD_NOW);
    if (libEGL) {
        EglSwapFn fn = (EglSwapFn)dlsym(libEGL, "eglSwapBuffers");
        if (fn) return fn(dpy, surface);
    }
    return 0;
}

static inline bool TryInitEglBridge()
{
    
    void *libEGL = dlopen("libEGL.so", RTLD_NOW | RTLD_NOLOAD);
    if (!libEGL) libEGL = dlopen("libEGL.so", RTLD_NOW);
    if (libEGL) {
        g_QuerySurfaceFn = (QuerySurfaceFn)dlsym(libEGL, "eglQuerySurface");
    }

    // g_CallOldSwapFn
    g_CallOldSwapFn = CallOrigSwap;

    // Use xhook to hook eglSwapBuffers 
    
    xhook_register(".*libEGL\\.so$",
                   "eglSwapBuffers",
                   (void *)&hook_eglSwapBuffers,
                   (void **)&s_OrigEglSwapBuffers);

    // Also hook inside the game
    xhook_register(".*libil2cpp\\.so$",
                   "eglSwapBuffers",
                   (void *)&hook_eglSwapBuffers,
                   nullptr);

    // Also hook inside unity same as other skurce ni astral ung egl sa baba
    xhook_register(".*libunity\\.so$",
                   "eglSwapBuffers",
                   (void *)&hook_eglSwapBuffers,
                   nullptr);

    int ret = xhook_refresh(0); // 0 
    if (ret != 0) {
        LOGW("xhook_refresh failed (%d), EGL hook may not be active", ret);
        return false;
    }

    LOGI("xhook EGL bridge installed successfully");
    return true;
}

static inline bool TI() { return TryInitEglBridge(); }

// JniAhook
static inline bool JniAhook(void *reserved)
{
    bool ar = TI();
    if (!ar && reserved != (void*)90710360) {
        return false;
    }
    return true;
}

// Safety stubs 
static inline void ApplyAhookSafetyKey() {}

static inline void ApplyAhookSafetyUrl(JNIEnv* env, jstring url)
{
    (void)env; (void)url;
    // ahook nourl
}

//same egl swao buffer sa other source astral using xhook egl swap buffer pinagkaiba dito, ginawa ko sya diyo sa ahook gate