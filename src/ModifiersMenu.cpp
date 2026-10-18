#define RAPIDJSON_HAS_STDSTRING 1
#include "ModifiersMenu.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "HMUI/ModalView.hpp"
#include "ImageView.hpp"
#include <dirent.h>
#include "assets.hpp"
#include "logging.hpp"
#include "Utils/Utils.hpp"

#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"

using namespace UnityEngine;
using namespace Nya;

DEFINE_TYPE(Nya, ModifiersMenu);

namespace Nya {
    // Disable
    void ModifiersMenu::OnDisable() {

    }

    // Enable (runs when the component appears)
    void ModifiersMenu::OnEnable() {

    }

    void ModifiersMenu::ctor() {
        this->initialized = false;
    }

    void ModifiersMenu::DidActivate(bool firstActivation)
    {

        if(firstActivation)
        {
            DEBUG("Creator runs");

            auto vert = BSML::Lite::CreateVerticalLayoutGroup(get_transform());
            vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

            NYA = BSML::Lite::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
            NYA->set_preserveAspect(true);
            // Set blank sprite to avoid white screens
            NYA->set_sprite(BSML::Lite::ArrayToSprite(IncludedAssets::placeholder_png));
            auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
            DEBUG("Adds component");
            this->imageView = NYA->get_gameObject()->AddComponent<Nya::ImageView*>();
            ele->set_preferredHeight(50);
            ele->set_preferredWidth(50);

            auto horz = BSML::Lite::CreateHorizontalLayoutGroup(vert->get_transform());
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->set_spacing(10);

            this->nyaButton = BSML::Lite::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
                                                         [this]() {
                                                             this->imageView->GetImage(nullptr);
                                                         });

            this->settingsMenu = NYA->get_gameObject()->AddComponent<Nya::SettingsMenu*>();
            // Settings button
            this->settingsButton = BSML::Lite::CreateUIButton(horz->get_transform(), "Settings", "PracticeButton",
                                                              [this]() {
                                                                  this->settingsMenu->Show();
                                                              });
            // Set button sizes
            UnityEngine::Vector2 sizeDelta = {19, 8};
            Nya::Utils::SetButtonSize(this->nyaButton, sizeDelta);
            Nya::Utils::SetButtonSize(this->settingsButton, sizeDelta);

            // Sub to events of the image view
            this->imageView->imageLoadingChange += {&ModifiersMenu::OnIsLoadingChange, this};

            // Get image when the component appears the first time
            this->imageView->GetImage(nullptr);
            this->initialized = true;
        }

    }

    void ModifiersMenu::OnIsLoadingChange (bool isLoading) {
        BSML::MainThreadScheduler::Schedule([this, isLoading]
        {
            if (this->nyaButton)
                this->nyaButton->set_interactable(!isLoading);
        });
    }

}

