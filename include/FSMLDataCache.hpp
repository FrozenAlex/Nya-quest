#pragma once

#include <string>
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace FSML::DataCache {
    class Entry;
    /// @brief Registers an entry in the datacache.
    /// @param key the key to use in your FSML file if you want to use this data as a source
    /// @param value the entry to register
    void RegisterEntry(std::string key, const Entry* value);

    class Entry {
        public:
            Entry(std::string key) { FSML::DataCache::RegisterEntry(key, this); }
            virtual ArrayW<uint8_t> get_data() const = 0;
    };
}

/** define used by FSML to get data from your mod into a cache to be used by anything that requires access to data within a mod for things like images
 * Make sure it returns a valid ArrayW<uint8_t>
 * if you want to use the data registered this way in your FSML file, use it like this:
 * 
 * say my mod is Qosmetics, and uses the MOD_ID Qosmetics.
 * if I wanted to use a data key, it'd look like this in my FSML file: Qosmetics_sabericon
 * sabericon is what I typed into the FSML_DATACACHE macro as you can see below:
 *
 * FSML_DATACACHE(sabericon) {
 *     return IncludedAssets::sabericon_png;   
 * }
 * 
 * the keys are prepended with your MOD_ID to prevent key collisions with other mods
 */
#define FSML_DATACACHE(key)\
class FSMLDataCacheEntry_##key : public FSML::DataCache::Entry {\
    public:\
        FSMLDataCacheEntry_##key() : FSML::DataCache::Entry(MOD_ID "_" #key) {}\
        ArrayW<uint8_t> get_data() const override;\
};\
static const FSMLDataCacheEntry_##key FSMLDataCacheEntry_##key##_instance{};\
ArrayW<uint8_t> FSMLDataCacheEntry_##key::get_data() const