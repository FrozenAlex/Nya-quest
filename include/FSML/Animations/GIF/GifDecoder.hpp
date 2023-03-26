#pragma once

#include "../AnimationInfo.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "custom-types/shared/coroutine.hpp"

namespace FSML {
    class GifDecoder {
        public:
            static custom_types::Helpers::Coroutine Process(ArrayW<uint8_t> data, std::function<void(AnimationInfo*)> onFinished);
            static void ProcessingThread(ArrayW<uint8_t> gifData, AnimationInfo* animationInfo);
    };
}