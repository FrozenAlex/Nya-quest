#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include <string>
#include <iostream>
#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"

#define NYA_PATH_FORMAT "/sdcard/ModData/%s/Mods/Nya/"

namespace NyaGlobals {
    static std::string nyaPath = string_format(NYA_PATH_FORMAT, Modloader::getApplicationId().c_str());;
    static std::string imagesPath = nyaPath + "Images/";
    static std::string imagesPathSFW = nyaPath + "Images/sfw/";
    static std::string imagesPathNSFW = nyaPath + "Images/nsfw/";
    static std::string tempPath = nyaPath + "Temp/";
}



DECLARE_CONFIG(NyaConfig,
    CONFIG_VALUE(inPause, bool, "Show in pause", true);
    CONFIG_VALUE(inMenu, bool, "Show in menu", true);

    CONFIG_VALUE(API, std::string, "Selected API", "waifu.pics");

    CONFIG_VALUE(AutoNya, bool, "Auto Nya", false);
    CONFIG_VALUE(AutoNyaInGame, bool, "Auto Nya in game", false);
    CONFIG_VALUE(AutoNyaDelay, float, "Nya Delay", 7.0f);
    CONFIG_VALUE(FloatingScreenScale, float, "Scale", 1.0f);
    CONFIG_VALUE(ShowHandle, bool, "Show handle", true);

    CONFIG_VALUE(NSFWUI, bool, "NSFWUI", false);
    CONFIG_VALUE(NSFWEnabled, bool, "NSFW Enabled", false);
    CONFIG_VALUE(SkipNSFWWarning, bool, "Skip NSFW warning", false);
    CONFIG_VALUE(RememberNSFW, bool, "Remember NSFW", false);    

    CONFIG_VALUE(pausePositionX, float, "Pause PositionX", -2.12f);
    CONFIG_VALUE(pausePositionY, float, "Pause PositionY", 1.52f);
    CONFIG_VALUE(pausePositionZ, float, "Pause PositionZ", 1.65f);

    CONFIG_VALUE(pauseRotationX, float, "Pause RotationX", 4.52f);
    CONFIG_VALUE(pauseRotationY, float, "Pause RotationY", 312.22f);
    CONFIG_VALUE(pauseRotationZ, float, "Pause RotationZ", 359.63f);

    CONFIG_VALUE(menuPositionX, float, "Menu PositionX", 0.0f);
    CONFIG_VALUE(menuPositionY, float, "Menu PositionY", 3.75f);
    CONFIG_VALUE(menuPositionZ, float, "Menu PositionZ", 4.0f);

    CONFIG_VALUE(menuRotationX, float, "Menu RotationX", 332.0f);
    CONFIG_VALUE(menuRotationY, float, "Menu RotationY", 0.0f);
    CONFIG_VALUE(menuRotationZ, float, "Menu RotationZ", 0.0f);


    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(inPause);
        CONFIG_INIT_VALUE(inMenu);

        CONFIG_INIT_VALUE(API);
        CONFIG_INIT_VALUE(AutoNya);
        CONFIG_INIT_VALUE(AutoNyaInGame);
        CONFIG_INIT_VALUE(AutoNyaDelay);
        CONFIG_INIT_VALUE(FloatingScreenScale);
        CONFIG_INIT_VALUE(ShowHandle);

        CONFIG_INIT_VALUE(NSFWUI);
        CONFIG_INIT_VALUE(NSFWEnabled);
        CONFIG_INIT_VALUE(SkipNSFWWarning);
        CONFIG_INIT_VALUE(RememberNSFW);

        // Positions because vectors crash
        CONFIG_INIT_VALUE(pausePositionX);
        CONFIG_INIT_VALUE(pausePositionY);
        CONFIG_INIT_VALUE(pausePositionZ);

        CONFIG_INIT_VALUE(pauseRotationX);
        CONFIG_INIT_VALUE(pauseRotationY);
        CONFIG_INIT_VALUE(pauseRotationZ);


        CONFIG_INIT_VALUE(menuPositionX);
        CONFIG_INIT_VALUE(menuPositionY);
        CONFIG_INIT_VALUE(menuPositionZ);

        CONFIG_INIT_VALUE(menuRotationX);
        CONFIG_INIT_VALUE(menuRotationY);
        CONFIG_INIT_VALUE(menuRotationZ);
    )
)