#include "UI/ViewControllers/SettingsViewController.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "UnityEngine/Resources.hpp"
std::vector<std::string> buttonOptions = {
    "None",
    "A",
    "B",
    "X",
    "Y"
};


DEFINE_TYPE(Nya, SettingsViewController);

void Nya::SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if (firstActivation) {

        this->nsfwModal = get_gameObject()->AddComponent<Nya::NSFWConsent*>();

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

        // Create StringWrapper Vector with Content of buttonOptions
        // We cant create it directly, as StringW is not designed for static use
        std::vector<StringW> buttonOptionsWrapper = {};
        for (auto option : buttonOptions) {
            buttonOptionsWrapper.emplace_back(option);
        }

        // Create actual Dropdown
        QuestUI::BeatSaberUI::CreateDropdown(container->get_transform(), "Nya on controller button", buttonOptionsWrapper[getNyaConfig().UseButton.GetValue()], buttonOptionsWrapper, [](auto value) {

            // Find Index of selected Element
            int index = std::find(buttonOptions.begin(), buttonOptions.end(), value) - buttonOptions.begin();

            // And Save it
            getNyaConfig().UseButton.SetValue(index); 
        });


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

        

        #ifdef NSFW
            QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16(getNyaConfig().NSFWUI.GetValue()?"Enable NSFW again":"Enable NSFW"), "PracticeButton",
            [this]() {
                QuestUI::MainThreadScheduler::Schedule([this]{
                    this->nsfwModal->Show();
                });
            });
            if (getNyaConfig().NSFWUI.GetValue()) {
              
                QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), to_utf16("Disable NSFW"), "PracticeButton",
                [this]() {
                    QuestUI::MainThreadScheduler::Schedule([this]{
                        getNyaConfig().NSFWUI.SetValue(false);
                        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuTransitionsHelper*>()[0]->RestartGame(nullptr);
                    });
                });
            }
        #endif
        
    }
}