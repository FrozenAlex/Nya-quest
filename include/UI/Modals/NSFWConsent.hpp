#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/types.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "main.hpp"
#include "EndpointConfig.hpp"
#include "assets.hpp"

struct ModalContent {
    StringW TopText;
    StringW MidText;
    IncludedAsset MidImage;
    StringW NoButtonText;
    StringW YesButtonText;
    bool ButtonIntractabilityCooldown;
    bool Animated;
    bool ShowInputs;
    bool MathTime;
};

enum FadeOutContent
{
    HornyPastryPuffer,
    IncorrectMath,
    Blank
};

using namespace UnityEngine::UI;
using namespace TMPro;
using namespace HMUI;
using namespace QuestUI;



DECLARE_CLASS_CODEGEN(Nya, NSFWConsent, UnityEngine::MonoBehaviour,
    public:
        std::vector<ModalContent> ModalContents;
        int ConfirmationStage = 0;
        void InitModalContents();
        void UpdateModalContent();
        void Hide();
        void ChangeModalContent(ModalContent& modalContent);

        custom_types::Helpers::Coroutine InteractabilityCooldown(SliderSetting *);
        custom_types::Helpers::Coroutine InteractabilityCooldown(Button *);

        custom_types::Helpers::Coroutine FadeoutModal(FadeOutContent content);

        DECLARE_CTOR(ctor);

        DECLARE_INSTANCE_METHOD(void, Show );
        DECLARE_INSTANCE_METHOD(bool, isShown );

        // // Settings buttons and modal
        DECLARE_INSTANCE_FIELD(ModalView*, modal);

        DECLARE_INSTANCE_FIELD(VerticalLayoutGroup *, mainLayout);
        DECLARE_INSTANCE_FIELD(HorizontalLayoutGroup *, hornyPastryPufferLayout);

        DECLARE_INSTANCE_FIELD(HorizontalLayoutGroup *, buttonsLayout);
        DECLARE_INSTANCE_FIELD(HorizontalLayoutGroup *, sliderLayout);


        DECLARE_INSTANCE_FIELD(TextMeshProUGUI *, topText);
        DECLARE_INSTANCE_FIELD(TextMeshProUGUI *, midText);
        DECLARE_INSTANCE_FIELD(ImageView *, midImage);


        DECLARE_INSTANCE_FIELD(Button *, noButton);
        DECLARE_INSTANCE_FIELD(Button *, yesButton);

        DECLARE_INSTANCE_FIELD(SliderSetting *, slider);
        DECLARE_INSTANCE_FIELD(Button *, submitButton);
)