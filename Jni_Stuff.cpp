#include <jni.h>
#include "obfuscate.h"
#include "Logger.h"
#include "AhookGate.h"
#include "SkinBridge.h"

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_MainActivity_applyAhookSafetyUrl(JNIEnv* env, jclass, jstring url)
{
    ApplyAhookSafetyUrl(env, url);
}

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_MainActivity_applySkinSafetyUrl(JNIEnv* env, jclass, jstring url)
{
    ApplySkinSafetyUrl(env, url);
}
