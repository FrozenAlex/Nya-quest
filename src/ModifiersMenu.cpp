#define RAPIDJSON_HAS_STDSTRING 1
#include "ModifiersMenu.hpp"
#include "Utils/FileUtils.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "System/IO/File.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "HMUI/ModalView.hpp"
#include "System/Action.hpp"
#include "ImageView.hpp"
#include "EndpointConfig.hpp"
#include <dirent.h>
#include <string>
#include <iostream>
#include "assets.hpp"
using namespace UnityEngine;
using namespace Nya;

DEFINE_TYPE(Nya, ModifiersMenu);

namespace Nya {
    // Disable
    void ModifiersMenu::OnDisable() {
    }

    // Enable (runs when the component appears)
    void ModifiersMenu::OnEnable() {
        if (!this->initialized) {
            // Get image when the component appears the first time
            this->imageView->GetImage(nullptr);
            this->initialized = true;
        }
    }

    void ModifiersMenu::ctor() {
        this->initialized = false;
        DEBUG("Creator runs");

        auto vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        // Set blank sprite to avoid white screens
        NYA->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::placeholder_png));
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        DEBUG("Adds component");
        this->imageView = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);

        this->nyaButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), "Nya", "PlayButton",
            [this]() {
                this->imageView->GetImage(nullptr);
            });

        this->settingsMenu = NYA->get_gameObject()->AddComponent<Nya::SettingsMenu*>();
        // Settings button
        this->settingsButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Settings"), "PracticeButton",
        [this]() {
            this->settingsMenu->Show();
        });

        
    }

    void ModifiersMenu::DidActivate(bool firstActivation)
    {
        if(firstActivation)
        {
            // Sub to events of the image view
            this->imageView->imageLoadingChange += {&ModifiersMenu::OnIsLoadingChange, this};
        }
    }

    void ModifiersMenu::OnIsLoadingChange (bool isLoading) {
        QuestUI::MainThreadScheduler::Schedule([this, isLoading]
        {
            if (this->nyaButton && this->nyaButton->m_CachedPtr.m_value)
                this->nyaButton->set_interactable(!isLoading);
        });
    }

    void ModifiersMenu::dtor(){
    }
}

