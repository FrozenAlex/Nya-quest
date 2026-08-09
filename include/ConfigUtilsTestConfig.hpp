#pragma once

#include <string>

#include "config-utils/shared/config-utils.hpp"
#include "UnityConfigTypeReflectors.hpp"

DECLARE_CONFIG(ConfigUtilsTestConfig) {
    CONFIG_VALUE(toggle, bool, "Toggle", true, "AddConfigValueToggle");
    CONFIG_VALUE(modifierButton, bool, "Modifier Button", false, "AddConfigValueModifierButton");

    CONFIG_VALUE(incrementInt, int, "Integer Increment", 5, "AddConfigValueIncrementInt");
    CONFIG_VALUE(incrementFloat, float, "Float Increment", 1.25f, "AddConfigValueIncrementFloat");
    CONFIG_VALUE(incrementDouble, double, "Double Increment", 2.5, "AddConfigValueIncrementDouble");
    CONFIG_VALUE(incrementEnum, int, "Enum Increment", 1, "AddConfigValueIncrementEnum");

    CONFIG_VALUE(slider, float, "Slider", 0.5f, "AddConfigValueSlider");
    CONFIG_VALUE(sliderIncrement, float, "Slider With Buttons", 0.5f, "AddConfigValueSliderIncrement");

    CONFIG_VALUE(inputString, std::string, "String Input", "Config Utils", "AddConfigValueInputString");
    CONFIG_VALUE(dropdownString, std::string, "String Dropdown", "Second", "AddConfigValueDropdownString");
    CONFIG_VALUE(dropdownEnum, int, "Enum Dropdown", 2, "AddConfigValueDropdownEnum");

    CONFIG_VALUE(color, UnityEngine::Color, "Color Picker", UnityEngine::Color(0.2f, 0.7f, 1.0f, 1.0f), "AddConfigValueColorPicker");
    CONFIG_VALUE(vector2, UnityEngine::Vector2, "Vector2", UnityEngine::Vector2(1.0f, 2.0f), "AddConfigValueIncrementVector2");
    CONFIG_VALUE(vector3, UnityEngine::Vector3, "Vector3", UnityEngine::Vector3(1.0f, 2.0f, 3.0f), "AddConfigValueIncrementVector3");
    CONFIG_VALUE(vector4, UnityEngine::Vector4, "Vector4", UnityEngine::Vector4(1.0f, 2.0f, 3.0f, 4.0f), "AddConfigValueIncrementVector4");
};
