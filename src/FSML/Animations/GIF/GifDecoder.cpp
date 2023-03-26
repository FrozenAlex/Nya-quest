#include "FSML/Animations/GIF/GifDecoder.hpp"
#include "main.hpp"
#include "Helpers/delegates.hpp"

#include "System/Threading/Tasks/Task.hpp"
#include "System/Func_1.hpp"
#include "System/Func_1.hpp"
#include "UnityEngine/WaitUntil.hpp"

#include "gif-lib/shared/gif_lib.h"

#include "EasyGifReader.h"

std::string errToString(const EasyGifReader::Error& err) {
    switch (err) {
        default:
        case EasyGifReader::Error::UNKNOWN: return "UNKNOWN";
        case EasyGifReader::Error::INVALID_OPERATION: return "INVALID_OPERATION";
        case EasyGifReader::Error::OPEN_FAILED: return "OPEN_FAILED";
        case EasyGifReader::Error::READ_FAILED: return "READ_FAILED";
        case EasyGifReader::Error::INVALID_FILENAME: return "INVALID_FILENAME";
        case EasyGifReader::Error::NOT_A_GIF_FILE: return "NOT_A_GIF_FILE";
        case EasyGifReader::Error::INVALID_GIF_FILE: return "INVALID_GIF_FILE";
        case EasyGifReader::Error::OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case EasyGifReader::Error::CLOSE_FAILED: return "CLOSE_FAILED";
        case EasyGifReader::Error::NOT_READABLE: return "NOT_READABLE";
        case EasyGifReader::Error::IMAGE_DEFECT: return "IMAGE_DEFECT";
        case EasyGifReader::Error::UNEXPECTED_EOF: return "UNEXPECTED_EOF";
    }
}

/// @brief makes RGB (0, 0, 0) into transparent pixels
inline uint32_t make_black_transparent(const uint32_t& v) {
    return v >> 8 ? v : 0;
}

namespace FSML {

    custom_types::Helpers::Coroutine GifDecoder::Process(ArrayW<uint8_t> data, std::function<void(AnimationInfo*)> onFinished) {
        auto animationInfo = new AnimationInfo();
        auto taskDelegate = MakeSystemAction(
            [data, animationInfo]() {
                GifDecoder::ProcessingThread(data, animationInfo);
            }
        );

        System::Threading::Tasks::Task::Run(taskDelegate);

        auto waitFunc = MakeDelegate<System::Func_1<bool>*>(
            std::function<bool()>(
                [animationInfo]() -> bool {
                    return animationInfo->isInitialized || animationInfo->isFailed;
                }
            )
        );

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitUntil::New_ctor(waitFunc));
        if (onFinished) 
            onFinished(animationInfo);
        else 
            ERROR("Nullptr onFinished given!");
        co_return;
    }

    void GifDecoder::ProcessingThread(ArrayW<uint8_t> gifData, AnimationInfo* animationInfo) {
        DEBUG("Open gif");
        try {
            auto gifReader = EasyGifReader::openMemory(gifData.begin(), gifData.size());
            int width = gifReader.width(), height = gifReader.height(), frameCount = gifReader.frameCount();

            animationInfo->frameCount = frameCount;
            animationInfo->frames.reserve(frameCount);
            animationInfo->isInitialized = true;

            DEBUG("iterating gif frames");
            for (const auto& gifFrame : gifReader) {
                auto currentFrame = new FrameInfo(gifFrame.width(), gifFrame.height());

                const uint8_t* pixels = (const uint8_t*)gifFrame.pixels();
                // get end of the data
                uint8_t* colorData = currentFrame->colors.ptr()->values + currentFrame->colors.ptr()->Length();
                int height = gifFrame.height();
                int rowSize = sizeof(uint32_t) * gifFrame.width();
                // we need to iterate the given data in reverse due to unity's texture system
                for (int y = 0; y < height; y++) {
                    // pre-decrement because we start at end of data
                    colorData -= rowSize;
                    // just copy all the data as is
                    //memcpy(colorData, pixels, rowSize);
                    // make black pixels transparent
                    std::transform((uint32_t*)pixels, (uint32_t*)(pixels + rowSize), (uint32_t*)colorData, make_black_transparent);
                    pixels += rowSize;
                }

                // delay in millis
                currentFrame->delay = gifFrame.rawDuration().milliseconds();
                // add to end of vector
                animationInfo->frames.emplace_back(currentFrame);
            }
        } catch (EasyGifReader::Error gifError) {
            animationInfo->isFailed = true;
            // TODO: Cleanup on gif errors
            ERROR("Gif error: {}", errToString(gifError));
        }
    }
}