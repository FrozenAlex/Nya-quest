#include "main.hpp"
#include "FSML/Animations/AnimationLoader.hpp"
#include "FSML/Animations/GIF/GifDecoder.hpp"


#include "UnityEngine/SystemInfo.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/WaitUntil.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "System/IO/File.hpp"

#include "Helpers/delegates.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "System/Func_1.hpp"
DEFINE_TYPE(FSML, AnimationLoader);


namespace FSML {
    int get_atlasSizeLimit() {
        using GetMaxTextureSize = function_ptr_t<int>;
        static auto getMaxTextureSize = reinterpret_cast<GetMaxTextureSize>(il2cpp_functions::resolve_icall("UnityEngine.SystemInfo::GetMaxTextureSize"));
        static int maxSize = getMaxTextureSize();
        return maxSize >= 4096 ? 4096 : maxSize;
    }

    void AnimationLoader::Process(AnimationType type, ArrayW<uint8_t> data, std::function<void(UnityEngine::Texture2D*, ArrayW<UnityEngine::Rect>, ArrayW<float>)> onProcessed) {
        auto sharedStarter = GlobalNamespace::SharedCoroutineStarter::get_instance();
        DEBUG("Starting animation decode");
        switch (type) {
            case AnimationType::GIF:
                sharedStarter->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                    GifDecoder::Process(data, 
                    [sharedStarter, onProcessed](AnimationInfo* animationInfo){ 
                        DEBUG("Processed Data, processing animation info");
                        if (animationInfo == nullptr) {
                            onProcessed(nullptr, ArrayW<UnityEngine::Rect>(), ArrayW<float>());
                            ERROR("animation info null");
                            return;
                        }

                        if (animationInfo->isFailed == true) {
                            onProcessed(nullptr, ArrayW<UnityEngine::Rect>(), ArrayW<float>());
                            ERROR("animation failed");
                            return;
                        }

                        sharedStarter->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(ProcessAnimationInfo(animationInfo, onProcessed)));
                    })));
                break;
            case AnimationType::APNG:
                // TODO: implement APNG support at some point?
                break;
            default:
                break;
        }
    }

    custom_types::Helpers::Coroutine AnimationLoader::ProcessAnimationInfo(AnimationInfo* animationInfo, std::function<void(UnityEngine::Texture2D*, ArrayW<UnityEngine::Rect>, ArrayW<float>)> onProcessed) {
        // Wait for a bit to reduce corruption
        while (animationInfo->frames.size() < animationInfo->frameCount) {
            co_yield nullptr; 
        }

        DEBUG("ProcessAnimInfo");
        int textureSize = get_atlasSizeLimit(), width = 0, height = 0;
        UnityEngine::Texture2D* texture = nullptr;
        auto textureList = ArrayW<UnityEngine::Texture2D*>(animationInfo->frameCount);
        ArrayW<float> delays = ArrayW<float>(animationInfo->frameCount);

        float lastThrottleTime = UnityEngine::Time::get_realtimeSinceStartup();

        for (int i = 0; i < animationInfo->frameCount; i++) {
            if (animationInfo->frames.size() <= i) {
                auto waitUntil = UnityEngine::WaitUntil::New_ctor(
                    MakeDelegate<System::Func_1<bool>*>(
                        std::function<bool()>(
                            [animationInfo, i]() -> bool {
                                return animationInfo->frames.size() > i;
                            }
                        )
                    )
                );
                co_yield reinterpret_cast<System::Collections::IEnumerator*>(waitUntil);
                lastThrottleTime = UnityEngine::Time::get_realtimeSinceStartup();
            }

            auto& currentFrameInfo = animationInfo->frames.at(i);
            if (!texture) {
                textureSize = GetTextureSize(animationInfo, i);
                
                width = currentFrameInfo->width;
                height = currentFrameInfo->height;
                texture = UnityEngine::Texture2D::New_ctor(width, height);
            }

            delays[i] = currentFrameInfo->delay;
            auto frameTexture = UnityEngine::Texture2D::New_ctor(currentFrameInfo->width, currentFrameInfo->height, UnityEngine::TextureFormat::RGBA32, false);
            frameTexture->set_wrapMode(UnityEngine::TextureWrapMode::Clamp);
            frameTexture->LoadRawTextureData(currentFrameInfo->colors.ptr());
            textureList[i] = frameTexture;

            if (UnityEngine::Time::get_realtimeSinceStartup() > lastThrottleTime + 0.0005f) {
                co_yield nullptr;
                lastThrottleTime = UnityEngine::Time::get_realtimeSinceStartup();
            }

            delete currentFrameInfo;
            currentFrameInfo = nullptr;
        }
        // note to self, no longer readable = true means you can't encode the texture to png!
        auto atlas = texture->PackTextures(textureList, 2, textureSize, true);
        // cleanup
        for (auto t : textureList) {
            UnityEngine::Object::DestroyImmediate(t);
        }
        if (onProcessed)
            onProcessed(texture, atlas, delays);

        // we are now done with the animation info
        delete animationInfo;
        co_return;
    }

    int AnimationLoader::GetTextureSize(AnimationInfo* animationInfo, int i) {
        int testNum = 2, numFramesInRow = 0, numFramesInColumn = 0;
        
        while (true) {
            int numFrames = animationInfo->frameCount;

            if ((numFrames % testNum) == 0) {
                numFrames += numFrames % testNum;
            }

            numFramesInRow = std::max(numFrames / testNum, 1);
            numFramesInColumn = numFrames / numFramesInRow;
            if (numFramesInRow <= numFramesInColumn) break;
            testNum += 2;
        }
        
        int textureWidth = std::clamp(numFramesInRow * animationInfo->frames[i]->width, 0, get_atlasSizeLimit());
        int textureHeight = std::clamp(numFramesInColumn * animationInfo->frames[i]->height, 0, get_atlasSizeLimit());
        return std::max(textureWidth, textureHeight);
    }
}