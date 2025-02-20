#pragma once

#include "custom-types/shared/macros.hpp"
#include "EndpointConfigUtils.hpp"
#include "UI/Modals/NSFWConsent.hpp"

DECLARE_CLASS_CODEGEN(Nya::UI::ViewControllers, SettingsViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(Nya::UI::Modals::NSFWConsent*, nsfwModal);
};