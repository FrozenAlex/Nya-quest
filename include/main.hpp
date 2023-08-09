#pragma once
#include "paper/shared/logger.hpp"
// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "EndpointConfig.hpp"

inline ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

namespace Nya {
    class Main {
        public:
            static bool configValid;
            static void loadConfig();
            static Nya::NyaFloatingUI* NyaFloatingUI;
    };

    Paper::ConstLoggerContext<4UL> getLogger();

    // Define these functions here so that we can easily read configuration and log information from other files
    Configuration& getConfig();  
    // Old version of the logger
    Logger& getLoggerOld();
    // Creates/removes .nomedia based on config
    void ApplyIndexingRules();
    // Cleans temp folder
    void CleanTempFolder();
}


#define INFO(...) Nya::getLogger().fmtLog<Paper::LogLevel::INF>(__VA_ARGS__)
#define ERROR(...) Nya::getLogger().fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define CRITICAL(...) Nya::getLogger().fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define DEBUG(...) Nya::getLogger().fmtLog<Paper::LogLevel::DBG>(__VA_ARGS__)
#define WARNING(...) Nya::getLogger().fmtLog<Paper::LogLevel::WRN>(__VA_ARGS__)
