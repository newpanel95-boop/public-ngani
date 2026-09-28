#pragma once

#include <iomanip>
#include <sstream>
#include <ctime>

#include "../../Substrate/json.hpp"

std::string androidFilesDir = "/storage/emulated/0/Android/data/com.garena.game.codm/files/";

void SaveConfiguration(const std::string& filename) {
    nlohmann::json config;

    config["ESPMenu"]["Box"] = Config.ESPMenu.Box;
    config["ESPMenu"]["isPlayerLine"] = Config.ESPMenu.isPlayerLine;
    config["ESPMenu"]["Name"] = Config.ESPMenu.Name;
    config["ESPMenu"]["Health"] = Config.ESPMenu.Health;
    config["ESPMenu"]["Distance"] = Config.ESPMenu.Distance;
    config["ESPMenu"]["Alert"] = Config.ESPMenu.Alert;
    config["ESPMenu"]["Count"] = Config.ESPMenu.Count;
    config["ESPMenu"]["Crosshair"] = Config.ESPMenu.Crosshair;
    config["ESPMenu"]["Aimline"] = Config.ESPMenu.Aimline;
    config["ESPMenu"]["Skeleton"] = Config.ESPMenu.Skeleton;
    config["ESPMenu"]["Target"] = static_cast<int>(Config.ESPMenu.Target);
    config["ESPMenu"]["BoxType"] = static_cast<int>(Config.ESPMenu.BoxType);
    config["ESPMenu"]["HealthPosition"] = static_cast<int>(Config.ESPMenu.HealthPosition);
    config["ESPMenu"]["CrosshairType"] = static_cast<int>(Config.ESPMenu.CrosshairType);
    config["ESPMenu"]["EspStyle"] = static_cast<int>(Config.ESPMenu.EspStyle);

    config["Aim"]["AimSilent"] = Config.Aim.AimSilent;
    config["Aim"]["Aimbot360"] = Config.Aim.Aimbot360;
    config["Aim"]["AimAssistSize"] = Config.Aim.AimAssistSize;
    config["Aim"]["Target"] = static_cast<int>(Config.Aim.Target);
    config["Aim"]["Trigger"] = static_cast<int>(Config.Aim.Trigger);
    config["Aim"]["By"] = static_cast<int>(Config.Aim.By);
    config["Aim"]["size"] = Config.Aim.size;
    config["Aim"]["Cross"] = Config.Aim.Cross;

    config["ExtraMenu"]["Flash"] = Config.ExtraMenu.Flash;
    config["ExtraMenu"]["Diving"] = Config.ExtraMenu.Diving;
    config["ExtraMenu"]["Fire"] = Config.ExtraMenu.Fire;
    config["ExtraMenu"]["Hit"] = Config.ExtraMenu.Hit;
    config["ExtraMenu"]["Rpd"] = Config.ExtraMenu.Rpd;
    config["ExtraMenu"]["Parachute"] = Config.ExtraMenu.Parachute;
    config["ExtraMenu"]["Recoil"] = Config.ExtraMenu.Recoil;
    config["ExtraMenu"]["Shake"] = Config.ExtraMenu.Shake;
    config["ExtraMenu"]["Spread"] = Config.ExtraMenu.Spread;
    config["ExtraMenu"]["Reload"] = Config.ExtraMenu.Reload;
    config["ExtraMenu"]["Scope"] = Config.ExtraMenu.Scope;
    config["ExtraMenu"]["Switch"] = Config.ExtraMenu.Switch;
    config["ExtraMenu"]["FastArmorPlate"] = Config.ExtraMenu.FastArmorPlate;
    config["ExtraMenu"]["Kinetic"] = Config.ExtraMenu.Kinetic;
    config["ExtraMenu"]["KineticArmor"] = Config.ExtraMenu.Kinetic;
    config["ExtraMenu"]["WallHack"] = Config.ExtraMenu.WallHack;
    config["ExtraMenu"]["ClearDisplay"] = Config.ExtraMenu.ClearDisplay;
    config["SlideRange"] = SlideRange;
    config["Bline"] = Config.Bline;
    config["Pline"] = Config.Pline;
    config["isJumpAdjustmentEnabled"] = isJumpAdjustmentEnabled;
    config["jumpHeightMultiplier"] = jumpHeightMultiplier;
    config["SnowB"] = SnowB;
    config["SnowBsize"] = SnowBsize;
    config["isSpeedHackEnabled"] = isSpeedHackEnabled;
    config["speedHackMultiplier"] = speedHackMultiplier;

    config["menu"][0] = menu[0];
    config["menu"][1] = menu[1];
    config["menu"][2] = menu[2];

    std::string configDir = androidFilesDir + "configs";
    mkdir(configDir.c_str(), 0777);

    std::string filePath = configDir + "/" + filename + ".json";
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << std::setw(4) << config;
        file.close();
    }
}

bool LoadConfiguration(const std::string& filename) {
    try {
        std::string filePath = androidFilesDir + "configs/" + filename + ".json";
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json config;
        file >> config;

        if (config.contains("ESPMenu")) {
            if (config["ESPMenu"].contains("Box")) Config.ESPMenu.Box = config["ESPMenu"]["Box"].get<bool>();
            if (config["ESPMenu"].contains("isPlayerLine")) Config.ESPMenu.isPlayerLine = config["ESPMenu"]["isPlayerLine"].get<bool>();
            if (config["ESPMenu"].contains("Name")) Config.ESPMenu.Name = config["ESPMenu"]["Name"].get<bool>();
            if (config["ESPMenu"].contains("Health")) Config.ESPMenu.Health = config["ESPMenu"]["Health"].get<bool>();
            if (config["ESPMenu"].contains("Distance")) Config.ESPMenu.Distance = config["ESPMenu"]["Distance"].get<bool>();
            if (config["ESPMenu"].contains("Alert")) Config.ESPMenu.Alert = config["ESPMenu"]["Alert"].get<bool>();
            if (config["ESPMenu"].contains("Count")) Config.ESPMenu.Count = config["ESPMenu"]["Count"].get<bool>();
            if (config["ESPMenu"].contains("Crosshair")) Config.ESPMenu.Crosshair = config["ESPMenu"]["Crosshair"].get<bool>();
            if (config["ESPMenu"].contains("Aimline")) Config.ESPMenu.Aimline = config["ESPMenu"]["Aimline"].get<bool>();
            if (config["ESPMenu"].contains("Skeleton")) Config.ESPMenu.Skeleton = config["ESPMenu"]["Skeleton"].get<bool>();
            if (config["ESPMenu"].contains("Target")) Config.ESPMenu.Target = static_cast<LineTarget>(config["ESPMenu"]["Target"].get<int>());
            if (config["ESPMenu"].contains("BoxType")) Config.ESPMenu.BoxType = static_cast<EspBoxType>(config["ESPMenu"]["BoxType"].get<int>());
            if (config["ESPMenu"].contains("HealthPosition")) Config.ESPMenu.HealthPosition = static_cast<EspHealthPosition>(config["ESPMenu"]["HealthPosition"].get<int>());
            if (config["ESPMenu"].contains("CrosshairType")) Config.ESPMenu.CrosshairType = static_cast<CrosshairTarget>(config["ESPMenu"]["CrosshairType"].get<int>());
            if (config["ESPMenu"].contains("EspStyle")) Config.ESPMenu.EspStyle = static_cast<EspStyleTarget>(config["ESPMenu"]["EspStyle"].get<int>());
        }

        if (config.contains("Aim")) {
            if (config["Aim"].contains("Aimbot360")) Config.Aim.Aimbot360 = config["Aim"]["Aimbot360"].get<bool>();
            if (config["Aim"].contains("AimSilent")) Config.Aim.AimSilent = config["Aim"]["AimSilent"].get<bool>();
            if (config["Aim"].contains("AimAssistSize")) Config.Aim.AimAssistSize = config["Aim"]["AimAssistSize"].get<float>();
            if (config["Aim"].contains("Target")) Config.Aim.Target = static_cast<EAimTarget>(config["Aim"]["Target"].get<int>());
            if (config["Aim"].contains("Trigger")) Config.Aim.Trigger = static_cast<EAimTrigger>(config["Aim"]["Trigger"].get<int>());
            if (config["Aim"].contains("By")) Config.Aim.By = static_cast<EAim>(config["Aim"]["By"].get<int>());
            if (config["Aim"].contains("size")) Config.Aim.size = config["Aim"]["size"].get<float>();
            if (config["Aim"].contains("Cross"))
                Config.Aim.Cross = config["Aim"]["Cross"].get<float>();
            else
                Config.Aim.Cross = Config.Aim.size;
        }

        if (config.contains("ExtraMenu")) {
            if (config["ExtraMenu"].contains("Flash")) Config.ExtraMenu.Flash = config["ExtraMenu"]["Flash"].get<bool>();
            if (config["ExtraMenu"].contains("Diving")) Config.ExtraMenu.Diving = config["ExtraMenu"]["Diving"].get<bool>();
            if (config["ExtraMenu"].contains("Fire")) Config.ExtraMenu.Fire = config["ExtraMenu"]["Fire"].get<bool>();
            if (config["ExtraMenu"].contains("Hit")) Config.ExtraMenu.Hit = config["ExtraMenu"]["Hit"].get<bool>();
            if (config["ExtraMenu"].contains("Rpd")) Config.ExtraMenu.Rpd = config["ExtraMenu"]["Rpd"].get<bool>();
            if (config["ExtraMenu"].contains("Parachute")) Config.ExtraMenu.Parachute = config["ExtraMenu"]["Parachute"].get<bool>();
            if (config["ExtraMenu"].contains("Recoil")) Config.ExtraMenu.Recoil = config["ExtraMenu"]["Recoil"].get<bool>();
            if (config["ExtraMenu"].contains("Shake")) Config.ExtraMenu.Shake = config["ExtraMenu"]["Shake"].get<bool>();
            if (config["ExtraMenu"].contains("Spread")) Config.ExtraMenu.Spread = config["ExtraMenu"]["Spread"].get<bool>();
            if (config["ExtraMenu"].contains("Reload")) Config.ExtraMenu.Reload = config["ExtraMenu"]["Reload"].get<bool>();
            if (config["ExtraMenu"].contains("Scope")) Config.ExtraMenu.Scope = config["ExtraMenu"]["Scope"].get<bool>();
            if (config["ExtraMenu"].contains("Switch")) Config.ExtraMenu.Switch = config["ExtraMenu"]["Switch"].get<bool>();
            if (config["ExtraMenu"].contains("FastArmorPlate")) Config.ExtraMenu.FastArmorPlate = config["ExtraMenu"]["FastArmorPlate"].get<bool>();
            if (config["ExtraMenu"].contains("Kinetic")) {
                Config.ExtraMenu.Kinetic = config["ExtraMenu"]["Kinetic"].get<bool>();
            } else if (config["ExtraMenu"].contains("KineticArmor")) {
                Config.ExtraMenu.Kinetic = config["ExtraMenu"]["KineticArmor"].get<bool>();
            }
            if (config["ExtraMenu"].contains("WallHack"))
                Config.ExtraMenu.WallHack = config["ExtraMenu"]["WallHack"].get<bool>();
            if (config["ExtraMenu"].contains("ClearDisplay"))
                Config.ExtraMenu.ClearDisplay = config["ExtraMenu"]["ClearDisplay"].get<bool>();
        }

        if (config.contains("SlideRange"))
            SlideRange = config["SlideRange"].get<float>();
        if (config.contains("Bline"))
            Config.Bline = config["Bline"].get<float>();
        if (config.contains("Pline"))
            Config.Pline = config["Pline"].get<float>();

        if (config.contains("isJumpAdjustmentEnabled"))
            isJumpAdjustmentEnabled = config["isJumpAdjustmentEnabled"].get<bool>();
        if (config.contains("jumpHeightMultiplier"))
            jumpHeightMultiplier = config["jumpHeightMultiplier"].get<float>();
        if (config.contains("SnowB"))
            SnowB = config["SnowB"].get<bool>();
        if (config.contains("SnowBsize"))
            SnowBsize = config["SnowBsize"].get<float>();
        if (config.contains("isSpeedHackEnabled"))
            isSpeedHackEnabled = config["isSpeedHackEnabled"].get<bool>();
        if (config.contains("speedHackMultiplier"))
            speedHackMultiplier = config["speedHackMultiplier"].get<float>();

        if (config.contains("menu")) {
            menu[0] = config["menu"][0].get<float>();
            menu[1] = config["menu"][1].get<float>();
            menu[2] = config["menu"][2].get<float>();
        }

        return true;
    }
    catch (const std::exception& e) {
        return false;
    }
}
