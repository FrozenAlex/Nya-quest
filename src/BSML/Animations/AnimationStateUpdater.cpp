#include "BSML/Animations/AnimationStateUpdater.hpp"

DEFINE_TYPE(BSML, AnimationStateUpdater);

namespace BSML {
    AnimationControllerData* AnimationStateUpdater::get_controllerData() {
        return controllerData;
    }

    void AnimationStateUpdater::set_controllerData(AnimationControllerData* value) {
        if (controllerData) {
            OnDisable();
        }
        controllerData = value;
        if (get_isActiveAndEnabled()) {
            OnEnable();
        }
    }

    void AnimationStateUpdater::OnEnable() {
        if (controllerData && image) {
            controllerData->get_activeImages()->Add(image);
        }
        if (controllerData) {
            controllerData->updatersCount++;
        }
    }

    void AnimationStateUpdater::OnDisable() {
        if (controllerData && image) {
            controllerData->get_activeImages()->Remove(image);
        }
    }

    void AnimationStateUpdater::OnDestroy() {
        if (controllerData && image) {
            controllerData->get_activeImages()->Remove(image);
        }
        if (controllerData) {
            controllerData->updatersCount--;
        }
    }
}