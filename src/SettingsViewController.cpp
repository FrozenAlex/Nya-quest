#include "SettingsViewController.hpp"


DEFINE_TYPE(Nya, SettingsViewController);

void Nya::SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation) {
        auto *container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform());

        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Floating in Pause Menu",
            getNyaConfig().inPause.GetValue(), [](bool value) {
            getNyaConfig().inPause.SetValue(value);
            if (
                Main::NyaFloatingUI != nullptr && 
                Main::NyaFloatingUI->UIScreen != nullptr
            ) {
                Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
            }
        });
        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Floating on Menu Screen",
            getNyaConfig().inMenu.GetValue(),
            [](bool value) {
                getNyaConfig().inMenu.SetValue(value);
                if (
                    Main::NyaFloatingUI != nullptr && 
                    Main::NyaFloatingUI->UIScreen != nullptr
                ) {
                    Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
                }
        });

        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Show handle",
            getNyaConfig().ShowHandle.GetValue(),
            [](bool value) {
                getNyaConfig().ShowHandle.SetValue(value);
                if (
                    Main::NyaFloatingUI != nullptr && 
                    Main::NyaFloatingUI->UIScreen != nullptr
                ) {
                    Main::NyaFloatingUI->UpdateHandleVisibility();
                }
        });

        auto slider = QuestUI::BeatSaberUI::CreateSliderSetting(container->get_transform(), "Floating Screen Scale", 0.1f, 
            getNyaConfig().FloatingScreenScale.GetValue(), 0.1f, 2.0f, [](float value) {
                getNyaConfig().FloatingScreenScale.SetValue(value);
                if (Main::NyaFloatingUI != nullptr) {
                    Main::NyaFloatingUI->UpdateScale();
                }
            }
            
        );

        QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "AutoNya",
            getNyaConfig().AutoNya.GetValue(),
            [](bool value) {
                getNyaConfig().AutoNya.SetValue(value);
                if (
                    value &&
                    Main::NyaFloatingUI != nullptr && 
                    Main::NyaFloatingUI->imageView != nullptr
                ) {
                    Main::NyaFloatingUI->imageView->OnEnable();
                }
        });

        QuestUI::BeatSaberUI::CreateSliderSetting(container->get_transform(), "Nya Delay", 0.5f, 
            getNyaConfig().AutoNyaDelay.GetValue(), 4.0f, 30.0f, [](float value) {
                getNyaConfig().AutoNyaDelay.SetValue(value);
            }
        );


        #ifdef NSFW
            if (getNyaConfig().NSFWUI.GetValue()) {
                QuestUI::BeatSaberUI::CreateToggle(container->get_transform(), "Remember NSFW",
                    getNyaConfig().RememberNSFW.GetValue(), [](bool value) {
                        getNyaConfig().RememberNSFW.SetValue(value);
                    }
                );
            }
        #endif

        // Buttons for settings
        // TODO: Make it work with floating ui off
        UnityEngine::UI::Button* faceHeadset = QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16("Face headset"), "PracticeButton",
        [this]() {
            if (Main::NyaFloatingUI != nullptr) {
                Main::NyaFloatingUI->hoverClickHelper->LookAtCamera();
            }
        });
        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16("Set upright"), "PracticeButton",
        [this]() {
            if (Main::NyaFloatingUI != nullptr) {
                Main::NyaFloatingUI->hoverClickHelper->SetUpRight();
            }
        });
        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16("Default position"), "PracticeButton",
        [this]() {
            if (Main::NyaFloatingUI != nullptr) {
                Main::NyaFloatingUI->SetDefaultPos();
            }
        });

        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16("Reset all positions"), "PracticeButton",
        [this]() {
            EndpointConfig::ResetPositions();
            if (
                Main::NyaFloatingUI != nullptr && 
                Main::NyaFloatingUI->UIScreen != nullptr
            ) {
                Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
            }
        });

        
    }
}