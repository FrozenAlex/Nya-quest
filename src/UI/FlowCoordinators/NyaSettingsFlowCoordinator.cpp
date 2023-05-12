#include "UI/FlowCoordinators/NyaSettingsFlowCoordinator.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

DEFINE_TYPE(Nya::UI::FlowCoordinators, NyaSettingsFlowCoordinator);


void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::Awake(){
    if (!SettingsViewController || !SettingsViewController->m_CachedPtr.m_value) {
        SettingsViewController = BeatSaberUI::CreateViewController<ViewControllers::SettingsViewController*>();
    }
}

void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::DidActivate(bool firstActivation, bool addedToHeirarchy, bool screenSystemEnabling) {
    if (!firstActivation) return;

    SetTitle(il2cpp_utils::newcsstr("Nya Settings"), HMUI::ViewController::AnimationType::In);
    showBackButton = true;
    ProvideInitialViewControllers(SettingsViewController, nullptr, nullptr, nullptr, nullptr);
}
void Nya::UI::FlowCoordinators::NyaSettingsFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
    if (this->parentFlowCoordinator && this->parentFlowCoordinator->m_CachedPtr.m_value) {
        this->parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }
}