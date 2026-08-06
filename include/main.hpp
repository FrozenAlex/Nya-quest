#pragma once
// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "scotland2/shared/loader.hpp"

#include "NyaFloatingUI.hpp"

inline modloader::ModInfo modInfo = {MOD_ID, VERSION, GIT_COMMIT}; // Stores the ID and version of our mod, and is sent to the modloader upon startup

namespace Nya {
    class Main {
        public:
            static safe_ptr<Nya::NyaFloatingUI*> NyaFloatingUI;
    };

    // Creates/removes .nomedia based on config
    void ApplyIndexingRules();
    // Cleans temp folder
    void CleanTempFolder();
}