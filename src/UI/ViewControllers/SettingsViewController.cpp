#include "UI/ViewControllers/SettingsViewController.hpp"


#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Settings.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "EndpointConfigUtils.hpp"

std::vector<std::string_view> buttonOptions = {
    "None",
    "A",
    "B",
    "X",
    "Y"
};


DEFINE_TYPE(Nya::UI::ViewControllers, SettingsViewController);

using namespace BSML;

void Nya::UI::ViewControllers::SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    std::span<std::string_view> buttonOptionsValues(buttonOptions);
    if (firstActivation) {
        this->nsfwModal = get_gameObject()->AddComponent<Nya::UI::Modals::NSFWConsent*>();

        auto *container = BSML::Lite::CreateScrollableSettingsContainer(get_transform());

        BSML::Lite::CreateToggle(container->get_transform(), "Floating in Pause Menu",
            getNyaConfig().inPause.GetValue(), [](bool value) {
            getNyaConfig().inPause.SetValue(value);
            if (
                Main::NyaFloatingUI && 
                Main::NyaFloatingUI->floatingScreen != nullptr
            ) {
                Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
            }
        });
        BSML::Lite::CreateToggle(container->get_transform(), "Floating on Menu Screen",
            getNyaConfig().inMenu.GetValue(),
            [](bool value) {
                getNyaConfig().inMenu.SetValue(value);
                if (
                    Main::NyaFloatingUI && 
                    Main::NyaFloatingUI->floatingScreen != nullptr
                ) {
                    Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
                }
        });

        BSML::Lite::CreateToggle(container->get_transform(), "Show handle",
            getNyaConfig().ShowHandle.GetValue(),
            [](bool value) {
                getNyaConfig().ShowHandle.SetValue(value);
                if (
                    Main::NyaFloatingUI && 
                    Main::NyaFloatingUI->floatingScreen != nullptr
                ) {
                    Main::NyaFloatingUI->UpdateHandleVisibility();
                }
        });

        auto slider = BSML::Lite::CreateSliderSetting(container->get_transform(), "Floating Screen Scale", 0.1f,
            getNyaConfig().FloatingScreenScale.GetValue(), 0.1f, 2.0f, [](float value) {
                getNyaConfig().FloatingScreenScale.SetValue(value);
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->UpdateScale();
                }
            }

        );

        BSML::Lite::CreateToggle(container->get_transform(), "AutoNya",
            getNyaConfig().AutoNya.GetValue(),
            [](bool value) {
                getNyaConfig().AutoNya.SetValue(value);
                if (
                    value &&
                    Main::NyaFloatingUI &&
                    Main::NyaFloatingUI->imageView != nullptr
                ) {
                    Main::NyaFloatingUI->imageView->OnEnable();
                }
        });

        BSML::Lite::CreateSliderSetting(container->get_transform(), "Nya Delay", 0.5f,
            getNyaConfig().AutoNyaDelay.GetValue(), 4.0f, 30.0f, [](float value) {
                getNyaConfig().AutoNyaDelay.SetValue(value);
            }
        );





        // Create actual Dropdown
        BSML::Lite::CreateDropdown(container->get_transform(), "Nya on controller button", buttonOptionsValues[getNyaConfig().UseButton.GetValue()], buttonOptionsValues, [](auto value) {

            // Find Index of selected Element
            int index = std::find(buttonOptions.begin(), buttonOptions.end(), value) - buttonOptions.begin();

            // And Save it
            getNyaConfig().UseButton.SetValue(index);
        });

        BSML::Lite::CreateToggle(container->get_transform(), "Index saved images",
            getNyaConfig().IndexSFW.GetValue(), [](bool value) {
                getNyaConfig().IndexSFW.SetValue(value);
                Nya::ApplyIndexingRules();
            }
        );

        #ifdef NSFW
            if (getNyaConfig().NSFWUI.GetValue()) {
                BSML::Lite::CreateToggle(container->get_transform(), "Index saved NSFW images",
                    getNyaConfig().IndexNSFW.GetValue(), [](bool value) {
                        getNyaConfig().IndexNSFW.SetValue(value);
                        Nya::ApplyIndexingRules();
                    }
                );
            }
        #endif

        #ifdef NSFW
            if (getNyaConfig().NSFWUI.GetValue()) {
                BSML::Lite::CreateToggle(container->get_transform(), "Remember NSFW",
                    getNyaConfig().RememberNSFW.GetValue(), [](bool value) {
                        getNyaConfig().RememberNSFW.SetValue(value);
                    }
                );
            }
        #endif

        {
            auto* hor = BSML::Lite::CreateHorizontalLayoutGroup(container->get_transform());
            // Buttons for settings
            // TODO: Make it work with floating ui off
            UnityEngine::UI::Button* faceHeadset = BSML::Lite::CreateUIButton(hor->get_transform(), "Face headset", "PracticeButton",
            [this]() {
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->LookAtCamera();
                }
            });
            BSML::Lite::CreateUIButton(hor->get_transform(), "Set upright", "PracticeButton",
            [this]() {
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->SetUpRight();
                }
            });
            BSML::Lite::CreateUIButton(hor->get_transform(), "Default position", "PracticeButton",
            [this]() {
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->SetDefaultPos();
                }
            });
        }

        BSML::Lite::CreateUIButton(container->get_transform(), "Reset all positions", "PracticeButton",
        [this]() {
            EndpointConfigUtils::ResetPositions();
            if (
                Main::NyaFloatingUI &&
                Main::NyaFloatingUI->floatingScreen != nullptr
            ) {
                Main::NyaFloatingUI->onSceneChange( Main::NyaFloatingUI->currentScene, true);
            }
        });



        #ifdef NSFW
            BSML::Lite::CreateUIButton(container->get_transform(), (getNyaConfig().NSFWUI.GetValue()?"Enable NSFW again":"Enable NSFW"), "PracticeButton",
            [this]() {
                BSML::MainThreadScheduler::Schedule([this]{
                    this->nsfwModal->Show();
                });
            });
            if (getNyaConfig().NSFWUI.GetValue()) {

                BSML::Lite::CreateUIButton(container->get_transform(), "Disable NSFW", "PracticeButton",
                [this]() {
                    BSML::MainThreadScheduler::Schedule([this]{
                        getNyaConfig().NSFWUI.SetValue(false);
                        // If we are turning it off completely, we should also turn off indexing of NSFW images
                        getNyaConfig().IndexNSFW.SetValue(false);
                        Nya::ApplyIndexingRules();
                        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuTransitionsHelper*>()[0]->RestartGame(nullptr);
                    });
                });
            }
        #endif
        
    }
}
