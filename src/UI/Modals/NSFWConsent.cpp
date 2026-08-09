#include "UI/Modals/NSFWConsent.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "assets.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/Transform.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "bsml/shared/BSML-Lite/Creation/Settings.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "bsml/shared/BSML-Lite/Creation/Image.hpp"
#include "NyaConfig.hpp"

DEFINE_TYPE(Nya::UI::Modals, NSFWConsent);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
using namespace std;


custom_types::Helpers::Coroutine Nya::UI::Modals::NSFWConsent::InteractabilityCooldown(SliderSetting * setting) {
    setting->set_interactable(false);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2.0f));
    setting->get_interactable();
    setting->set_interactable(true);
    co_return;
} 

custom_types::Helpers::Coroutine Nya::UI::Modals::NSFWConsent::InteractabilityCooldown(Button * button) {
    button->set_interactable(false);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2.0f));
    button->set_interactable(true);
    
    co_return;
} 



custom_types::Helpers::Coroutine Nya::UI::Modals::NSFWConsent::FadeoutModal(FadeOutContent content) { 
    // TODO: animate fadeout
    co_yield nullptr;

    switch (content)
    {
    case FadeOutContent::HornyPastryPuffer:
        {
            mainLayout->get_gameObject()->SetActive(false);
            hornyPastryPufferLayout->get_gameObject()->SetActive(true);
            // Wait for 2 seconds
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2));
            modal->Hide();

            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuTransitionsHelper*>()[0]->RestartGame(nullptr);
        }
        break;
    case FadeOutContent::IncorrectMath:
        {
            mainLayout->get_gameObject()->SetActive(false);
            hornyPastryPufferLayout->get_gameObject()->SetActive(false);
            // Wait for 2 seconds
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2));
            modal->Hide();
        }
        break;
    
    case FadeOutContent::Blank:
        {
            mainLayout->get_gameObject()->SetActive(false);
            hornyPastryPufferLayout->get_gameObject()->SetActive(false);
            // Wait for 2 seconds
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2));
            modal->Hide();
        }
        break;
    
    default:
        break;
    }


}

void Nya::UI::Modals::NSFWConsent::UpdateModalContent() {
    if (ConfirmationStage >= ModalContents.size()) {

        // Enable nsfw
        getNyaConfig().NSFWUI.SetValue(true);
        
        this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
            this->FadeoutModal(FadeOutContent::HornyPastryPuffer)
        ));

        return;
    }

    auto& modalContent = ModalContents[ConfirmationStage];
    ChangeModalContent(modalContent);
}

void Nya::UI::Modals::NSFWConsent::InitModalContents() {
    ModalContents.clear();
 ModalContents.push_back(
            {
                "Woah There!",
                "Are you sure you want to enable NSFW features? You have to be 18+ to do this!",
                IncludedAssets::Chocola_Surprised_png,
                "No",
                "Yes, I'm 18+",
                true,
                false,
                true,
                false
            }
        );

        ModalContents.push_back(
            {
                "Are you sure?",
                "Mhm... you super sure you're 18 or older??",
                IncludedAssets::Chocola_Question_Mark_png,
                "No, I'm not",
                "Yes, I'm certain",
                true,
                true,
                true,
                false
            }
        );
        ModalContents.push_back(
            {
                "Are you very sure?",
                "If you're lying I will find out and tell your parents. \r\n(They will be very disappointed in you)",
                IncludedAssets::Chocola_Angry_png,
                "Sorry, I lied",
                "Yes, I'm not lying",
                true,
                true,
                true,
                false
            }
        );
        ModalContents.push_back(
            {
                "Just double checking",
                "Okay so you're absolutely positive that you're 18+ and want to enable NSFW features?",
                IncludedAssets::Chocola_Howdidyoudothat_png,
                "No",
                "Yes",
                true,
                true,
                true,
                false
            }
        );
        ModalContents.push_back(
            {
                "Surprise math question!",
                "To confirm that you're really 18, let's do a maths question! \r\nWhat is 6 + 9 * (4 - 2) + 0?",
                IncludedAssets::Chocola_Laugh_png,
                "No",
                "Yes",
                true,
                true,
                true,
                true
            }
        );
        ModalContents.push_back(
            {
                "Correct!",
                "If you got that right then you must be a smart and sensible adult!",
                IncludedAssets::Chocola_Happy_png,
                "but I'm not...",
                "I am!",
                true,
                true,
                true,
                false
            }
        );
        ModalContents.push_back(
            {
                "Wait!",
                "The NSFW features could be dangerous! \r\nWhy else would they be called \"Not Safe For Work??\"",
                IncludedAssets::Chocola_Spooked_png,
                "That sounds risky!",
                "I am prepared",
                true,
                true,
                true,
                false
            }
        );

        ModalContents.push_back(
            {
                "Last time I'll ask",
                "So you definitely want to enable the NSFW features and suffer the consequences which may entail from it?",
                IncludedAssets::Chocola_Bashful_png,
                "No",
                "Yes",
                true,
                true,
                true,
                false
            }
        );
}

void Nya::UI::Modals::NSFWConsent::ctor() {
}

void Nya::UI::Modals::NSFWConsent::Awake() {
    {
       InitModalContents();
    }

    this->modal = BSML::Lite::CreateModal(get_transform(), {0,0},{95, 70}, nullptr);


    // Root component
    auto* rootVertical = BSML::Lite::CreateVerticalLayoutGroup(this->modal->get_transform());
    auto rootVerticalElement = rootVertical->GetComponent<UnityEngine::UI::LayoutElement*>();
    rootVertical->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    rootVerticalElement->set_preferredWidth(92);

    {        
        // Create main layout
        mainLayout = BSML::Lite::CreateVerticalLayoutGroup(rootVertical->get_transform());
        // mainLayout->get_gameObject()->set_active(false);
        // Create top text
        {   
            auto vert = BSML::Lite::CreateVerticalLayoutGroup(mainLayout->get_transform());
            vert->set_padding(RectOffset::New_ctor(2, 0, 0, 0));
            topText = BSML::Lite::CreateText(mainLayout->get_transform(), "TopText", false);
            topText->set_fontSize(8.2f);
            topText->set_fontStyle(TMPro::FontStyles::Underline);
            topText->set_alignment(TMPro::TextAlignmentOptions::Center);
        }
        {
            // Create horizontal layout for mid text and image
            auto hor = BSML::Lite::CreateHorizontalLayoutGroup(mainLayout->get_transform());
            auto fitter = hor->GetComponent<UnityEngine::UI::ContentSizeFitter*>();
            fitter->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

            {
                // Create mid text layout
                auto midTextLayout = BSML::Lite::CreateVerticalLayoutGroup(hor->get_transform());
                midTextLayout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(45);

                // Create mid text
                midText = BSML::Lite::CreateText(midTextLayout->get_transform(), "MidText", false);
                midText->set_fontSize(5.0f);
                midText->set_alignment(TMPro::TextAlignmentOptions::Center);
                midText->set_enableWordWrapping(true);
            }

            // Create mid image
            midImage = BSML::Lite::CreateImage(hor->get_transform(), nullptr, {0, 0}, {40, 40});
            midImage->set_preserveAspect(true);
            // TODO: Remove later
            midImage->set_sprite(BSML::Lite::ArrayToSprite(IncludedAssets::Vanilla_Horny_Pastry_Puffer_png));
            midImage->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(40);
            midImage->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(40);

        }
        {
            // Create buttons layout
            buttonsLayout = BSML::Lite::CreateHorizontalLayoutGroup(mainLayout->get_transform());
            // Create no button
            noButton = BSML::Lite::CreateUIButton(buttonsLayout->get_transform(), "No", [this](){
                this->Hide();
            });
            noButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(34);
            noButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);

            // Create yes button
            yesButton = BSML::Lite::CreateUIButton(buttonsLayout->get_transform(), "Yes", [this](){
                ConfirmationStage += 1;
                UpdateModalContent();
            });
            yesButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(34);
            yesButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);
        }
        {
            // Create slider layout
            sliderLayout = BSML::Lite::CreateHorizontalLayoutGroup(mainLayout->get_transform());
            sliderLayout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(80);

            // Create slider
            slider = BSML::Lite::CreateSliderSetting(sliderLayout->get_transform(), "Value", 1.0f, 1.0f, 0.0f, 60.0f, [this](float value){});
            slider->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(80);
            
            // create submit button
            submitButton = BSML::Lite::CreateUIButton(sliderLayout->get_transform(), "Submit", [this](){
                if (slider->get_Value() == 24.0f) {
                    ConfirmationStage += 1;
                    UpdateModalContent();
                } else {
                    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                        this->FadeoutModal(FadeOutContent::IncorrectMath)
                    ));
                }
            });
            submitButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(25);
            submitButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);
        }
    }
    {
        // Create nsfw layout lel
        hornyPastryPufferLayout = BSML::Lite::CreateHorizontalLayoutGroup(rootVertical->get_transform());
        auto hornyPastryPufferElement = hornyPastryPufferLayout->GetComponent<UnityEngine::UI::LayoutElement*>();
        hornyPastryPufferElement->set_preferredWidth(92);
        hornyPastryPufferElement->set_preferredHeight(70);
        hornyPastryPufferElement->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        auto hornyImageView = BSML::Lite::CreateImage(hornyPastryPufferLayout->get_transform(), nullptr, {0, 0}, {0, 0});
        hornyImageView->set_preserveAspect(true);
        hornyImageView->set_sprite(BSML::Lite::ArrayToSprite(IncludedAssets::Vanilla_Horny_Pastry_Puffer_png));
        hornyPastryPufferLayout->get_gameObject()->set_active(false);
    }
}

void Nya::UI::Modals::NSFWConsent::Show() {

    BSML::MainThreadScheduler::Schedule([this] {
        mainLayout->get_gameObject()->set_active(true);
        hornyPastryPufferLayout->get_gameObject()->set_active(false);
        buttonsLayout->get_gameObject()->set_active(true);
        sliderLayout->get_gameObject()->set_active(false);
        ConfirmationStage = 0;

        UpdateModalContent();

        this->modal->Show();
    });
    

}

void Nya::UI::Modals::NSFWConsent::ChangeModalContent(ModalContent& modalContent) {
    // Scheduler
    BSML::MainThreadScheduler::Schedule([this, modalContent] {
        topText->set_text(modalContent.TopText);
        midText->set_text(modalContent.MidText);
        midImage->set_sprite(BSML::Lite::ArrayToSprite(modalContent.MidImage));

        if (modalContent.ShowInputs) {
            if (modalContent.MathTime) {

                buttonsLayout->get_gameObject()->set_active(false);
                sliderLayout->get_gameObject()->set_active(true);

                slider->set_Value(0.0f);

                if (modalContent.ButtonIntractabilityCooldown) {
                    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                        this->InteractabilityCooldown(slider)
                    ));
                    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                        this->InteractabilityCooldown(submitButton)
                    ));
                } else {
                    submitButton->set_interactable(true);
                }
            } else {
                    
                buttonsLayout->get_gameObject()->set_active(true);
                sliderLayout->get_gameObject()->set_active(false);

                noButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_text(modalContent.NoButtonText);
                yesButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->set_text(modalContent.YesButtonText);
                
                if (modalContent.ButtonIntractabilityCooldown) {
                    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                        this->InteractabilityCooldown(yesButton)
                    )); 
                    this->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(
                        this->InteractabilityCooldown(noButton)
                    ));
                } else {
                    yesButton->set_interactable(true);
                    noButton->set_interactable(true);
                }
            }
        }
    });

    
}

void Nya::UI::Modals::NSFWConsent::Hide() {
    BSML::MainThreadScheduler::Schedule([this] {
        this->modal->Hide();
    });
}

bool Nya::UI::Modals::NSFWConsent::isShown() {
    return this->modal->_isShown;
}
