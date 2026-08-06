#pragma once

#include "custom-types/shared/macros.hpp"
#include "UI/Modals/NSFWConsent.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(Nya::UI::ViewControllers, SettingsViewController, HMUI::ViewController) {
    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(Nya::UI::Modals::NSFWConsent*, nsfwModal);
};