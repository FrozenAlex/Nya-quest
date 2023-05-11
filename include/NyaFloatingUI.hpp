// This code is mostly taken from https://github.com/Royston1999/SliceDetails-Quest
#pragma once

#include "UI/HoverClickHelper.hpp"
#include <cmath>
#define _USE_MATH_DEFINES
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenMoverPointer.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Plane.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/Screen.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UI/Modals/SettingsMenu.hpp"
#include "ImageView.hpp"


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
            void updateCoordinates(UnityEngine::Transform* transform);
            void updateCoordinates(UnityEngine::Vector3 position, UnityEngine::Vector3 eulerRotation);
            void OnActiveSceneChanged(UnityEngine::SceneManagement::Scene current, UnityEngine::SceneManagement::Scene _);
            void onSceneChange(Nya::FloatingUIScene scene, bool reinitialize = false);

            Nya::FloatingUIScene currentScene = Nya::FloatingUIScene::Disabled;
            bool isInitialized = false;

            DECLARE_INSTANCE_METHOD(void, SetDefaultPos);
            DECLARE_INSTANCE_METHOD(void, ScaleFloatingScreen, float scale);
            DECLARE_INSTANCE_METHOD(void, UpdateScale);

            DECLARE_INSTANCE_METHOD(void, UpdateHandleVisibility);

            DECLARE_INSTANCE_METHOD(void, OnIsLoadingChange, bool isLoading); 
            
            DECLARE_CTOR(ctor);
            // DECLARE_DTOR(dtor);

            DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, screenhandle);
            DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, UIScreen);
            DECLARE_INSTANCE_FIELD(UnityEngine::Material*, UINoGlow);
            
            DECLARE_INSTANCE_FIELD(Nya::HoverClickHelper*, hoverClickHelper);

            // NYA
            DECLARE_INSTANCE_FIELD(HMUI::ImageView*, NYA);
            DECLARE_INSTANCE_FIELD(NyaUtils::ImageView*, imageView);
            DECLARE_INSTANCE_FIELD(UnityEngine::Plane*, plane);

            DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, nyaButton);
            // Settings buttons and modal
            DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, settingsButton);
            DECLARE_INSTANCE_FIELD(Nya::SettingsMenu*, settingsMenu);

            static NyaFloatingUI* get_instance();
            static void delete_instance();
            static bool isEnabled();
        private:
            static NyaFloatingUI* instance;       
)