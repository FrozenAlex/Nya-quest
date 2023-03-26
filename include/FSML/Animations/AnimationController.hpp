#pragma once

#include "custom-types/shared/macros.hpp"
#include "AnimationControllerData.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Rect.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"

DECLARE_CLASS_CODEGEN(FSML, AnimationController, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(AnimationControllerData*, loadingAnimation);
    using StringToAnimDataDictionary = System::Collections::Generic::Dictionary_2<StringW, Il2CppObject*>;
    DECLARE_INSTANCE_FIELD(StringToAnimDataDictionary*, registeredAnimations);
    DECLARE_INSTANCE_METHOD(AnimationControllerData*, Register, StringW key, UnityEngine::Texture2D* texture, ArrayW<UnityEngine::Rect> uvs, ArrayW<float> delays);
    DECLARE_INSTANCE_METHOD(void, InitializeLoadingAnimation);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, RemoveUnusedAnimationData);
    
    public:
        static AnimationController* get_instance();
        DECLARE_CTOR(ctor);
    protected:
        static SafePtrUnity<AnimationController> instance;
)