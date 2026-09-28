#pragma once

#include <cstdint>
#include <string>
#include <vector>

extern bool SnowB;
extern float SnowBsize;
extern bool isSpeedHackEnabled;
extern float speedHackMultiplier;
extern bool isJumpAdjustmentEnabled;
extern float jumpHeightMultiplier;
extern float SlideRange;

inline float (*orig_GetAssitAimSpeed)(void *, Vector3, float, float, float, bool, bool) = nullptr;
inline float GetAssitAimSpeed(void * instance, Vector3 assistCentorPos, float assistDis, float dis, float angle, bool isPVE, bool gamepadInput) {
    if (instance != NULL) {
        if (Config.Aim.AimAssistSize > 0.0f) {
            return (float)Config.Aim.AimAssistSize;
        }
    }
    return orig_GetAssitAimSpeed(instance, assistCentorPos, assistDis, dis, angle, isPVE, gamepadInput);
}

inline void (*orig_OnFlashBangExplode)(void *, int, float, float, float) = nullptr;
inline void hook_OnFlashBangExplode(void *instance, int weaponItemID, float whiteTime, float whiteAlphaTime, float initIntensity) {
    if (instance != nullptr && Config.ExtraMenu.Flash) {
        whiteTime = 0.1f;
        whiteAlphaTime = 0.1f;
        initIntensity = 0.1f;
    }
    orig_OnFlashBangExplode(instance, weaponItemID, whiteTime, whiteAlphaTime, initIntensity);
}

inline float (*orig_get_FireBoltTime)(void *) = nullptr;
inline float get_FireBoltTime(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fire) {
            return 0.00001f;
        }
    }
    return orig_get_FireBoltTime(instance);
}

inline float (*orig_get_FireInterval)(void *) = nullptr;
inline float get_FireInterval(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fire) {
            return 0.00001f;
        }
    }
    return orig_get_FireInterval(instance);
}

inline float (*orig_get_DelaySprintFire)(void *) = nullptr;
inline float get_DelaySprintFire(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Fire) {
            return 0.00001f;
        }
    }
    return orig_get_DelaySprintFire(instance);
}

typedef void (*SetUltraFrameRateDeviceInfo_t)(void* thiz, bool enableUltraFrameRate, int ultraFrameRate, int ultraFrameRateBR, int ultraFrameRateQualityLimit, bool customizedFrameRate);
SetUltraFrameRateDeviceInfo_t orig_SetUltraFrameRateDeviceInfo;

void hooked_SetUltraFrameRateDeviceInfo(void* thiz, bool enableUltraFrameRate, int ultraFrameRate, int ultraFrameRateBR, int ultraFrameRateQualityLimit, bool customizedFrameRate) {
    if (thiz != NULL) {
        enableUltraFrameRate = true;
        ultraFrameRate = 185;
        ultraFrameRateBR = 185;
        ultraFrameRateQualityLimit = 185;
        customizedFrameRate = true;
    }
    orig_SetUltraFrameRateDeviceInfo(thiz, enableUltraFrameRate, ultraFrameRate, ultraFrameRateBR, ultraFrameRateQualityLimit, customizedFrameRate);
}

inline float (*orig_GetMaxJumpHeight)(void*) = nullptr;
inline float hook_GetMaxJumpHeight(void* instance) {
    float orig_ = orig_GetMaxJumpHeight(instance);
    return (jumpHeightMultiplier > 1.0f) ? orig_ * jumpHeightMultiplier : orig_;
}

inline bool (*orig_SingleLineCheckPhysics)(void* instance, int hitType, void* hitTarget, void* hitCollider, Vector3 startPos, Vector3 dir, void* impactInfo) = nullptr;
inline bool SingleLineCheckPhysics(void* instance, int hitType, void* hitTarget, void* hitCollider, Vector3 startPos, Vector3 dir, void* impactInfo) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Hit) {
            return true;
        }
    }
    return orig_SingleLineCheckPhysics(instance, hitType, hitTarget, hitCollider, startPos, dir, impactInfo);
}

inline float (*o_get_SlideTackleAcclerationSpeed)(void*) = nullptr;
inline float h_get_SlideTackleAcclerationSpeed(void* ins) {
    if (SlideRange > 0.0f) {
        return SlideRange + 1;
    }
    return o_get_SlideTackleAcclerationSpeed(ins);
}

inline float (*o_PawnGetMaxSpeed)(void*) = nullptr;
inline float h_PawnGetMaxSpeed(void* ins) {
    if (SlideRange > 0.0f) {
        return SlideRange + 256;
    }
    return o_PawnGetMaxSpeed(ins);
}

inline float (*o_get_SlideTackleSpeed)(void*) = nullptr;
inline float h_get_SlideTackleSpeed(void* ins) {
    if (SlideRange > 0.0f) {
        return SlideRange;
    }
    return o_get_SlideTackleSpeed(ins);
}

inline float (*o_GetSuperSlideRate)(void*) = nullptr;
inline float h_GetSuperSlideRate(void* ins) {
    if (SlideRange > 0.0f) {
        return SlideRange + 256;
    }
    return o_GetSuperSlideRate(ins);
}

inline float (*orig_get_AddHotTime)(void* instance) = nullptr;
inline float get_AddHotTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Rpd) {
            return 0.00001f;
        }
    }
    return orig_get_AddHotTime(instance);
}

inline void (*orig_OpenParachute)(void* instance, bool isAuto) = nullptr;
inline void OpenParachute(void* instance, bool isAuto) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Parachute) {
            return;
        }
    }
    return orig_OpenParachute(instance, isAuto);
}

inline float (*orig_get_ChangeClipTime)(void* instance) = nullptr;
inline float get_ChangeClipTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.0001f;
        }
    }
    return orig_get_ChangeClipTime(instance);
}

inline float (*orig_get_ChangeClipLoopTime)(void* instance) = nullptr;
inline float get_ChangeClipLoopTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Reload) {
            return 0.0001f;
        }
    }
    return orig_get_ChangeClipLoopTime(instance);
}

inline float (*orig_get_AimingTime)(void* instance) = nullptr;
inline float get_AimingTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Scope) {
            return 0.0001f;
        }
    }
    return orig_get_AimingTime(instance);
}

inline float (*orig_get_EquipTime)(void* instance) = nullptr;
inline float get_EquipTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.0001f;
        }
    }
    return orig_get_EquipTime(instance);
}

inline float (*orig_get_UnequipTime)(void* instance) = nullptr;
inline float get_UnequipTime(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Switch) {
            return 0.0001f;
        }
    }
    return orig_get_UnequipTime(instance);
}

inline constexpr float kPlateTime = 2.75f;
inline constexpr uintptr_t kPlateTimeOff = 0x30;
inline constexpr uintptr_t kUseCompTimeOff = 0xB8;

inline void SetObjFloat(void *instance, uintptr_t offset, float value)
{
    if (instance != nullptr)
        *reinterpret_cast<float *>(reinterpret_cast<uintptr_t>(instance) + offset) = value;
}

inline int (*orig_BRUseItemArmorPlate_SetUseItemTime)(void *instance, uint32_t playerId) = nullptr;
inline void *g_fastPlateInst = nullptr;

inline int hook_BRUseItemArmorPlate_SetUseItemTime(void *instance, uint32_t playerId)
{
    int ret = orig_BRUseItemArmorPlate_SetUseItemTime(instance, playerId);
    if (Config.ExtraMenu.FastArmorPlate && instance != nullptr && ret == 1) {
        g_fastPlateInst = instance;
        SetObjFloat(instance, kPlateTimeOff, kPlateTime);
    }
    return ret;
}

inline void (*orig_BRUseItemArmorPlate_Tick)(void *instance, void *useItemComponent) = nullptr;
inline void hook_BRUseItemArmorPlate_Tick(void *instance, void *useItemComponent)
{
    orig_BRUseItemArmorPlate_Tick(instance, useItemComponent);

    if (Config.ExtraMenu.FastArmorPlate && instance == g_fastPlateInst) {
        SetObjFloat(instance, kPlateTimeOff, kPlateTime);
        SetObjFloat(useItemComponent, kUseCompTimeOff, kPlateTime);
    }
}

bool (*orig_IsInEM3Eye)(void *instance);
bool get_IsInEM3Eye(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.RedWallhack) {
            return true;
        }
    }
    return orig_IsInEM3Eye(instance);
}

float (*orig_GetAccDistance)(void *instance);
float GetAccDistance(void *instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.RedWallhack) {
            return 50.0f;
        }
    }
    return orig_GetAccDistance(instance);
}

inline bool IsTutorialEnabled() {
    return false;
}

inline bool InGameRadarEnabled()
{
    return false;
}

inline bool (*orig_get_AdvanceUAVEnabled)(void*) = nullptr;
inline bool hook_get_AdvanceUAVEnabled(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_get_AdvanceUAVEnabled(instance);
}

inline bool (*orig_ShowFireLocOnRadar)(void*) = nullptr;
inline bool hook_ShowFireLocOnRadar(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_ShowFireLocOnRadar(instance);
}

inline bool (*orig_get_ShowOnRadarBySonar)(void*) = nullptr;
inline bool hook_get_ShowOnRadarBySonar(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_get_ShowOnRadarBySonar(instance);
}

inline bool (*orig_get_ShowOnRadarByThreat)(void*) = nullptr;
inline bool hook_get_ShowOnRadarByThreat(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_get_ShowOnRadarByThreat(instance);
}

inline bool (*orig_CanShowOnRadarForEnemy)(void*) = nullptr;
inline bool hook_CanShowOnRadarForEnemy(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_CanShowOnRadarForEnemy(instance);
}

inline bool (*orig_get_NeedShowOnMap)(void*) = nullptr;
inline bool hook_get_NeedShowOnMap(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_get_NeedShowOnMap(instance);
}

inline bool (*orig_NeedToShowEnemySpriteOnRadar)(void*) = nullptr;
inline bool hook_NeedToShowEnemySpriteOnRadar(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_NeedToShowEnemySpriteOnRadar(instance);
}

inline bool (*orig_BRGameInfo_WillShowEnemyOnRadar)(void*) = nullptr;
inline bool hook_BRGameInfo_WillShowEnemyOnRadar(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRGameInfo_WillShowEnemyOnRadar(instance);
}

inline bool (*orig_BRGameInfo_ShouldShowEnemyOnRadar)(void*, void*) = nullptr;
inline bool hook_BRGameInfo_ShouldShowEnemyOnRadar(void* instance, void* pawn)
{
    if (instance != nullptr && pawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRGameInfo_ShouldShowEnemyOnRadar(instance, pawn);
}

inline bool (*orig_BRArmored_WillShowEnemyOnRadar)(void*) = nullptr;
inline bool hook_BRArmored_WillShowEnemyOnRadar(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRArmored_WillShowEnemyOnRadar(instance);
}

inline bool (*orig_BRArmored_ShouldShowEnemyOnRadar)(void*, void*) = nullptr;
inline bool hook_BRArmored_ShouldShowEnemyOnRadar(void* instance, void* pawn)
{
    if (instance != nullptr && pawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRArmored_ShouldShowEnemyOnRadar(instance, pawn);
}

inline bool (*orig_TacticalRadarViewBR_PreDeterminedShouldShowEnemyPawnList)(void*) = nullptr;
inline bool hook_TacticalRadarViewBR_PreDeterminedShouldShowEnemyPawnList(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarViewBR_PreDeterminedShouldShowEnemyPawnList(instance);
}

inline bool (*orig_TacticalRadarViewBR_UpdateEnemyShowOnMap)(void*, void*, void*, bool) = nullptr;
inline bool hook_TacticalRadarViewBR_UpdateEnemyShowOnMap(void* instance, void* inSprite, void* inPawn, bool onEdge)
{
    if (instance != nullptr && inPawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarViewBR_UpdateEnemyShowOnMap(instance, inSprite, inPawn, onEdge);
}

inline bool (*orig_TacticalMapViewBR_PreDeterminedShouldShowEnemyPawnList)(void*) = nullptr;
inline bool hook_TacticalMapViewBR_PreDeterminedShouldShowEnemyPawnList(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalMapViewBR_PreDeterminedShouldShowEnemyPawnList(instance);
}

inline bool (*orig_TacticalMapViewBR_UpdateEnemyShowOnMap)(void*, void*, void*, bool) = nullptr;
inline bool hook_TacticalMapViewBR_UpdateEnemyShowOnMap(void* instance, void* inSprite, void* inPawn, bool onEdge)
{
    if (instance != nullptr && inPawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalMapViewBR_UpdateEnemyShowOnMap(instance, inSprite, inPawn, onEdge);
}

inline bool (*orig_BRArmored_IsScanEnable)(void*) = nullptr;
inline bool hook_BRArmored_IsScanEnable(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRArmored_IsScanEnable(instance);
}

inline bool (*orig_BRArmored_IsPawnShouldOnRadar)(void*, void*, float) = nullptr;
inline bool hook_BRArmored_IsPawnShouldOnRadar(void* instance, void* pawn, float range)
{
    if (instance != nullptr && pawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_BRArmored_IsPawnShouldOnRadar(instance, pawn, range);
}

inline bool (*orig_TacticalRadarViewMP_IsUAVActive)(void*) = nullptr;
inline bool hook_TacticalRadarViewMP_IsUAVActive(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarViewMP_IsUAVActive(instance);
}

inline bool (*orig_TacticalRadarViewMP_ShowEnemySpriteByUAVEffect)(void*) = nullptr;
inline bool hook_TacticalRadarViewMP_ShowEnemySpriteByUAVEffect(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarViewMP_ShowEnemySpriteByUAVEffect(instance);
}

inline bool (*orig_TacticalRadarView_IsUAVActive)(void*) = nullptr;
inline bool hook_TacticalRadarView_IsUAVActive(void* instance)
{
    if (instance != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarView_IsUAVActive(instance);
}

inline bool (*orig_TacticalRadarView_IsRadarEnemyShowDurationByUAV)(void*, void*) = nullptr;
inline bool hook_TacticalRadarView_IsRadarEnemyShowDurationByUAV(void* instance, void* pawn)
{
    if (instance != nullptr && pawn != nullptr && InGameRadarEnabled())
        return true;
    return orig_TacticalRadarView_IsRadarEnemyShowDurationByUAV(instance, pawn);
}

inline float (*orig_get_AccelerationForwardSpeedUp)(void* instance) = nullptr;
inline float get_AccelerationForwardSpeedUp(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 200.0f;
        }
    }
    return orig_get_AccelerationForwardSpeedUp(instance);
}

inline float (*orig_get_MaxVelocityForwardSpeedUp)(void* instance) = nullptr;
inline float get_MaxVelocityForwardSpeedUp(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Diving) {
            return 200.0f;
        }
    }
    return orig_get_MaxVelocityForwardSpeedUp(instance);
}

inline float (*get_m_PhysSkisMaxSpeed)(void*) = nullptr;
inline float hooked_get_m_PhysSkisMaxSpeed(void* instance) {
    if (SnowBsize > 0.0f) {
        return SnowBsize;
    }
    return get_m_PhysSkisMaxSpeed(instance);
}

inline float (*original_CalcFinalMoveScale)(void*) = nullptr;
inline float hooked_CalcFinalMoveScale(void* instance) {
    if (instance == nullptr) {
        return original_CalcFinalMoveScale(instance);
    }
    if (speedHackMultiplier > 1.0f && speedHackMultiplier <= 100.0f) {
        return speedHackMultiplier;
    }
    return original_CalcFinalMoveScale(instance);
}

inline bool (*orig_get_IsKineticArmor)(void* instance) = nullptr;
inline bool get_IsKineticArmor(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Kinetic) {
            return true;
        }
    }
    return orig_get_IsKineticArmor(instance);
}

inline float (*orig_GetScaleRecoil)(void* instance) = nullptr;
inline float GetScaleRecoil(void* instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Recoil) {
            return 0.00001f;
        }
    }
    return orig_GetScaleRecoil(instance);
}

inline float (*orig_MinInaccuracy)(void *) = nullptr;
inline float MinInaccuracy(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return 0.00001f;
        }
    }
    return orig_MinInaccuracy(instance);
}

inline float (*orig_MaxInaccuracy)(void *) = nullptr;
inline float MaxInaccuracy(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return 0.00001f;
        }
    }
    return orig_MaxInaccuracy(instance);
}

inline float (*orig_DisperseBase)(void *) = nullptr;
inline float DisperseBase(void * instance) {
    if (instance != NULL) {
        if (Config.ExtraMenu.Spread) {
            return 0.00001f;
        }
    }
    return orig_DisperseBase(instance);
}

inline void InitializeAllHooks() {
    HOOK_LIB("libunity.so", "0x666FD90", GetAssitAimSpeed, orig_GetAssitAimSpeed);
    HOOK_LIB("libunity.so", "0x51E977C", hook_OnFlashBangExplode, orig_OnFlashBangExplode);
    HOOK_LIB("libunity.so", "0x51237F4", get_FireBoltTime, orig_get_FireBoltTime);
    HOOK_LIB("libunity.so", "0x5105C74", get_FireInterval, orig_get_FireInterval);
    HOOK_LIB("libunity.so", "0x513CBC4", get_DelaySprintFire, orig_get_DelaySprintFire);

    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FDEE28), (void*)hooked_SetUltraFrameRateDeviceInfo, (void**)&orig_SetUltraFrameRateDeviceInfo);

    HOOK_LIB("libunity.so", "0x5221D00", hook_GetMaxJumpHeight, orig_GetMaxJumpHeight);
    HOOK_LIB("libunity.so", "0xC1514C0", SingleLineCheckPhysics, orig_SingleLineCheckPhysics);
    HOOK_LIB("libunity.so", "0x68823FC", OpenParachute, orig_OpenParachute);
    HOOK_LIB("libunity.so", "0x50ECADC", get_ChangeClipTime, orig_get_ChangeClipTime);
    HOOK_LIB("libunity.so", "0x4ED49A4", get_AimingTime, orig_get_AimingTime);
    HOOK_LIB("libunity.so", "0x50ED8D4", get_EquipTime, orig_get_EquipTime);
    HOOK_LIB("libunity.so", "0x9677554", get_IsInEM3Eye, orig_IsInEM3Eye);
    HOOK_LIB("libunity.so", "0xAD1DD78", GetAccDistance, orig_GetAccDistance);
    HOOK_LIB_NO_ORIG("libunity.so", "0x9DE0E58", IsTutorialEnabled);
    HOOK_LIB("libunity.so", "0x5985F8C", hook_get_AdvanceUAVEnabled, orig_get_AdvanceUAVEnabled);
    HOOK_LIB("libunity.so", "0x93CEF00", hook_BRUseItemArmorPlate_SetUseItemTime, orig_BRUseItemArmorPlate_SetUseItemTime);
    HOOK_LIB("libunity.so", "0x93CE924", hook_BRUseItemArmorPlate_Tick, orig_BRUseItemArmorPlate_Tick);

    HOOK_LIB("libunity.so", "0x5DE981C", get_AccelerationForwardSpeedUp, orig_get_AccelerationForwardSpeedUp);
    HOOK_LIB("libunity.so", "0x5DE9880", get_MaxVelocityForwardSpeedUp, orig_get_MaxVelocityForwardSpeedUp);
    HOOK_LIB("libunity.so", "0x522860C", hooked_get_m_PhysSkisMaxSpeed, get_m_PhysSkisMaxSpeed);
    HOOK_LIB("libunity.so", "0x51D2EB8", hooked_CalcFinalMoveScale, original_CalcFinalMoveScale);
    HOOK_LIB("libunity.so", "0x51B3D64", get_IsKineticArmor, orig_get_IsKineticArmor);
    HOOK_LIB("libunity.so", "0xC9BAFF8", GetScaleRecoil, orig_GetScaleRecoil);
    HOOK_LIB("libunity.so", "0xC9B8F78", MinInaccuracy, orig_MinInaccuracy);
    HOOK_LIB("libunity.so", "0xC159288", MaxInaccuracy, orig_MaxInaccuracy);
    HOOK_LIB("libunity.so", "0xC9C76C4", DisperseBase, orig_DisperseBase);
}
