#include "BSML/Animations/AnimationControllerData.hpp"

#include "Helpers/utilities.hpp"

#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include <chrono>

DEFINE_TYPE(BSML, AnimationControllerData);


namespace BSML {
    AnimationControllerData* AnimationControllerData::Make_new(UnityEngine::Texture2D* tex, ArrayW<UnityEngine::Rect> uvs, ArrayW<float> delays) {
        auto self = AnimationControllerData::New_ctor();
        // Init uvIndex to 0 for force frame drawing thing
        self->uvIndex = 0; 
        self->updatersCount = 0;
        self->_isPlaying = true;
        self->isDelayConsistent = true;
        auto time = std::chrono::system_clock::now();
        auto milis = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch());
        self->lastSwitch = milis.count();

        self->sprites = ArrayW<UnityEngine::Sprite*>(uvs.size());
        float firstDelay = -1;
        int texWidth = tex->get_width();
        int texHeight = tex->get_height();

        for (int i = 0; i < uvs.size(); i++) {
            self->sprites[i] = UnityEngine::Sprite::Create(
                tex, 
                UnityEngine::Rect(
                    uvs[i].m_XMin * texWidth, 
                    uvs[i].m_YMin * texHeight,
                    uvs[i].m_Width * texWidth,
                    uvs[i].m_Height * texHeight
                ),
                {0, 0},
                100.0f,
                0,
                UnityEngine::SpriteMeshType::Tight,
                {0, 0, 0, 0},
                false
            );

            if (i == 0) {
                firstDelay = delays[i];
            }

            if (delays[i] != firstDelay) {
                self->isDelayConsistent = false;
            }
        }

        self->sprite = Utilities::LoadSpriteFromTexture(tex);
        self->uvs = uvs;
        self->delays = delays;

        return self;
    }

    ListWrapper<UnityEngine::UI::Image*> AnimationControllerData::get_activeImages() {
        if (!activeImages) {
            activeImages = List<UnityEngine::UI::Image*>::New_ctor();
        }
        return activeImages;
    }

    bool AnimationControllerData::get_isPlaying() {
        return _isPlaying;
    }

    /**
     * Has any updaters connected to it (safe to delete)
    */
    bool AnimationControllerData::isUsed() {
        return updatersCount > 0;
    }

    /**
     * Draws first frame, useful for deleting old frames safely
    */
    void AnimationControllerData::ForceDrawFrame(){
        for (auto image : get_activeImages()) {
            image->set_sprite(sprites[uvIndex]);
        }
    }

    void AnimationControllerData::set_isPlaying(bool value) {
        _isPlaying = value;
    }

    void AnimationControllerData::CheckFrame(unsigned long long now) {
        if (get_activeImages().size() == 0) return;

        auto diffMs = (now - lastSwitch);
        if (diffMs < delays[uvIndex]) return;
        // VV Bump animations with consistently 10ms or lower frame timings to 100ms
        if (isDelayConsistent && delays[uvIndex] <= 10 && diffMs < 100) return;

        lastSwitch = now;
        do {
            uvIndex++;
            if (uvIndex >= uvs.size()) uvIndex = 0;
        } while (!isDelayConsistent && delays[uvIndex] == 0);

        for (auto image : get_activeImages()) {
            image->set_sprite(sprites[uvIndex]);
        }
    }
}