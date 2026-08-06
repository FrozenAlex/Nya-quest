#pragma once

#include <string_view>
#include "beatsaber-hook/shared/types.hpp"
#include "beatsaber-hook/shared/arrayw.hpp"

struct IncludedAsset {

    IncludedAsset(uint8_t* start, uint8_t* end) : array(reinterpret_cast<Array<uint8_t>*>(start)) {
        array->klass = nullptr;
        array->monitor = nullptr;
        array->bounds = nullptr;
        array->max_length = end - start - 33;
        *(end - 1)= '\0';
    }
    
    operator ArrayW<uint8_t>() const {
        init();
        return array;
    }

    operator std::string_view() const {
        return { reinterpret_cast<char*>(array->_values), array->max_length };
    }
    
    operator std::span<uint8_t>() const {
        return { array->_values, array->max_length };
    }

    void init() const {
        if(!array->klass)
            array->klass = i2c::class_of<Array<uint8_t>*>();
    }

    private:
        Array<uint8_t>* array;

};

#define DECLARE_FILE(name)                         \
    extern "C" uint8_t _binary_##name##_start[];  \
    extern "C" uint8_t _binary_##name##_end[];    \
    const IncludedAsset name { _binary_##name##_start, _binary_##name##_end};

namespace IncludedAssets {

	DECLARE_FILE(Chocola_Angry_png)
	DECLARE_FILE(Chocola_Bashful_png)
	DECLARE_FILE(Chocola_Dead_png)
	DECLARE_FILE(Chocola_Happy_png)
	DECLARE_FILE(Chocola_Howdidyoudothat_png)
	DECLARE_FILE(Chocola_Laugh_png)
	DECLARE_FILE(Chocola_Mini_Sitting_png)
	DECLARE_FILE(Chocola_Question_Mark_png)
	DECLARE_FILE(Chocola_Sobbing_png)
	DECLARE_FILE(Chocola_Spooked_png)
	DECLARE_FILE(Chocola_Surprised_png)
	DECLARE_FILE(Chocovani_Greeting_png)
	DECLARE_FILE(Chocovani_Laptop_png)
	DECLARE_FILE(Vanilla_Flushed_png)
	DECLARE_FILE(Vanilla_Horny_Pastry_Puffer_png)
	DECLARE_FILE(Vanilla_Mini_Sitting_png)
	DECLARE_FILE(Vanilla_Stare_png)
	DECLARE_FILE(placeholder_png)

}
