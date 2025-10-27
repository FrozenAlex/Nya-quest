#pragma once

#include "custom-types/shared/macros.hpp"
#include "UI/Modals/NSFWConsent.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(Nya::UI::ViewControllers, SettingsViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(Nya::UI::Modals::NSFWConsent*, nsfwModal);
);