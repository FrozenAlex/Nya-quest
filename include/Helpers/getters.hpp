#pragma once

#include "UnityEngine/Material.hpp"
#include "VRUIControls/PhysicsRaycasterWithCache.hpp"
#include "Zenject/DiContainer.hpp"
#include "HMUI/HoverHintController.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "GlobalNamespace/IVRPlatformHelper.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

namespace FSML::Helpers {
    VRUIControls::PhysicsRaycasterWithCache* GetPhysicsRaycasterWithCache();
    Zenject::DiContainer* GetDiContainer();
    HMUI::HoverHintController* GetHoverHintController();

    TMPro::TMP_FontAsset* GetMainTextFont();
    UnityEngine::Material* GetMainUIFontMaterial();
    UnityEngine::Material* GetUINoGlowMat();
    GlobalNamespace::IVRPlatformHelper* GetIVRPlatformHelper();
    GlobalNamespace::MainFlowCoordinator* GetMainFlowCoordinator();
}