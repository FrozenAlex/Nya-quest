#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "main.hpp"
#include "logging.hpp"
#include "EndpointConfigUtils.hpp"

#include "Utils/Utils.hpp"
#include "ImageView.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "Utils/FileUtils.hpp"
#include "assets.hpp"
#include "libs/magic_enum.hpp"
#include "UnityEngine/Camera.hpp"
#include "Utils/Utils.hpp"

using namespace UnityEngine::UI;
using namespace UnityEngine;

DEFINE_TYPE(Nya, NyaFloatingUI);

namespace Nya {
    // Function gets url for the current selected category

    void NyaFloatingUI::ctor()
    {
        imageView = nullptr;
        INFO("Created NyaFloatingUI instance");
    }

    void NyaFloatingUI::initScreen(){
        if (this->isInitialized){
            return;
        }

        this->floatingScreen = BSML::Lite::CreateFloatingScreen({100.0f, 80.0f}, {0.0f, 1.0f, 1.0f}, {0, 0, 0}, 0.0f, false, true, BSML::Side::Bottom);
        floatingScreen->set_HighlightHandle(true);

        auto UIScreen = floatingScreen->get_gameObject();
        UIScreen->set_active(false);
        UIScreen->GetComponent<UnityEngine::Canvas*>()->set_sortingOrder(31);

        UnityEngine::GameObject::DontDestroyOnLoad(UIScreen);

        floatingScreen->handle->get_gameObject()->get_transform()->set_localPosition(UnityEngine::Vector3(0.0f, -32.0f, 0.0f));
        floatingScreen->handle->get_transform()->set_localScale(UnityEngine::Vector3(5.3f, 3.3f, 5.3f));

        auto* vert = BSML::Lite::CreateVerticalLayoutGroup(floatingScreen->get_transform());

        vert->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        NYA = BSML::Lite::CreateImage(vert->get_transform(), nullptr, Vector2::get_zero(), Vector2(50, 50));
        NYA->set_preserveAspect(true);
        // Set blank sprite to avoid white screens
        NYA->set_sprite(BSML::Lite::ArrayToSprite(Assets::placeholder_png));

        auto ele = NYA->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();

        DEBUG("Adding image view to the game object");
        imageView = NYA->get_gameObject()->AddComponent<NyaUtils::ImageView*>();
        ele->set_preferredHeight(70);
        ele->set_preferredWidth(70);

        auto horz = BSML::Lite::CreateHorizontalLayoutGroup(vert->get_transform());
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);


        // Get new picture
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

        // Set size of the buttons
        UnityEngine::Vector2 sizeDelta = {19, 8};
        Nya::Utils::SetButtonSize(this->nyaButton, sizeDelta);
        Nya::Utils::SetButtonSize(this->settingsButton, sizeDelta);

        this->isInitialized = true;

        this->UpdateScale();
        this->UpdateHandleVisibility();

        // Sub to events
        if (this->imageView) {
            this->imageView->imageLoadingChange.addCallback(&NyaFloatingUI::OnIsLoadingChange, this);
        } else {
            INFO("ImageView not found");
        }

        floatingScreen->HandleReleased.addCallback(&NyaFloatingUI::updateCoordinates, this);
    }

    void NyaFloatingUI::SetDefaultPos () {
        // Do nothing if hover click helper is not present
        
        // If screen does not exist, initialize the first time to reset stuff
        if (!this->floatingScreen) {
            DEBUG("LOL IT ACTUALLY HAPPENS");
            // this->isInitialized = false;
            this->initScreen();
        }

        if (this->currentScene == Nya::FloatingUIScene::Pause) {
            this->floatingScreen->set_ScreenRotation(UnityEngine::Quaternion::Euler(getNyaConfig().pauseRotation.GetDefaultValue()));
            this->floatingScreen->set_ScreenPosition(getNyaConfig().pausePosition.GetDefaultValue());

            // Save target 
            this->updateCoordinates(
                getNyaConfig().pausePosition.GetDefaultValue(),
                getNyaConfig().pauseRotation.GetDefaultValue()
            );                
        }

        if (this->currentScene == Nya::FloatingUIScene::MainMenu) {
            this->floatingScreen->set_ScreenRotation(UnityEngine::Quaternion::Euler(getNyaConfig().menuRotation.GetDefaultValue()));
            this->floatingScreen->set_ScreenPosition(getNyaConfig().menuPosition.GetDefaultValue());

            // Save coordinates
            this->updateCoordinates(
                getNyaConfig().menuPosition.GetDefaultValue(),
                getNyaConfig().menuRotation.GetDefaultValue()
            );
        }
    }

    void NyaFloatingUI::onSceneChange(Nya::FloatingUIScene scene, bool reinitialize) {
        INFO("Switched from {} to {} ", magic_enum::enum_name(this->currentScene), magic_enum::enum_name(scene));
        
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
                if (this->floatingScreen) floatingScreen->get_gameObject()->set_active(false);
                
                return;
        };

        // If screen does not exist, initialize the first time
        if (!this->floatingScreen) {
            this->initScreen();
            DEBUG("Initialized screen");
           

            // If the screen is created, get the first image after a delay to avoid being too fast
            BSML::MainThreadScheduler::ScheduleAfterTime(0.3f, [this](){
                DEBUG("Getting first image");
                this->imageView->GetImage(nullptr);
            });
        }

        if (scene == Nya::FloatingUIScene::Pause) {
            INFO("Showing pause");
            this->floatingScreen->set_ScreenRotation(UnityEngine::Quaternion::Euler(getNyaConfig().pauseRotation.GetValue()));
            this->floatingScreen->set_ScreenPosition(getNyaConfig().pausePosition.GetValue());
        }

        if (scene == Nya::FloatingUIScene::MainMenu) {
            INFO("Showing main menu");
            this->floatingScreen->set_ScreenRotation(UnityEngine::Quaternion::Euler(getNyaConfig().menuRotation.GetValue()));
            this->floatingScreen->set_ScreenPosition(getNyaConfig().menuPosition.GetValue());
        }

        floatingScreen->get_gameObject()->set_active(true);
    }
    
    void NyaFloatingUI::updateCoordinates(BSML::FloatingScreen* self, const BSML::FloatingScreenHandleEventArgs& args) {
        DEBUG("Updating coordinates called from handle event");
        this->updateCoordinates(self->get_transform());
    }

    // Saves the coordinates to a config
    void NyaFloatingUI::updateCoordinates(UnityEngine::Transform* transform){
        auto position = transform->get_position();
        auto rotation = transform->get_rotation().get_eulerAngles();

        this->updateCoordinates(position, rotation);
    }

    void NyaFloatingUI::updateCoordinates(UnityEngine::Vector3 position, UnityEngine::Vector3 eulerRotation) {
        if (this->currentScene == Nya::FloatingUIScene::Pause){
            getNyaConfig().pausePosition.SetValue(position);
            getNyaConfig().pauseRotation.SetValue(eulerRotation);
        }
        if (this->currentScene == Nya::FloatingUIScene::MainMenu){
            getNyaConfig().menuPosition.SetValue(position);
            getNyaConfig().menuRotation.SetValue(eulerRotation);
        }
    }

    NyaFloatingUI* NyaFloatingUI::instance = nullptr;

    NyaFloatingUI* NyaFloatingUI::get_instance()
    {
        if (instance)
            return instance;

        auto go = GameObject::New_ctor(StringW(___TypeRegistration::get()->name()));
        Object::DontDestroyOnLoad(go);
        instance = go->AddComponent<NyaFloatingUI*>();
        return instance;
    }

    void NyaFloatingUI::delete_instance()
    {
        if (instance){
            if (instance->get_gameObject()){
                Object::DestroyImmediate(instance->get_gameObject());
            }
            instance = nullptr;
        }
    }

    // Check if nya is enabled anywhere
    bool NyaFloatingUI::isEnabled (){
        return (
            getNyaConfig().inMenu.GetValue() ||
            getNyaConfig().inPause.GetValue() 
        );
    }

    void NyaFloatingUI::OnIsLoadingChange (bool isLoading) {
        BSML::MainThreadScheduler::Schedule([this, isLoading]
        {
            if (this->nyaButton)
                this->nyaButton->set_interactable(!isLoading);
        });
    }

    void NyaFloatingUI::OnActiveSceneChanged(UnityEngine::SceneManagement::Scene prevScene, UnityEngine::SceneManagement::Scene nextScene) {
        std::string prevSceneName(prevScene.get_name());
        std::string nextSceneName(nextScene.get_name());
        INFO("scene changed from {} to {}", prevSceneName, nextSceneName);
        
   
        if (nextSceneName.find("Menu") != std::string::npos ) {
             BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::MainMenu);
            });
            
        } else if (nextSceneName.find("Pause") != std::string::npos ) {
            BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Pause);
            });
        } else {
            BSML::MainThreadScheduler::Schedule([this]
            {
                this->onSceneChange(Nya::FloatingUIScene::Disabled);
            });
        }
    };

    void NyaFloatingUI::ScaleFloatingScreen(float scale) {
        if (this->floatingScreen) {
            this->floatingScreen->get_gameObject()->get_transform()->set_localScale(
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
        if (this->floatingScreen) {
            this->floatingScreen->set_ShowHandle(visibility);
        }
    }


    void NyaFloatingUI::LookAtCamera(){
        auto mainCamera = UnityEngine::Camera::get_main();
        if (!mainCamera) return;
        auto currentPosition = this->floatingScreen->get_ScreenPosition();
        auto newRotation = UnityEngine::Quaternion::LookRotation(
            Vector3::op_Subtraction(
                currentPosition,
                mainCamera->get_transform()->get_position())
            );
        // TODO: Do lerp
        this->floatingScreen->set_ScreenRotation(newRotation);
        Main::NyaFloatingUI->updateCoordinates(currentPosition, newRotation.get_eulerAngles());
    }

    void NyaFloatingUI::SetUpRight (){
        if (!this->floatingScreen) return;
        auto currentPosition = this->floatingScreen->get_ScreenPosition();
        auto currentRotation = this->floatingScreen->get_ScreenRotation();
        auto newRotation = UnityEngine::Quaternion::Euler(0.0, currentRotation.get_eulerAngles().y, 0.0);
       
        Main::NyaFloatingUI->updateCoordinates(currentPosition, newRotation.get_eulerAngles());
    }
}