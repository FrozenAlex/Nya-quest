#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "EndpointConfig.hpp"

#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "Utils/FileUtils.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "assets.hpp"
using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Nya, NyaFloatingUI);

namespace Nya {
    // Function gets url for the current selected category

    void NyaFloatingUI::ctor()
    {
        instance = this;
        screenhandle = nullptr;
        UIScreen = nullptr;
        UINoGlow = nullptr;
        hoverClickHelper = nullptr;
        imageView = nullptr;
        INFO("Created NyaFloatingUI instance");
    }

    void NyaFloatingUI::initScreen(){
        if (this->isInitialized){
            return;
        }

        UIScreen = QuestUI::BeatSaberUI::CreateFloatingScreen({40.0f, 32.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}, 0.0f, true, true, 0);
        UIScreen->set_active(false);
        UIScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(31);
        UnityEngine::GameObject::DontDestroyOnLoad(UIScreen);

        // Handle creation
        screenhandle = UIScreen->GetComponent<QuestUI::FloatingScreen*>()->handle;
        UIScreen->GetComponent<QuestUI::FloatingScreen*>()->bgGo->GetComponentInChildren<QuestUI::Backgroundable*>()->ApplyBackgroundWithAlpha("round-rect-panel", 0.0f);
        screenhandle->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -23.0f, 0.0f));
        screenhandle->get_transform()->set_localScale(UnityEngine::Vector3(5.3f, 3.3f, 5.3f));

        QuestUI::FloatingScreen* thing = UIScreen->GetComponent<QuestUI::FloatingScreen*>();

        auto* vert = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(UIScreen->get_transform());
    
        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = QuestUI::BeatSaberUI::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        // Set blank sprite to avoid white screens
        NYA->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::placeholder_png));
        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        imageView = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(50);
        ele->set_preferredWidth(50);

        auto horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);


        // Get new picture
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

        UINoGlow = QuestUI::ArrayUtil::First(UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>(), [](UnityEngine::Material* x) { return x->get_name() == "UINoGlow"; });
        auto* screenthingidk = thing->get_gameObject()->AddComponent<HMUI::Screen*>();
        auto* normalpointer = Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>().get(0);
        hoverClickHelper = Nya::addHoverClickHelper(normalpointer, screenhandle, thing);

        this->isInitialized = true;

        this->UpdateScale();
        this->UpdateHandleVisibility();

        // // Sub to events
        this->imageView->imageLoadingChange += {&NyaFloatingUI::OnIsLoadingChange, this};
    }
    
    void NyaFloatingUI::SetDefaultPos () {
        // Do nothing if hover click helper is not present
        
        // If screen does not exist, initialize the first time to reset stuff
        if (!this->UIScreen || !this->UIScreen->m_CachedPtr.m_value) {
            DEBUG("LOL IT ACTUALLY HAPPENS");
            // this->isInitialized = false;
            this->initScreen();
        }

        if ( !this->hoverClickHelper || !this->hoverClickHelper->m_CachedPtr.m_value) {
            DEBUG("SetDefaultPos canceled");
            return;
        }

        if (this->currentScene == Nya::FloatingUIScene::Pause) {
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().pausePositionX.GetDefaultValue(), 
                    getNyaConfig().pausePositionY.GetDefaultValue(),
                    getNyaConfig().pausePositionZ.GetDefaultValue()
                ),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().pauseRotationX.GetDefaultValue(), 
                    getNyaConfig().pauseRotationY.GetDefaultValue(), 
                    getNyaConfig().pauseRotationZ.GetDefaultValue()
                )
            );

            // Save target 
            this->updateCoordinates(
                UnityEngine::Vector3(
                    getNyaConfig().pausePositionX.GetDefaultValue(), 
                    getNyaConfig().pausePositionY.GetDefaultValue(),
                    getNyaConfig().pausePositionZ.GetDefaultValue()
                ),
                UnityEngine::Vector3(
                    getNyaConfig().pauseRotationX.GetDefaultValue(), 
                    getNyaConfig().pauseRotationY.GetDefaultValue(), 
                    getNyaConfig().pauseRotationZ.GetDefaultValue()
                )
            );
        }

        if (this->currentScene == Nya::FloatingUIScene::MainMenu) {
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetDefaultValue(), 
                    getNyaConfig().menuPositionY.GetDefaultValue(),
                    getNyaConfig().menuPositionZ.GetDefaultValue()
                ),
                UnityEngine::Quaternion::Euler(
                    getNyaConfig().menuRotationX.GetDefaultValue(), 
                    getNyaConfig().menuRotationY.GetDefaultValue(), 
                    getNyaConfig().menuRotationZ.GetDefaultValue()
                )
            );

            this->updateCoordinates(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetDefaultValue(), 
                    getNyaConfig().menuPositionY.GetDefaultValue(),
                    getNyaConfig().menuPositionZ.GetDefaultValue()
                ),
                UnityEngine::Vector3(
                    getNyaConfig().menuRotationX.GetDefaultValue(), 
                    getNyaConfig().menuRotationY.GetDefaultValue(), 
                    getNyaConfig().menuRotationZ.GetDefaultValue()
                )
            );
        }
    }

    void NyaFloatingUI::onSceneChange(Nya::FloatingUIScene scene, bool reinitialize) {
        INFO("Switched from {} to {} ", this->currentScene, scene);
        
        // Do nothing if the scene did not change unless reinitialize is active
        if (!reinitialize && this->currentScene == scene) {
            return;
        }
        
        this->currentScene = scene;

        // Disable everything if config does not need it and return
        if (
            (
                scene == Nya::FloatingUIScene::Disabled
            ) || (
                scene == Nya::FloatingUIScene::Pause && !getNyaConfig().inPause.GetValue()
            ) || (
                scene == Nya::FloatingUIScene::MainMenu && !getNyaConfig().inMenu.GetValue()
            )) {
                INFO("DISABLING THE SCREEN");
                if (
                    this->UIScreen &&
                    this->UIScreen->m_CachedPtr.m_value
                ) UIScreen->set_active(false);
                if (
                    this->hoverClickHelper &&
                    this->hoverClickHelper->m_CachedPtr.m_value
                ) this->hoverClickHelper->set_enabled(false);
                return;
        };
        
        // If screen does not exist, initialize the first time
        if (!this->UIScreen || !this->UIScreen->m_CachedPtr.m_value) {
            this->initScreen();
            // If the screen is created, get the first image
            this->imageView->GetImage(nullptr);
        }

        if (scene == Nya::FloatingUIScene::Pause) {
            INFO("Showing pause");
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().pausePositionX.GetValue(), 
                    getNyaConfig().pausePositionY.GetValue(),
                    getNyaConfig().pausePositionZ.GetValue()
                ),
                Quaternion::Euler(
                    getNyaConfig().pauseRotationX.GetValue(), 
                    getNyaConfig().pauseRotationY.GetValue(), 
                    getNyaConfig().pauseRotationZ.GetValue()
                ),
                false
            );

            this->hoverClickHelper->UpdatePointer();
        }

        if (scene == Nya::FloatingUIScene::MainMenu) {
            INFO("Showing main menu");
            this->hoverClickHelper->SetPosition(
                UnityEngine::Vector3(
                    getNyaConfig().menuPositionX.GetValue(), 
                    getNyaConfig().menuPositionY.GetValue(),
                    getNyaConfig().menuPositionZ.GetValue()
                ),
                Quaternion::Euler(
                    getNyaConfig().menuRotationX.GetValue(), 
                    getNyaConfig().menuRotationY.GetValue(), 
                    getNyaConfig().menuRotationZ.GetValue()
                ),
                false
            );
          
            this->hoverClickHelper->UpdatePointer();
        }

        INFO("SETTING SCREEN TO ACTIVE");
        // Set UIScreen active and reset click helper state
        UIScreen->set_active(true);
        if (this->hoverClickHelper && this->hoverClickHelper->m_CachedPtr.m_value) {
            this->hoverClickHelper->set_enabled(true);
            INFO("RESETTING CLICK HELPER");
            hoverClickHelper->resetBools();
            INFO("Reset click helper");
        }
    }

    // Saves the coordinates to a config
    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        auto position = transform->get_position();
        auto rotation = transform->get_rotation().get_eulerAngles();

        this->updateCoordinates(position, rotation);
    }

    void NyaFloatingUI::updateCoordinates(UnityEngine::Vector3 position, UnityEngine::Vector3 eulerRotation) {

        // INFO("Position: %.02f, %.02f, %.02f", position.x, position.y, position.z);
        // INFO("Rotation: %.02f, %.02f, %.02f", rotation.x, rotation.y, rotation.z);
        if (this->currentScene == Nya::FloatingUIScene::Pause){
            INFO("Saved to Pause");
            getNyaConfig().pausePositionX.SetValue(position.x);
            getNyaConfig().pausePositionY.SetValue(position.y);
            getNyaConfig().pausePositionZ.SetValue(position.z);
        
            getNyaConfig().pauseRotationX.SetValue(eulerRotation.x);
            getNyaConfig().pauseRotationY.SetValue(eulerRotation.y);
            getNyaConfig().pauseRotationZ.SetValue(eulerRotation.z);
        }
        if (this->currentScene == Nya::FloatingUIScene::MainMenu){
            INFO("Saved to MainMenu");
            getNyaConfig().menuPositionX.SetValue(position.x);
            getNyaConfig().menuPositionY.SetValue(position.y);
            getNyaConfig().menuPositionZ.SetValue(position.z);
            
            getNyaConfig().menuRotationX.SetValue(eulerRotation.x);
            getNyaConfig().menuRotationY.SetValue(eulerRotation.y);
            getNyaConfig().menuRotationZ.SetValue(eulerRotation.z);
        }
    }

    NyaFloatingUI* NyaFloatingUI::instance = nullptr;

    // singleton?
    NyaFloatingUI* NyaFloatingUI::get_instance()
    {
        if (instance)
            return instance;
        auto go = GameObject::New_ctor(StringW(___TypeRegistration::get()->name()));
        Object::DontDestroyOnLoad(go);
        return go->AddComponent<NyaFloatingUI*>();
    }

    void NyaFloatingUI::delete_instance()
    {
        if (instance){
            Object::DestroyImmediate(instance->get_gameObject());
            instance = nullptr;
        }
    }

//    Check if nya is enabled anywhere
    bool NyaFloatingUI::isEnabled (){
        return (
            getNyaConfig().inMenu.GetValue() ||
            getNyaConfig().inPause.GetValue() 
        );
    }

    void NyaFloatingUI::OnIsLoadingChange (bool isLoading) {
        QuestUI::MainThreadScheduler::Schedule([this, isLoading]
        {
            if (this->nyaButton && this->nyaButton->m_CachedPtr.m_value)
                this->nyaButton->set_interactable(!isLoading);
        });
    }

    void NyaFloatingUI::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);
        
   
        if (nextSceneName.find("Menu") != std::string::npos ) {
             QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::MainMenu);
            });
            
        } else if (nextSceneName.find("Pause") != std::string::npos ) {
            QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Pause);
            });
        } else {
            QuestUI::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Disabled);
            });
        }
    };

    void NyaFloatingUI::ScaleFloatingScreen(float scale) {
        if (this->UIScreen && this->UIScreen->m_CachedPtr.m_value) {
            this->UIScreen->get_gameObject()->get_transform()->set_localScale(
                UnityEngine::Vector3(
                    0.03f * scale,
                    0.03f * scale,
                    0.03f * scale
                )
            );
        }
    }

    void NyaFloatingUI::UpdateScale() {
        float scale = getNyaConfig().FloatingScreenScale.GetValue();
        this->ScaleFloatingScreen(scale);
    }

    void NyaFloatingUI::UpdateHandleVisibility(){
        bool visibility = getNyaConfig().ShowHandle.GetValue();
        if (this->screenhandle && this->screenhandle->m_CachedPtr.m_value) {
            this->screenhandle->set_active(visibility);
        }
    }
}