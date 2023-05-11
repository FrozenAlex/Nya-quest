#include "UI/Modals/NSFWConsent.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "assets.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
DEFINE_TYPE(Nya, NSFWConsent);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI::BeatSaberUI;
using namespace std;

#define coro(coroutine) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(coroutine))

custom_types::Helpers::Coroutine Nya::NSFWConsent::InteractabilityCooldown(SliderSetting * setting) {
    setting->slider->set_interactable(false);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2.0f));
    setting->slider->set_interactable(true);
    co_return;
} 

custom_types::Helpers::Coroutine Nya::NSFWConsent::InteractabilityCooldown(Button * button) {
    button->set_interactable(false);
    co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2.0f));
    button->set_interactable(true);
    
    co_return;
} 



custom_types::Helpers::Coroutine Nya::NSFWConsent::FadeoutModal(FadeOutContent content) { 
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
            modal->Hide(true, nullptr);

            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuTransitionsHelper*>()[0]->RestartGame(nullptr);
        }
        break;
    case FadeOutContent::IncorrectMath:
        {
            mainLayout->get_gameObject()->SetActive(false);
            hornyPastryPufferLayout->get_gameObject()->SetActive(false);
            // Wait for 2 seconds
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2));
            modal->Hide(true, nullptr);
        }
        break;
    
    case FadeOutContent::Blank:
        {
            mainLayout->get_gameObject()->SetActive(false);
            hornyPastryPufferLayout->get_gameObject()->SetActive(false);
            // Wait for 2 seconds
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(2));
            modal->Hide(true, nullptr);
        }
        break;
    
    default:
        break;
    }


}

void Nya::NSFWConsent::UpdateModalContent() {
    if (ConfirmationStage >= ModalContents.size()) {

        // Enable nsfw
        getNyaConfig().NSFWUI.SetValue(true);
        
        
        coro(this->FadeoutModal(FadeOutContent::HornyPastryPuffer));

        return;
    }

    auto& modalContent = ModalContents[ConfirmationStage];
    ChangeModalContent(modalContent);
}

void Nya::NSFWConsent::InitModalContents() {
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

void Nya::NSFWConsent::ctor() {
    {
       InitModalContents();
    }

    this->modal = CreateModal(get_transform(), {95, 70}, nullptr);


    // Root component
    auto* rootVertical = CreateVerticalLayoutGroup(this->modal->get_transform());
    auto rootVerticalElement = rootVertical->GetComponent<UnityEngine::UI::LayoutElement*>();
    rootVertical->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    rootVerticalElement->set_preferredWidth(92);

    {        
        // Create main layout
        mainLayout = CreateVerticalLayoutGroup(rootVertical->get_transform());
        // mainLayout->get_gameObject()->set_active(false);
        // Create top text
        {   
            auto vert = CreateVerticalLayoutGroup(mainLayout->get_transform());
            vert->set_padding(RectOffset::New_ctor(2, 0, 0, 0));
            topText = CreateText(mainLayout->get_transform(), to_utf16("TopText"), false);
            topText->set_fontSize(8.2f);
            topText->set_fontStyle(TMPro::FontStyles::Underline);
            topText->set_alignment(TMPro::TextAlignmentOptions::Center);
        }
        {
            // Create horizontal layout for mid text and image
            auto hor = CreateHorizontalLayoutGroup(mainLayout->get_transform());
            auto fitter = hor->GetComponent<UnityEngine::UI::ContentSizeFitter*>();
            fitter->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

            {
                // Create mid text layout
                auto midTextLayout = CreateVerticalLayoutGroup(hor->get_transform());
                midTextLayout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(45);

                // Create mid text
                midText = CreateText(midTextLayout->get_transform(), to_utf16("MidText"), false);
                midText->set_fontSize(5.0f);
                midText->set_alignment(TMPro::TextAlignmentOptions::Center);
                midText->set_enableWordWrapping(true);
            }

            // Create mid image
            midImage = CreateImage(hor->get_transform(), nullptr, {0, 0}, {40, 40});
            midImage->set_preserveAspect(true);
            // TODO: Remove later
            midImage->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::Vanilla_Horny_Pastry_Puffer_png));
            midImage->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(40);
            midImage->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(40);

        }
        {
            // Create buttons layout
            buttonsLayout = CreateHorizontalLayoutGroup(mainLayout->get_transform());
            // Create no button
            noButton = CreateUIButton(buttonsLayout->get_transform(), "No", [this](){
                this->Hide();
            });
            noButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(34);
            noButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);

            // Create yes button
            yesButton = CreateUIButton(buttonsLayout->get_transform(), "Yes", [this](){
                ConfirmationStage += 1;
                UpdateModalContent();
            });
            yesButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(34);
            yesButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);
        }
        {
            // Create slider layout
            sliderLayout = CreateHorizontalLayoutGroup(mainLayout->get_transform());
            sliderLayout->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(80);

            // Create slider
            slider = CreateSliderSetting(sliderLayout->get_transform(), "Value", 1.0f, 1.0f, 0.0f, 60.0f, [this](float value){});
            slider->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(80);
            
            // create submit button
            submitButton = CreateUIButton(sliderLayout->get_transform(), "Submit", [this](){
                if (slider->get_value() == 24.0f) {
                    ConfirmationStage += 1;
                    UpdateModalContent();
                } else {
                    coro(this->FadeoutModal(FadeOutContent::IncorrectMath));
                }
            });
            submitButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(25);
            submitButton->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(10);
        }
    }

    {
        // Create nsfw layout lel
        hornyPastryPufferLayout = CreateHorizontalLayoutGroup(rootVertical->get_transform());
        auto hornyPastryPufferElement = hornyPastryPufferLayout->GetComponent<UnityEngine::UI::LayoutElement*>();
        hornyPastryPufferElement->set_preferredWidth(92);
        hornyPastryPufferElement->set_preferredHeight(70);
        hornyPastryPufferElement->GetComponent<UnityEngine::UI::ContentSizeFitter*>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);

        auto hornyImageView = CreateImage(hornyPastryPufferLayout->get_transform(), nullptr, {0, 0}, {0, 0});
        hornyImageView->set_preserveAspect(true);
        hornyImageView->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::Vanilla_Horny_Pastry_Puffer_png));
        hornyPastryPufferLayout->get_gameObject()->set_active(false);
    }
    
}

void Nya::NSFWConsent::Show() {

    QuestUI::MainThreadScheduler::Schedule([this] {
        mainLayout->get_gameObject()->set_active(true);
        hornyPastryPufferLayout->get_gameObject()->set_active(false);
        buttonsLayout->get_gameObject()->set_active(true);
        sliderLayout->get_gameObject()->set_active(false);
        ConfirmationStage = 0;

        UpdateModalContent();

        this->modal->Show(true, true, nullptr);
    });
    

}

void Nya::NSFWConsent::ChangeModalContent(ModalContent& modalContent) {
    // Scheduler
    QuestUI::MainThreadScheduler::Schedule([this, modalContent] {
        topText->SetText(modalContent.TopText);
        midText->SetText(modalContent.MidText);
        midImage->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(modalContent.MidImage));

        if (modalContent.ShowInputs) {
            if (modalContent.MathTime) {

                buttonsLayout->get_gameObject()->set_active(false);
                sliderLayout->get_gameObject()->set_active(true);

                slider->set_value(0.0f);

                if (modalContent.ButtonIntractabilityCooldown) {
                    coro(this->InteractabilityCooldown(slider));
                    coro(this->InteractabilityCooldown(submitButton));
                } else {
                    submitButton->set_interactable(true);
                }
            } else {
                    
                buttonsLayout->get_gameObject()->set_active(true);
                sliderLayout->get_gameObject()->set_active(false);

                noButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->SetText(modalContent.NoButtonText);
                yesButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>()->SetText(modalContent.YesButtonText);
                
                if (modalContent.ButtonIntractabilityCooldown) {
                    coro(this->InteractabilityCooldown(yesButton));
                    coro(this->InteractabilityCooldown(noButton));
                } else {
                    yesButton->set_interactable(true);
                    noButton->set_interactable(true);
                }
            }
        }
    });

    
}

void Nya::NSFWConsent::Hide() {
    QuestUI::MainThreadScheduler::Schedule([this] {
        this->modal->Hide(true, nullptr);
    });
}

bool Nya::NSFWConsent::isShown() {
    return this->modal->isShown;
}