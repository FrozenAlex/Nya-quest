#pragma once
// Settings menu popup for floating window and non floating window
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Plane.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/Screen.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "HMUI/ModalView.hpp"
#include "UI/PanelUI.hpp"
#include "custom-types/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreenMoverPointer.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"


DECLARE_CLASS_CODEGEN(Nya, SettingsMenu, UnityEngine::MonoBehaviour,
    public:
       
        void Hide();

        DECLARE_CTOR(ctor);

        // If the menu is floating
        bool isFloating = false;

        DECLARE_INSTANCE_METHOD(void, Show );
        DECLARE_INSTANCE_METHOD(bool, isShown );

        // Settings buttons and modal
        DECLARE_INSTANCE_FIELD(HMUI::ModalView*, settingsModal);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, settingsButton);
        DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, api_switch);
        DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, sfw_endpoint);
        
        DECLARE_INSTANCE_FIELD(HMUI::SimpleTextDropdown*, nsfw_endpoint);
        DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, nsfw_toggle);
        
        DECLARE_INSTANCE_FIELD(List<StringW>*, sfw_endpoints);
        DECLARE_INSTANCE_FIELD(List<StringW>*, nsfw_endpoints);
)