#include "UI/ViewControllers/ConfigUtilsTestViewController.hpp"

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "ConfigUtilsTestConfig.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"

DEFINE_TYPE(Nya::UI::ViewControllers, ConfigUtilsTestViewController);

namespace {
    const std::vector<std::string> incrementEnumOptions = {
        "First",
        "Second",
        "Third",
        "Fourth"
    };

    std::array<std::string_view, 4> dropdownOptions = {
        "First",
        "Second",
        "Third",
        "Fourth"
    };
}

void Nya::UI::ViewControllers::ConfigUtilsTestViewController::DidActivate(
    bool firstActivation,
    bool addedToHierarchy,
    bool screenSystemEnabling
) {
    if (!firstActivation) {
        return;
    }

    auto* container = BSML::Lite::CreateScrollableSettingsContainer(get_transform());
    auto& config = getConfigUtilsTestConfig();

    AddConfigValueToggle(container->get_transform(), config.toggle);
    AddConfigValueModifierButton(container->get_transform(), config.modifierButton);

    AddConfigValueIncrementInt(container->get_transform(), config.incrementInt, 1, -10, 10);
    AddConfigValueIncrementFloat(container->get_transform(), config.incrementFloat, 2, 0.25f, -10.0f, 10.0f);
    AddConfigValueIncrementDouble(container->get_transform(), config.incrementDouble, 3, 0.125, -10.0, 10.0);
    AddConfigValueIncrementEnum(container->get_transform(), config.incrementEnum, incrementEnumOptions);

    AddConfigValueSlider(container->get_transform(), config.slider, 2, 0.05f, 0.0f, 1.0f);
    AddConfigValueSliderIncrement(container->get_transform(), config.sliderIncrement, 0.1f, 0.0f, 1.0f);

    AddConfigValueInputString(container->get_transform(), config.inputString);
    auto dropdownOptionSpan = std::span<std::string_view>(dropdownOptions);
    AddConfigValueDropdownString(container->get_transform(), config.dropdownString, dropdownOptionSpan);
    AddConfigValueDropdownEnum(container->get_transform(), config.dropdownEnum, dropdownOptionSpan);

    AddConfigValueColorPicker(container->get_transform(), config.color);
    AddConfigValueIncrementVector2(container->get_transform(), config.vector2, 2, 0.25);
    AddConfigValueIncrementVector3(container->get_transform(), config.vector3, 2, 0.25);
    AddConfigValueIncrementVector4(container->get_transform(), config.vector4, 2, 0.25);
}
