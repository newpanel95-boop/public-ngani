#pragma once
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>
#include <vector>
#include <algorithm>

std::unordered_map<std::string, bool> sBool;
std::unordered_map<int, int> activeKillEffects;
std::unordered_map<int, int> activeBulletTrackEffects;
std::unordered_map<int, int> activeWeaponFireEffects;
std::unordered_map<int, int> activeWeaponBrocast;
std::unordered_map<int, int> activeVehicleSkins;
std::unordered_map<int, int> activeVehicleSkinsById;
std::unordered_map<int, void *> activeVehicleSkinConfs;
std::vector<TargetChar> g_targetCharacters;
int g_selectedTargetCharIndex = 0;

std::vector<void *> itemInventoryInstance;
std::vector<void *> weaponExtraInstance;
std::vector<void *> weaponFireEffectInstance;
std::vector<void *> weaponConfInstance;
std::vector<void *> weaponAssetGroupInstance;
std::vector<void *> mythicArmorInstance;
std::vector<void *> mythicSightInstance;
std::vector<void *> killEffectItemInstance;
std::vector<void *> weaponSkinConfigInstance;
std::vector<void *> itemResourceConfigInstance;
std::vector<void *> CharacterModelConfigInstance;
std::vector<void *> RoleConfConfigInstance;
std::vector<void *> RoleSkinConfigInstance;
std::vector<void *> RolePackConfConfigInstance;
std::vector<void *> BRDeadboxSkinConfigInstance;
std::vector<void *> BRDropPlaneSkinConfigInstance;
std::vector<void *> vehicleSkinConfigInstance;
std::vector<void *> vehicleItemConfigInstance;
std::vector<void *> activeSkisInstances;

std::vector<itemInfo> itemData;
std::vector<charInfo> charData;
std::vector<deadbox> deadboxF;
std::vector<planeID> dropplane;
std::vector<vehicleInfo> vehicleData;
RoleSkinFields* roleskinFields;
CharacterModelFields* characterfields;
RolePackFields* packfields;
RoleConfFields* roleFields;
ItemResourceFields* itemFields;
WeaponConfFields* weaponconfFields;
WeaponSkinFields* weaponskinFields;
Item2InventoryFields* item2Fields;
WeaponAssetGroupFields* weaponAssetFields;
WeaponFireEffectFields* weaponfireFields;
WeaponConfExtraFields* weaponextraFields;
MythicArmorFields* mythicarmorFields;
MythicSightFields* mythicsightFields;
KillEffectItemFields* killeffectFields;
BRDeadboxSkinFields* deadboxFields;
BRDropPlaneSkinFields* dropplaneFields;
VehicleSkinConfFields* vehicleSkinFields;
VehicleItemConfFields* vehicleItemFields;
uintptr_t Item2InventoryAddress = 0x6971D34;
uintptr_t WeaponConfAddress = 0xAF7E7FC;
uintptr_t WeaponConfExtraAddress = 0xAF80080;
uintptr_t WeaponFireEffectAddress = 0xAF81990;
uintptr_t CharacterModelAddress = 0xB1A9738;
uintptr_t BRDeadboxSkinAddress = 0x6FF3160;
uintptr_t BRDropPlaneSkinAddress = 0xB1981D4;
uintptr_t WeaponAssetGroupAddress = 0xAF7A080;
uintptr_t MythicArmorConfigAddress = 0x5C90E84;
uintptr_t MythicSightConfigAddress = 0x90DDEEC;
uintptr_t KillEffectItemConfConfigAddress = 0x90C54CC;
uintptr_t WeaponSkinAddress = 0xAF8B4D4;
uintptr_t ItemResourceAddress = 0x6985AB8;
uintptr_t RoleConfAddress = 0x90F20BC;
uintptr_t RoleSkinAddress = 0x90F95C8;
uintptr_t RolePackConfAddress = 0x90F784C;
uintptr_t WeaponConfName = 0xAF7FDC8;
uintptr_t GetDropPlaneName = 0xB198B08;
uintptr_t GetNameRoleSkin = 0x90F98F4;
uintptr_t GetRoleConfName = 0x90F4214;
uintptr_t GetDeadBoxIDAddress = 0x90F2CA0;
uintptr_t GetCurrentWeaponFireEffect = 0xA61AAA8;
uintptr_t GetCurrentBulletTrackEffect = 0xA61AB8C;
uintptr_t GetCurrentWeaponKillEffect = 0xA61AC70;
uintptr_t GetCurrentWeaponBrocast = 0xA61ADFC;
uintptr_t VehicleSkinConfAddress = 0xAF74090;
uintptr_t VehicleItemConfAddress = 0xAF73A18;
uintptr_t GetNameVehicleSkin = 0xAF74B38;
uintptr_t LoadoutBrUIHelperGetVehicleSkinAddress = 0xB299B24;
uintptr_t VehicleSkinHelperChangeVehicleSkinIdAddress = 0x9B84878;
uintptr_t VehicleSkinHelperChangeVehicleSkinConfAddress = 0x9B849EC;
uintptr_t VehicleSkisSetupSkinAddress = 0x9A9F668;
uintptr_t VehicleSkisChangeSkinIdAddress = 0x9A9F7DC;
uintptr_t VehicleSkisChangeSkinConfAddress = 0x9A9FAE0;
uintptr_t VehicleSkisBuildVehicleModelAddress = 0x9AA24A0;
uintptr_t VehicleSkisLoadModelCompleteAddress = 0x9AA26EC;
uintptr_t VehicleSkisApplySkinAddress = 0x9AA2AFC;

static constexpr int kSnowboardVehicleId = 31707110;

char searchQuery[256] = "";

bool loadskinhack = false;
bool loadCharacter = false;
bool loadVehicle = false;
bool skinBuildGraceStarted = false;
std::chrono::steady_clock::time_point skinBuildGraceStart;

std::string lastKnownName = "";
int emptyNameCount = 0;

std::unordered_map<std::string, int> nameCountMap;
std::unordered_map<std::string, int> nameCountChar;
std::unordered_map<std::string, bool> getplane;

uintptr_t location = 0;

uintptr_t getRealOffset(uintptr_t offset) {
    while (location <= 0) {
        location = Tools::GetBaseAddress("libunity.so");
        if (location <= 0) {
            usleep(1000);
        }
    }
    return location + offset;
}

std::string GetNameString(uintptr_t off, void *getadd) {
    auto getC = (String *(*)(void *)) (getRealOffset(off));
    if (Tools::IsPtrValid((void *) getC)) {
        auto getV = getC(getadd);
        std::string getB = getV->CString();
        return getB;
    }
    return "";
}

static std::string NormalizeCharacterName(const std::string& raw) {
    if (raw.empty()) return raw;
    static const std::string prefix = "LOC_ROLE_NAME_";
    if (raw.rfind(prefix, 0) != 0) return raw;

    std::string s = raw.substr(prefix.length());
    for (char &c : s) {
        if (c == '_') c = ' ';
    }
    while (!s.empty() && s.front() == ' ') s.erase(s.begin());
    while (!s.empty() && s.back() == ' ') s.pop_back();
    return s.empty() ? std::string("UnknownName") : s;
}

inline std::string GetRarityPrefix(int colorID) {
    switch (colorID) {
        case 5: return "[M] ";
        case 4: return "[L] ";
        case 3: return "[E] ";
        default: return "[C] ";
    }
}

static bool TryGetRoleSkinDisplayName(RoleConfFields* role, std::string& outName, int& outColorID) {
    if (!role) return false;
    const int candidateIDs[] = {
        role->roleFinalSuitID,
        role->roleLeftArmID,
        role->roleBasicHologramID,
        (int)role->ID
    };

    for (void* skin : RoleSkinConfigInstance) {
        if (!skin || !Tools::IsPtrValid(skin)) continue;
        auto* fields = (RoleSkinFields*)((uintptr_t)skin + 0x18);
        if (!Tools::IsPtrValid(fields)) continue;

        bool matches = false;
        for (int id : candidateIDs) {
            if (id != 0 && (int)fields->ID == id) {
                matches = true;
                break;
            }
        }
        if (!matches) continue;

        std::string name = GetNameString(GetNameRoleSkin, skin);
        if (name.empty()) continue;
        outName = name;
        outColorID = fields->ColorID;
        return true;
    }
    return false;
}

#define ReadInt(base, offset) (*(int*)((uintptr_t)(base) + (offset)))
#define ReadBool(base, offset) (*(bool*)((uintptr_t)(base) + (offset)))
#define ReadFloat(base, offset) (*(float*)((uintptr_t)(base) + (offset)))
#define ReadByte(base, offset) (*(uint8_t*)((uintptr_t)(base) + (offset)))
#define READ_PTR(type, base, offset) (*(type**)((uintptr_t)(base) + (offset)))

inline bool VehPtr(void *ptr)
{
    return ptr != nullptr && Tools::IsPtrValid(ptr);
}

inline int VehArrayLen(void *arr)
{
    if (!VehPtr(arr))
        return 0;

    const int len = *(int *)((uintptr_t)arr + 0x18);
    return len >= 0 && len < 512 ? len : 0;
}

inline void *VehicleSkinParams(void *skinConf)
{
    if (!VehPtr(skinConf))
        return nullptr;

    return *(void **)((uintptr_t)skinConf + 0xC8);
}

inline void *FindVehicleSkinConfigById(int skinId)
{
    if (skinId <= 0)
        return nullptr;

    auto exact = activeVehicleSkinConfs.find(skinId);
    if (exact != activeVehicleSkinConfs.end() && VehPtr(exact->second))
        return exact->second;

    for (void *skin : vehicleSkinConfigInstance) {
        if (!VehPtr(skin))
            continue;

        auto *fields = (VehicleSkinConfFields *)((uintptr_t)skin + 0x10);
        if (VehPtr(fields) && (int)fields->ID == skinId)
            return skin;
    }

    return nullptr;
}

inline int ActiveVehicleSkinIdForType(int vehicleType)
{
    auto it = activeVehicleSkins.find(vehicleType);
    return it != activeVehicleSkins.end() ? it->second : 0;
}

inline bool IsSnowboardSkinId(int skinId)
{
    if (skinId <= 0)
        return false;

    for (void *skin : vehicleSkinConfigInstance) {
        if (!VehPtr(skin))
            continue;

        auto *fields = (VehicleSkinConfFields *)((uintptr_t)skin + 0x10);
        if (VehPtr(fields) && fields->VehicleId == kSnowboardVehicleId && (int)fields->ID == skinId)
            return true;
    }

    return false;
}

inline int ActiveVehicleSkinIdForConf(void *skinConf)
{
    if (!VehPtr(skinConf))
        return 0;

    auto *fields = (VehicleSkinConfFields *)((uintptr_t)skinConf + 0x10);
    if (!VehPtr(fields))
        return 0;

    auto byId = activeVehicleSkinsById.find((int)fields->VehicleId);
    return byId != activeVehicleSkinsById.end() ? byId->second : ActiveVehicleSkinIdForType((int)fields->VehicleId);
}

inline void (*orig_VehicleSkis_SetupSkin)(void *instance, uint skinID, bool findItem) = nullptr;
inline void (*orig_VehicleSkis_ChangeSkinId)(void *instance, uint skinID) = nullptr;
inline void (*orig_VehicleSkis_ChangeSkinConf)(void *instance, void *skinConf) = nullptr;
inline void (*orig_VehicleSkis_BuildVehicleModel)(void *instance) = nullptr;
inline void (*orig_VehicleSkis_LoadModelComplete)(void *instance, int assetID, void *go) = nullptr;
inline uint (*orig_LoadoutBrUIHelper_GetVehicleSkin)(void *instance, int gameType) = nullptr;

inline void TrackSkisInstance(void *instance)
{
    if (!VehPtr(instance))
        return;

    for (void *known : activeSkisInstances) {
        if (known == instance)
            return;
    }

    activeSkisInstances.push_back(instance);
}

inline int ActiveSkisSkinId()
{
    const int selected = ActiveVehicleSkinIdForType(kSnowboardVehicleId);
    return selected > 0 ? selected : 0;
}

inline void ForceSkisSkinFields(void *instance, int selected)
{
    if (!VehPtr(instance) || selected <= 0)
        return;

    void *conf = FindVehicleSkinConfigById(selected);
    auto *fields = VehPtr(conf) ? (VehicleSkinConfFields *)((uintptr_t)conf + 0x10) : nullptr;
    const int mesh = VehPtr(fields) ? fields->NewVehicleResId : 0;

    *(uint *)((uintptr_t)instance + 0x88) = (uint)selected;
    if (mesh > 0) {
        *(int *)((uintptr_t)instance + 0x20) = mesh;
        *(int *)((uintptr_t)instance + 0xB8) = mesh;
    }
}

inline void ApplySkisSkinConf(void *instance, int selected)
{
    if (!VehPtr(instance) || selected <= 0 || orig_VehicleSkis_ChangeSkinConf == nullptr)
        return;

    void *conf = FindVehicleSkinConfigById(selected);
    if (VehPtr(conf))
        orig_VehicleSkis_ChangeSkinConf(instance, conf);
}

inline void ApplySkisSkinToObject(void *targetObj, int selected)
{
    if (!VehPtr(targetObj) || selected <= 0)
        return;

    void *conf = FindVehicleSkinConfigById(selected);
    if (!VehPtr(conf))
        return;

    if (VehicleSkinHelper_ChangeVehicleSkinId != nullptr)
        VehicleSkinHelper_ChangeVehicleSkinId(targetObj, (uint)selected);
    if (VehicleSkinHelper_ChangeVehicleSkinConf != nullptr)
        VehicleSkinHelper_ChangeVehicleSkinConf(targetObj, conf);
}

inline void ApplySkisMaterials(void *instance, int selected)
{
    if (!VehPtr(instance) || selected <= 0)
        return;

    void *conf = FindVehicleSkinConfigById(selected);
    void *mats = VehicleSkinParams(conf);
    if (!VehPtr(mats) || VehArrayLen(mats) <= 0 || VehicleSkis_ApplySkin == nullptr)
        return;

    *(void **)((uintptr_t)instance + 0x90) = mats;
    ForceSkisSkinFields(instance, selected);
    VehicleSkis_ApplySkin(instance);
}

inline void RefreshSkisInstances()
{
    const int selected = ActiveSkisSkinId();
    if (selected <= 0)
        return;

    for (void *instance : activeSkisInstances) {
        if (!VehPtr(instance))
            continue;

        ForceSkisSkinFields(instance, selected);
        ApplySkisSkinConf(instance, selected);
        ApplySkisMaterials(instance, selected);
        if (orig_VehicleSkis_BuildVehicleModel != nullptr)
            orig_VehicleSkis_BuildVehicleModel(instance);
        ForceSkisSkinFields(instance, selected);
        ApplySkisMaterials(instance, selected);
    }
}

inline void hook_VehicleSkis_SetupSkin(void *instance, uint skinID, bool findItem)
{
    TrackSkisInstance(instance);
    uint nextSkin = skinID;
    const int selected = ActiveSkisSkinId();
    if (selected > 0)
        nextSkin = (uint)selected;

    ForceSkisSkinFields(instance, (int)nextSkin);
    if (orig_VehicleSkis_SetupSkin != nullptr)
        orig_VehicleSkis_SetupSkin(instance, nextSkin, findItem);
    ForceSkisSkinFields(instance, (int)nextSkin);
    ApplySkisSkinConf(instance, (int)nextSkin);
}

inline void hook_VehicleSkis_ChangeSkinId(void *instance, uint skinID)
{
    TrackSkisInstance(instance);
    uint nextSkin = skinID;
    const int selected = ActiveSkisSkinId();
    if (selected > 0)
        nextSkin = (uint)selected;

    ForceSkisSkinFields(instance, (int)nextSkin);
    if (orig_VehicleSkis_ChangeSkinId != nullptr)
        orig_VehicleSkis_ChangeSkinId(instance, nextSkin);
    ForceSkisSkinFields(instance, (int)nextSkin);
    ApplySkisSkinConf(instance, (int)nextSkin);
}

inline void hook_VehicleSkis_ChangeSkinConf(void *instance, void *skinConf)
{
    TrackSkisInstance(instance);
    void *nextSkin = skinConf;
    const int selectedId = ActiveSkisSkinId() > 0 ? ActiveSkisSkinId() : ActiveVehicleSkinIdForConf(skinConf);

    if (selectedId > 0) {
        void *selected = FindVehicleSkinConfigById(selectedId);
        if (VehPtr(selected))
            nextSkin = selected;
    }

    ForceSkisSkinFields(instance, selectedId);
    if (orig_VehicleSkis_ChangeSkinConf != nullptr)
        orig_VehicleSkis_ChangeSkinConf(instance, nextSkin);
    ForceSkisSkinFields(instance, selectedId);
}

inline void hook_VehicleSkis_BuildVehicleModel(void *instance)
{
    TrackSkisInstance(instance);
    const int selected = ActiveSkisSkinId();
    ForceSkisSkinFields(instance, selected);

    if (orig_VehicleSkis_BuildVehicleModel != nullptr)
        orig_VehicleSkis_BuildVehicleModel(instance);

    ForceSkisSkinFields(instance, selected);
    ApplySkisSkinConf(instance, selected);
}

inline void hook_VehicleSkis_LoadModelComplete(void *instance, int assetID, void *go)
{
    TrackSkisInstance(instance);
    const int selected = ActiveSkisSkinId();
    ForceSkisSkinFields(instance, selected);

    if (orig_VehicleSkis_LoadModelComplete != nullptr)
        orig_VehicleSkis_LoadModelComplete(instance, assetID, go);

    ForceSkisSkinFields(instance, selected);
    ApplySkisSkinToObject(go, selected);
    ApplySkisSkinConf(instance, selected);
    ApplySkisMaterials(instance, selected);
}

inline uint hook_LoadoutBrUIHelper_GetVehicleSkin(void *instance, int gameType)
{
    const int selected = ActiveSkisSkinId();
    if (selected > 0)
        return (uint)selected;

    return orig_LoadoutBrUIHelper_GetVehicleSkin != nullptr
        ? orig_LoadoutBrUIHelper_GetVehicleSkin(instance, gameType)
        : 0;
}

inline void BuildVehicleData()
{
    if (loadVehicle || vehicleSkinConfigInstance.empty())
        return;

    vehicleData.clear();
    activeVehicleSkinConfs.clear();

    for (void *vehicleSkin : vehicleSkinConfigInstance) {
        if (!VehPtr(vehicleSkin))
            continue;

        auto *fields = (VehicleSkinConfFields *)((uintptr_t)vehicleSkin + 0x10);
        if (!VehPtr(fields) || fields->VehicleId != kSnowboardVehicleId || fields->ID == 0)
            continue;

        std::string skinName = GetNameString(GetNameVehicleSkin, vehicleSkin);
        if (skinName.empty())
            skinName = "Snowboard " + std::to_string((int)fields->ID);
        vehicleData.push_back({GetRarityPrefix(fields->ColorID) + skinName, fields->VehicleId, (int)fields->ID});
        activeVehicleSkinConfs[(int)fields->ID] = vehicleSkin;
    }

    loadVehicle = !vehicleData.empty();
}

void *GameUpdate(void *) {
    while (true) {
        if (!skinBuildGraceStarted) {
            skinBuildGraceStart = std::chrono::steady_clock::now();
            skinBuildGraceStarted = true;
        }

        const bool startupGraceElapsed =
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - skinBuildGraceStart
            ).count() >= 15;

        if (startupGraceElapsed) {
            BuildVehicleData();
        }

        if (startupGraceElapsed && !loadCharacter && CharacterModelConfigInstance.size() > 0 && itemResourceConfigInstance.size() > 0
            && RoleConfConfigInstance.size() > 0 && RoleSkinConfigInstance.size() > 0) {
            std::string _g = "";
            for (auto &z : BRDeadboxSkinConfigInstance) {
                if (!z) continue;
                auto *y = (BRDeadboxSkinFields *) ((uintptr_t) z + 0x10);
                bool __b = false;

                for (auto &q : RoleConfConfigInstance) {
                    if (!q) continue;
                    auto *f = (RoleConfFields *) ((uintptr_t) q + 0x10);
                    if (!f || f->ID == 0) {
                        f = (RoleConfFields *) ((uintptr_t) q + 0x14);
                    }
                    std::string s = GetNameString(GetRoleConfName, q);

                    if ((y->ID & 0xFFFFFFF) != 0) {
                        auto fx = (int (*)(void *))(getRealOffset(GetDeadBoxIDAddress));
                        int k = fx(q);

                        if (y->ID == k && !__b) {
                            auto dump = [&](const std::string &nm) {
                                deadboxF.push_back({GetRarityPrefix(y->ColorID) + nm, {
                                    y->ColorID,
                                    y->DeadBoxEffectAsset,
                                    y->Flag,
                                    y->FlagAsset,
                                    y->ModelAsset3P,
                                    y->ModelAssetUI
                                }});
                            };

                            if (y->ColorID == 5) {
                                _g = s;
                                dump(s);
                                __b = true;
                            } else {
                                dump(s);
                            }
                        }

                        if (__b) break;

                        if (k + 1 == y->ID) {
                            if (y->ColorID == 5 && y->FlagAsset != 0 && !_g.empty()) {
                                std::string v = _g + " (Variant)";
                                deadboxF.push_back({GetRarityPrefix(y->ColorID) + v, {
                                    y->ColorID,
                                    y->DeadBoxEffectAsset,
                                    y->Flag,
                                    y->FlagAsset,
                                    y->ModelAsset3P,
                                    y->ModelAssetUI
                                }});
                            }
                        }
                    }
                }
            }

            for (auto X0 : CharacterModelConfigInstance) {
                if (!X0) continue;
                auto *X1 = (CharacterModelFields *)((uintptr_t)X0 + 0x10);
                auto A = X1->ItemID;
                auto B = X1->BRBagModel;
                auto C = X1->BRHeadModel;
                auto D = X1->BRLobby;
                auto E = X1->BRModel;
                auto F = X1->BindEffect1P;
                auto G = X1->ChangeClipEffect1P;
                auto H = X1->DefaultModelID;
                auto I = X1->Guarder1P;
                auto J = X1->Guarder3P;
                auto K = X1->GuarderBagModel;
                auto L = X1->GuarderHeadModel;
                auto M = X1->GuarderLobby;
                auto N = X1->Traitor1P;

                for (auto Y0 : itemResourceConfigInstance) {
                    if (!Y0) continue;
                    auto *Y1 = (ItemResourceFields *)((uintptr_t)Y0 + 0x10);
                    auto A_ = Y1->AvatarModelID;
                    if (A != A_) continue;

                    auto P = Y1->FxAssetID;
                    auto Q = Y1->InventoryModelID;
                    auto R = Y1->ModelAssetIDRaw;
                    auto S = Y1->UIMiniSpriteName;
                    auto T = Y1->UISmallSpriteName;
                    auto U = Y1->UISpriteName;
                    auto V = Y1->UISquareSpriteName;
                    auto W = Y1->ID;

                    for (auto Z0 : RoleConfConfigInstance) {
                        if (!Z0) continue;
                        auto *Z1 = (RoleConfFields *)((uintptr_t)Z0 + 0x10);
                        if (!Z1 || Z1->ID == 0) {
                            Z1 = (RoleConfFields *)((uintptr_t)Z0 + 0x14);
                        }
                        if (Z1->ID != W) continue;

                        auto a = Z1->roleLeftArmID;
                        auto b = Z1->roleFinalSuitID;
                        auto c = Z1->roleBasicHologramID;
                        auto d = Z1->ColorID;
                        auto e = Z1->ColorSubID;
                        auto f = Z1->ShowRare;
                        auto g = Z1->RoleLvGroupID;
                        auto h = Z1->RolePackID;

                        int rolePackID = 0;
                        int entryAnimID = 0;
                        int gestureId = 0;
                        int handEffectUI = 0;
                        int loadingFrame = 0;
                        int killStreakSkinID = 0;
                        for (auto RP : RolePackConfConfigInstance) {
                            if (!RP) continue;
                            auto *rpF = (RolePackFields *)((uintptr_t)RP + 0x10);
                            if (rpF->RolePackID != h) continue;
                            rolePackID = rpF->RolePackID;
                            entryAnimID = rpF->EntryAnimID;
                            gestureId = rpF->GestureId;
                            handEffectUI = rpF->HandEffectUI;
                            loadingFrame = rpF->LoadingFrame;
                            killStreakSkinID = rpF->KillStreakSkinID;
                        }

                        std::string runtimeName = NormalizeCharacterName(GetNameString(GetRoleConfName, Z0));
                        if (runtimeName.empty()) {
                            if (!lastKnownName.empty()) {
                                char c_ = 'A' + emptyNameCount++;
                                runtimeName = lastKnownName + " " + c_ + "+";
                            } else runtimeName = "UnknownName";
                        } else {
                            lastKnownName = runtimeName;
                            emptyNameCount = 0;
                        }

                        std::string skinName = runtimeName;
                        int skinColorID = d;

                        std::string key = skinName + "_" + std::to_string(A);
                        if (nameCountChar.count(key)) {
                            nameCountChar[key]++;
                            skinName += " +" + std::to_string(nameCountChar[key]);
                        } else nameCountChar[key] = 0;

                        std::string Zz = GetRarityPrefix(skinColorID) + skinName;

                        if (Zz.find("A+") != std::string::npos) h--;

                        charData.push_back({
                            Zz,
                            {B, C, D, E, F, G, H, I, J, K, L, M},
                            {P, Q, R},
                            {S, T, U, V},
                            {a, b, c, d, e, f, g, h},
                            {Z1->LOCID_Name},
                            {rolePackID, entryAnimID, gestureId, handEffectUI, loadingFrame, killStreakSkinID}
                        });

                        g_targetCharacters.push_back({runtimeName, N, X1->Traitor3P, (int)W, (int)Z1->ID, rolePackID});
                    }
                    break;
                }
            }
            if (!g_targetCharacters.empty() && g_selectedTargetCharIndex == 0) {
                for (size_t i = 0; i < g_targetCharacters.size(); ++i) {
                    if (ToLower(g_targetCharacters[i].name) == "charly") {
                        g_selectedTargetCharIndex = (int)i;
                        break;
                    }
                }
            }
            loadCharacter = true;
        }

        if (startupGraceElapsed && !loadskinhack && weaponConfInstance.size() > 0 && itemInventoryInstance.size() > 0
            && weaponAssetGroupInstance.size() > 0 && weaponFireEffectInstance.size() > 0
            && weaponExtraInstance.size() > 0 && killEffectItemInstance.size() > 0
            && mythicArmorInstance.size() > 0 && mythicSightInstance.size() > 0
            && itemResourceConfigInstance.size() > 0) {
            for (auto conf : weaponConfInstance) {
                if (!conf) continue;
                auto baseID = *(int *) ((uintptr_t) conf + 0x34);
                auto confID = *(int *) ((uintptr_t) conf + 0x40);

                for (auto item : itemInventoryInstance) {
                    if (!item) continue;
                    auto itemIDbase = *(int *) ((uintptr_t) item + 0x20);
                    int itemBaseModified;

                    int itemBase;
                    if (baseID == itemIDbase) {
                        itemBase = *(int *) ((uintptr_t) item + 0x20);
                        itemBaseModified = itemBase + 200;
                    }

                    if (confID == itemIDbase) {
                        auto confcolorID = *(uint8_t *) ((uintptr_t) conf + 0x22);
                        auto itemIDskin2 = *(int *) ((uintptr_t) item + 0x24);
                        auto itemIDskin3 = *(int *) ((uintptr_t) item + 0x28);

                        std::string AName;
                        std::string getName = GetNameString(WeaponConfName, conf);

                        AName += GetRarityPrefix(confcolorID) + getName;

                        if (nameCountMap.find(AName) != nameCountMap.end()) {
                            nameCountMap[AName]++;
                            AName += " +" + std::to_string(nameCountMap[AName]);
                        } else {
                            nameCountMap[AName] = 0;
                        }

                        if (!AName.empty()) {
                            int fireIds = 0, fireIds2 = 0, assetIds = 0;
                            int attachAsset1P = 0, attachAsset3P = 0, attachAssetHip1P = 0, attachAssetHip3P = 0;

                            for (auto asset : weaponAssetGroupInstance) {
                                if (!asset) continue;
                                auto assetID = *(int *) ((uintptr_t) asset + 0x44);

                                if (itemIDskin2 == assetID) {
                                    auto fireEffectID = *(int *) ((uintptr_t) asset + 0x40);

                                    for (auto fireConf : weaponFireEffectInstance) {
                                        if (!fireConf) continue;
                                        int fireID = *(int *) ((uintptr_t) fireConf + 0x80);
                                        int assetIdBulletSmoke = *(int *) ((uintptr_t) fireConf + 0x1C);

                                        if (AName.find("[M]") != std::string::npos) {
                                            if (fireEffectID == fireID) {
                                                if (assetIdBulletSmoke != 0) {
                                                    fireIds = fireID;
                                                    attachAsset1P = *(int *) ((uintptr_t) fireConf + 0x64);
                                                    attachAsset3P = *(int *) ((uintptr_t) fireConf + 0x68);
                                                    attachAssetHip1P = *(int *) ((uintptr_t) fireConf + 0x6C);
                                                    attachAssetHip3P = *(int *) ((uintptr_t) fireConf + 0x70);
                                                    break;
                                                }

                                                int nextFireID = fireID + 1;
                                                bool found = false;

                                                for (int i = 0; i < 10; i++) {
                                                    for (auto nextFireConf : weaponFireEffectInstance) {
                                                        if (!nextFireConf) continue;
                                                        int nextFireIDCheck = *(int *) ((uintptr_t) nextFireConf + 0x80);
                                                        int nextAssetIdBulletSmoke = *(int *) ((uintptr_t) nextFireConf + 0x1C);

                                                        if (nextFireIDCheck == nextFireID && nextAssetIdBulletSmoke != 0) {
                                                            fireIds = nextFireIDCheck;
                                                            attachAsset1P = *(int *) ((uintptr_t) nextFireConf + 0x64);
                                                            attachAsset3P = *(int *) ((uintptr_t) nextFireConf + 0x68);
                                                            attachAssetHip1P = *(int *) ((uintptr_t) nextFireConf + 0x6C);
                                                            attachAssetHip3P = *(int *) ((uintptr_t) nextFireConf + 0x70);
                                                            found = true;
                                                            break;
                                                        }
                                                    }
                                                    if (found) break;
                                                    nextFireID++;
                                                }

                                                if (fireIds == 0) fireIds = fireID;
                                                fireIds2 = fireID;
                                                assetIds = assetID;
                                            }
                                        }
                                    }
                                }
                            }

                            int confbaseskin = 0, confSkinID = 0, confBrocastID = 0, confColorID = 0, confBluePrintID = 0;
                            if (confID == itemIDbase) {
                                confbaseskin = ReadInt(conf, 0x34);
                                confColorID = ReadByte(conf, 0x22);
                                confSkinID = ReadInt(conf, 0x38);
                                confBrocastID = ReadInt(conf, 0x3C);
                                confBluePrintID = ReadByte(conf, 0x38);
                            }

                            int mythicArmor = 0, deadReplay = 0, killEffect = 0, extraOrig = 0;
                            for (auto extra : weaponExtraInstance) {
                                if (!extra) continue;
                                weaponextraFields = (WeaponConfExtraFields*) ((uintptr_t) extra + 0x10);
                                auto extraID = weaponextraFields->ID;

                                if (baseID == extraID) {
                                    extraOrig = weaponextraFields->ID;
                                }

                                if (confID == extraID) {
                                    deadReplay = weaponextraFields->DefaultDeadReplayEffectId;
                                    killEffect = weaponextraFields->DefaultKillEffectId;
                                }
                            }

                            for (auto armor : mythicArmorInstance) {
                                if (!armor) continue;
                                mythicarmorFields = (MythicArmorFields*) ((uintptr_t) armor + 0x14);
                                auto armorassetID = mythicarmorFields->AssetID;
                                auto secondTab = mythicarmorFields->SecondTab;
                                auto thirdTab = mythicarmorFields->ThirdTab;

                                if (itemIDskin3 == secondTab) {
                                    if (thirdTab == 5) {
                                        if (AName.find("[M]") != std::string::npos) {
                                            mythicArmor = armorassetID;
                                        }
                                    }
                                }
                            }

                            int sightMythic = 0;
                            for (auto sight : mythicSightInstance) {
                                if (!sight) continue;
                                mythicsightFields = (MythicSightFields*) ((uintptr_t) sight + 0x10);
                                auto sightID = *(int *) ((uintptr_t) sight + 0x14);
                                auto equipArray = *(Array<int>**) ((uintptr_t) sight + 0x38);

                                if (equipArray && equipArray->getLength() > 0) {
                                    if (itemIDskin3 == equipArray->m_Items[equipArray->getLength()]) {
                                        if (AName.find("[M]") != std::string::npos) {
                                            sightMythic = sightID;
                                        }
                                    }
                                }
                            }

                            for (auto wKilling : killEffectItemInstance) {
                                if (!wKilling) continue;
                                killeffectFields = (KillEffectItemFields*) ((uintptr_t) wKilling + 0x10);
                                auto equipArray = *(Array<int> **) ((uintptr_t) wKilling + 0x90);

                                int canEquip = 0;
                                if (equipArray && equipArray->getLength() > 0) {
                                    canEquip = equipArray->m_Items[equipArray->getLength()];
                                }

                                auto realAssetIDs_1 = *(Array<int> **) ((uintptr_t) wKilling + 0x10);
                                int lastAssetID = 0;
                                if (realAssetIDs_1 && realAssetIDs_1->getLength() > 0) {
                                    lastAssetID = realAssetIDs_1->m_Items[realAssetIDs_1->getLength()];
                                }

                                if (itemIDskin3 == canEquip) {
                                    if (AName.find("[M]") != std::string::npos) {
                                        killEffect = lastAssetID;
                                    }
                                }
                            }

                            unsigned short spr1 = 0;
                            unsigned short spr2 = 0;
                            unsigned short spr3 = 0;
                            unsigned short spr4 = 0;
                            int xItem1 = 0, xItem2 = 0, xItem3 = 0;

                            for (auto itemResource : itemResourceConfigInstance) {
                                if (!itemResource) continue;
                                itemFields = (ItemResourceFields*) ((uintptr_t) itemResource + 0x10);
                                auto checkItem = itemFields->ID;
                                auto sItem1 = itemFields->FxAssetID;
                                auto sItem2 = itemFields->InventoryModelID;
                                auto sItem3 = itemFields->ModelAssetIDRaw;
                                auto sprite1 = itemFields->UISmallSpriteName;
                                auto sprite2 = itemFields->UIMiniSpriteName;
                                auto sprite3 = itemFields->UISpriteName;
                                auto sprite4 = itemFields->UISquareSpriteName;

                                if (checkItem == confID) {
                                    xItem1 = sItem1;
                                    xItem2 = sItem2;
                                    xItem3 = sItem3;
                                    spr1 = sprite1;
                                    spr2 = sprite2;
                                    spr3 = sprite3;
                                    spr4 = sprite4;
                                }
                            }

                            itemData.push_back({AName,
                                {itemBase, itemIDskin2, itemIDskin3, itemBaseModified},
                                {confbaseskin, confColorID, confID, confBrocastID, confBluePrintID},
                                {extraOrig, mythicArmor, sightMythic, deadReplay, killEffect},
                                {assetIds, fireIds, fireIds2},
                                {attachAsset1P, attachAsset3P, attachAssetHip1P, attachAssetHip3P},
                                {xItem1, xItem2, xItem3},
                                {spr1, spr2, spr3, spr4}
                            });
                        }
                    }
                }
            }
            loadskinhack = true;
        }
if (BRDropPlaneSkinConfigInstance.size() > 0) {
            for (auto planedrop : BRDropPlaneSkinConfigInstance) {
                if (!planedrop) continue;
                dropplaneFields = (BRDropPlaneSkinFields*)((uintptr_t)planedrop + 0x10);
                std::string plane = GetNameString(GetDropPlaneName, planedrop);

                if (dropplaneFields->ModelAsset1P != 0 && !getplane[plane]) {
                    getplane[plane] = true;
                    dropplane.push_back({GetRarityPrefix(dropplaneFields->ColorID) + plane, {
                        dropplaneFields->ColorID,
                        dropplaneFields->ModelAsset1P,
                        dropplaneFields->ModelAsset3P,
                        dropplaneFields->ModelAssetCutScene,
                        dropplaneFields->ModelAssetUI,
                        dropplaneFields->Priority
                    }});

                    if (getplane.size() == BRDropPlaneSkinConfigInstance.size()) {
                        break;
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    return NULL;
}

void *BP_TH(void *) {
    ProcMap libanogs, libil2cpp, libanort;
    do {
        libanogs = KittyMemory::getLibraryMap("libanogs.so");
        libil2cpp = KittyMemory::getLibraryMap("libunity.so");
        libanort = KittyMemory::getLibraryMap("libanort.so");
        sleep(1);
    } while(!libanogs.isValid() || !libil2cpp.isValid() || !libanort.isValid());
    sleep(1);

    auto safeCtorHook = [&](uintptr_t rva, std::vector<void *> &container, const char *name) {
        uintptr_t addr = getRealOffset(rva);
        if (addr != 0 && Tools::IsPtrValid((void *)addr)) {
            hookConstructor((void *)addr, container);
        } else {
            LOGW("Skip ctor hook %s (invalid addr: 0x%lX)", name, (unsigned long)addr);
        }
    };

    safeCtorHook(Item2InventoryAddress, itemInventoryInstance, "Item2Inventory");
    safeCtorHook(WeaponConfAddress, weaponConfInstance, "WeaponConf");
    safeCtorHook(WeaponConfExtraAddress, weaponExtraInstance, "WeaponConfExtra");
    safeCtorHook(WeaponFireEffectAddress, weaponFireEffectInstance, "WeaponFireEffect");
    safeCtorHook(WeaponAssetGroupAddress, weaponAssetGroupInstance, "WeaponAssetGroup");
    safeCtorHook(ItemResourceAddress, itemResourceConfigInstance, "ItemResource");
    safeCtorHook(MythicArmorConfigAddress, mythicArmorInstance, "MythicArmor");
    safeCtorHook(MythicSightConfigAddress, mythicSightInstance, "MythicSight");
    safeCtorHook(KillEffectItemConfConfigAddress, killEffectItemInstance, "KillEffectItem");
    safeCtorHook(WeaponSkinAddress, weaponSkinConfigInstance, "WeaponSkin");
    safeCtorHook(CharacterModelAddress, CharacterModelConfigInstance, "CharacterModel");
    safeCtorHook(RoleConfAddress, RoleConfConfigInstance, "RoleConf");
    safeCtorHook(RoleSkinAddress, RoleSkinConfigInstance, "RoleSkin");
    safeCtorHook(VehicleSkinConfAddress, vehicleSkinConfigInstance, "VehicleSkinConf");
    safeCtorHook(VehicleItemConfAddress, vehicleItemConfigInstance, "VehicleItemConf");

    auto hookVehicleRva = [&](uintptr_t rva, void *hook, void **orig, const char *name) {
        uintptr_t addr = KittyMemory::getAbsoluteAddress("libunity.so", rva);
        if (addr != 0 && Tools::IsPtrValid((void *)addr)) {
            Tools::Hook((void *)addr, hook, orig);
        } else {
            LOGW("Skip hook %s (invalid addr: 0x%lX)", name, (unsigned long)addr);
        }
    };

    uintptr_t vehicleSkinIdAddr = KittyMemory::getAbsoluteAddress("libunity.so", VehicleSkinHelperChangeVehicleSkinIdAddress);
    if (vehicleSkinIdAddr != 0 && Tools::IsPtrValid((void *)vehicleSkinIdAddr))
        VehicleSkinHelper_ChangeVehicleSkinId = (void (*)(void *, uint))vehicleSkinIdAddr;

    uintptr_t vehicleSkinConfAddr = KittyMemory::getAbsoluteAddress("libunity.so", VehicleSkinHelperChangeVehicleSkinConfAddress);
    if (vehicleSkinConfAddr != 0 && Tools::IsPtrValid((void *)vehicleSkinConfAddr))
        VehicleSkinHelper_ChangeVehicleSkinConf = (void (*)(void *, void *))vehicleSkinConfAddr;

    hookVehicleRva(VehicleSkisSetupSkinAddress, (void *)hook_VehicleSkis_SetupSkin, (void **)&orig_VehicleSkis_SetupSkin, "VehicleSkis_SetupSkin");
    hookVehicleRva(VehicleSkisChangeSkinIdAddress, (void *)hook_VehicleSkis_ChangeSkinId, (void **)&orig_VehicleSkis_ChangeSkinId, "VehicleSkis_ChangeSkinId");
    hookVehicleRva(VehicleSkisChangeSkinConfAddress, (void *)hook_VehicleSkis_ChangeSkinConf, (void **)&orig_VehicleSkis_ChangeSkinConf, "VehicleSkis_ChangeSkinConf");
    hookVehicleRva(VehicleSkisBuildVehicleModelAddress, (void *)hook_VehicleSkis_BuildVehicleModel, (void **)&orig_VehicleSkis_BuildVehicleModel, "VehicleSkis_BuildVehicleModel");
    hookVehicleRva(VehicleSkisLoadModelCompleteAddress, (void *)hook_VehicleSkis_LoadModelComplete, (void **)&orig_VehicleSkis_LoadModelComplete, "VehicleSkis_LoadModelComplete");
    uintptr_t skisApplySkinAddr = KittyMemory::getAbsoluteAddress("libunity.so", VehicleSkisApplySkinAddress);
    if (skisApplySkinAddr != 0 && Tools::IsPtrValid((void *)skisApplySkinAddr))
        VehicleSkis_ApplySkin = (void (*)(void *))skisApplySkinAddr;
    hookVehicleRva(LoadoutBrUIHelperGetVehicleSkinAddress, (void *)hook_LoadoutBrUIHelper_GetVehicleSkin, (void **)&orig_LoadoutBrUIHelper_GetVehicleSkin, "LoadoutBrUIHelper_GetVehicleSkin");

    uintptr_t killFxAddr = KittyMemory::getAbsoluteAddress("libunity.so", GetCurrentWeaponKillEffect);
    if (killFxAddr != 0 && Tools::IsPtrValid((void *)killFxAddr)) {
        Tools::Hook((void *)killFxAddr, (void *)_GetCurrentWeaponKillEffect, (void **)&orig_GetCurrentWeaponKillEffect);
    } else {
        LOGW("Skip hook GetCurrentWeaponKillEffect (invalid addr: 0x%lX)", (unsigned long)killFxAddr);
    }

    uintptr_t weaponFireAddr = KittyMemory::getAbsoluteAddress("libunity.so", GetCurrentWeaponFireEffect);
    if (weaponFireAddr != 0 && Tools::IsPtrValid((void *)weaponFireAddr)) {
        Tools::Hook((void *)weaponFireAddr, (void *)_GetCurrentWeaponFireEffect, (void **)&orig_GetCurrentWeaponFireEffect);
    } else {
        LOGW("Skip hook GetCurrentWeaponFireEffect (invalid addr: 0x%lX)", (unsigned long)weaponFireAddr);
    }

    uintptr_t bulletTrailAddr = KittyMemory::getAbsoluteAddress("libunity.so", GetCurrentBulletTrackEffect);
    if (bulletTrailAddr != 0 && Tools::IsPtrValid((void *)bulletTrailAddr)) {
        Tools::Hook((void *)bulletTrailAddr, (void *)_GetCurrentBulletTrackEffect, (void **)&orig_GetCurrentBulletTrackEffect);
    } else {
        LOGW("Skip hook GetCurrentBulletTrackEffect (invalid addr: 0x%lX)", (unsigned long)bulletTrailAddr);
    }

    uintptr_t broadcastAddr = KittyMemory::getAbsoluteAddress("libunity.so", GetCurrentWeaponBrocast);
    if (broadcastAddr != 0 && Tools::IsPtrValid((void *)broadcastAddr)) {
        Tools::Hook((void *)broadcastAddr, (void *)_GetCurrentWeaponBrocast, (void **)&orig_GetCurrentWeaponBrocast);
    } else {
        LOGW("Skip hook GetCurrentWeaponBrocast (invalid addr: 0x%lX)", (unsigned long)broadcastAddr);
    }
pthread_t t2;
    pthread_create(&t2, NULL, GameUpdate, NULL);

    return 0;
}

void* SuperThread(void*) {
    while (true) {
        auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto td = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(std::min(0LL, SLEEP_TIME - td), SLEEP_TIME)));
    }
    return nullptr;
}
