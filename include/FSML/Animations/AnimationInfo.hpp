#pragma once

#include <vector>
// this is just included because otherwise the SafePtr complains about conversion to unity object
#include "UnityEngine/Object.hpp"

namespace FSML {
    class FrameInfo;
    class AnimationInfo {
        public:
            ~AnimationInfo();
            bool isInitialized = false;
            bool isFailed = false;
            int frameCount = 0;
            std::vector<FrameInfo*> frames;
    };
    
    class FrameInfo {
        public:
            int width, height, bpp;
            SafePtr<Array<uint8_t>> colors;
            int delay;
            FrameInfo(int width, int height, int bpp = 4);
    };
}