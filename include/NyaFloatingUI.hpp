// This code is mostly taken from https://github.com/Royston1999/SliceDetails-Quest
#pragma once

#include <cmath>
#define _USE_MATH_DEFINES
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Plane.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/Screen.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UI/Modals/SettingsMenu.hpp"
#include "ImageView.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"


using namespace UnityEngine::UI;

namespace Nya {
    enum FloatingUIScene {
        Disabled, Pause, MainMenu
    };
};


DECLARE_CLASS_CODEGEN(Nya, NyaFloatingUI, UnityEngine::MonoBehaviour,
        public:
            void initScreen();
            void onPause();
            void onUnPause();
            void onResultsScreenActivate();
            void onResultsScreenDeactivate();
            void updateCoordinates(BSML::FloatingScreen* self, const BSML::FloatingScreenHandleEventArgs& args);
            void updateCoordinates(UnityEngine::Transform* transform);
            void updateCoordinates(UnityEngine::Vector3 position, UnityEngine::Vector3 eulerRotation);
            void OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current, UnityEngine::SceneManagement::Scene _);
            void onSceneChange(Nya::FloatingUIScene scene, bool reinitialize = false);
            void ensureAboveTheGround();

            Nya::FloatingUIScene currentScene = Nya::FloatingUIScene::Disabled;
            bool isInitialized = false;

            DECLARE_INSTANCE_METHOD(void, SetDefaultPos);
            DECLARE_INSTANCE_METHOD(void, ScaleFloatingScreen, float scale);
            DECLARE_INSTANCE_METHOD(void, UpdateScale);
            DECLARE_INSTANCE_METHOD(void, LookAtCamera);
            DECLARE_INSTANCE_METHOD(void, SetUpRight);

            DECLARE_INSTANCE_METHOD(void, UpdateHandleVisibility);

            DECLARE_INSTANCE_METHOD(void, OnIsLoadingChange, bool isLoading); 
            
            DECLARE_CTOR(ctor);
            DECLARE_INSTANCE_FIELD(UnityW<BSML::FloatingScreen>, floatingScreen);
            DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::GameObject>, floatingScreenGO);
            DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Material>, UINoGlow);

            // NYA
            DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, NYA);
            DECLARE_INSTANCE_FIELD(UnityW<NyaUtils::ImageView>, imageView);
            DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::Plane>, plane);

            DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, nyaButton);
            // Settings buttons and modal
            DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, settingsButton);
            DECLARE_INSTANCE_FIELD(UnityW<Nya::SettingsMenu>, settingsMenu);

            static NyaFloatingUI* get_instance();
            static void delete_instance();
            static bool isEnabled();
        private:
            static NyaFloatingUI* instance;       
)