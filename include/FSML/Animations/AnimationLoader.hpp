#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "AnimationInfo.hpp"
#include "custom-types/shared/coroutine.hpp"


DECLARE_CLASS_CODEGEN(FSML, AnimationLoader, Il2CppObject,
    public:
        enum class AnimationType {
            GIF,
            APNG,
        };
        static void Process(AnimationType type, ArrayW<uint8_t> data, std::function<void(UnityEngine::Texture2D*, ArrayW<UnityEngine::Rect>, ArrayW<float>)> onProcessed);
        static custom_types::Helpers::Coroutine ProcessAnimationInfo(AnimationInfo* animationInfo, std::function<void(UnityEngine::Texture2D*, ArrayW<UnityEngine::Rect>, ArrayW<float>)> onProcessed);
        static int GetTextureSize(AnimationInfo* animationInfo, int i);
)