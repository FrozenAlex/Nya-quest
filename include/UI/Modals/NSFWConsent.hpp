#pragma once

#include "custom-types/shared/macros.hpp"
#include "bsml/shared/BSML/Components/Settings/SliderSetting.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "assets.hpp"

struct ModalContent {
    std::string TopText;
    std::string MidText;
    IncludedAsset MidImage;
    std::string NoButtonText;
    std::string YesButtonText;
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
DECLARE_CLASS_CODEGEN(Nya::UI::Modals, NSFWConsent, UnityEngine::MonoBehaviour,
    public:
        std::vector<ModalContent> ModalContents;
        int ConfirmationStage = 0;
        void InitModalContents();
        void UpdateModalContent();
        void Hide();
        void ChangeModalContent(ModalContent& modalContent);

        custom_types::Helpers::Coroutine InteractabilityCooldown(BSML::SliderSetting *);
        custom_types::Helpers::Coroutine InteractabilityCooldown(UnityEngine::UI::Button *);

        custom_types::Helpers::Coroutine FadeoutModal(FadeOutContent content);

        DECLARE_CTOR(ctor);
        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, Show );
        DECLARE_INSTANCE_METHOD(bool, isShown );

        // // Settings buttons and modal
        DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, modal);

        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::VerticalLayoutGroup>, mainLayout);
        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalLayoutGroup>, hornyPastryPufferLayout);

        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalLayoutGroup>, buttonsLayout);
        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::HorizontalLayoutGroup>, sliderLayout);


        DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, topText);
        DECLARE_INSTANCE_FIELD(UnityW<TMPro::TextMeshProUGUI>, midText);
        DECLARE_INSTANCE_FIELD(UnityW<HMUI::ImageView>, midImage);


        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, noButton);
        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, yesButton);

        DECLARE_INSTANCE_FIELD(UnityW<BSML::SliderSetting>, slider);
        DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, submitButton);
);