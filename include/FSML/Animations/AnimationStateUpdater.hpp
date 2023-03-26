#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/UI/Image.hpp"

#include "AnimationControllerData.hpp"

DECLARE_CLASS_CODEGEN(FSML, AnimationStateUpdater, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Image*, image);
    DECLARE_INSTANCE_FIELD(AnimationControllerData*, controllerData);
    DECLARE_INSTANCE_METHOD(AnimationControllerData*, get_controllerData);
    DECLARE_INSTANCE_METHOD(void, set_controllerData, AnimationControllerData* value);

    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
)