#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include <string>
#include <iostream>
#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"
#include "logging.hpp"

#define NYA_MOD_PATH_FORMAT "/sdcard/ModData/{}/Mods/Nya/"

namespace NyaGlobals {
        static std::string nyaPath = "/sdcard/Pictures/Nya/";
        static std::string imagesPathSFW = nyaPath + "sfw/";
        static std::string imagesPathNSFW = nyaPath + "nsfw/";
        static std::string tempPath = fmt::format(NYA_MOD_PATH_FORMAT, modloader::get_application_id().c_str()) + "temp/";
}

DECLARE_JSON_CLASS(EndpointConfig,
    NAMED_VALUE_OPTIONAL(std::string, sfw, "sfw");
    NAMED_VALUE_OPTIONAL(std::string, nsfw, "nsfw");
    EndpointConfig(std::optional<std::string> safe, std::optional<std::string> unsafe) : sfw(safe), nsfw(unsafe) {};
    EndpointConfig() {};
);


DECLARE_CONFIG(NyaConfig,
    CONFIG_VALUE(inPause, bool, "Show in pause", true);
    CONFIG_VALUE(inMenu, bool, "Show in menu", true);

    CONFIG_VALUE(API, std::string, "Selected API", "waifu.pics");

    CONFIG_VALUE(AutoNya, bool, "Auto Nya", false);
    CONFIG_VALUE(AutoNyaInGame, bool, "Auto Nya in game", false);
    CONFIG_VALUE(AutoNyaDelay, float, "Nya Delay", 7.0f);
    CONFIG_VALUE(FloatingScreenScale, float, "Scale", 1.0f);
    CONFIG_VALUE(ShowHandle, bool, "Show handle", true);
    CONFIG_VALUE(UseButton, int, "Nya controller button", 0);

    CONFIG_VALUE(NSFWUI, bool, "NSFWUI", false);
    CONFIG_VALUE(NSFWEnabled, bool, "NSFW Enabled", false);
    CONFIG_VALUE(SkipNSFWWarning, bool, "Skip NSFW warning", false);
    CONFIG_VALUE(RememberNSFW, bool, "Remember NSFW", false);

    // Android indexing settings
    CONFIG_VALUE(IndexSFW, bool, "Index SFW", true);
    CONFIG_VALUE(IndexNSFW, bool, "Index NSFW", false);

    CONFIG_VALUE(pausePosition, UnityEngine::Vector3, "Pause Position", UnityEngine::Vector3(-2.12f, 1.52f, 1.65f));
    CONFIG_VALUE(pauseRotation, UnityEngine::Vector3, "Pause Rotation", UnityEngine::Vector3(4.52f, 312.22f, 359.63f));

    CONFIG_VALUE(menuPosition, UnityEngine::Vector3, "Menu Position", UnityEngine::Vector3(0.0f, 4.0f, 4.0f));
    CONFIG_VALUE(menuRotation, UnityEngine::Vector3, "Menu Rotation", UnityEngine::Vector3(332.0f, 0.0f, 0.0f));

    CONFIG_VALUE(endpoints, StringKeyedMap<EndpointConfig>, "EndpointsConfig", {});
);