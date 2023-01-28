#include "SettingsMenu.hpp"
#include "Utils/Utils.hpp"
#include "NyaFloatingUI.hpp"
#include "HMUI/ScrollView.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
DEFINE_TYPE(Nya, SettingsMenu);

static ConstString SourcesSettingsWrapper("SourcesSettingsWrapper");
static ConstString SettingsMenuWrapper("SettingsMenuWrapper");
static ConstString FloatingSettingsWrapper("FloatingSettingsWrapper");
namespace Nya
{
    // Constructor
    void SettingsMenu::ctor()
    {

        // Init modal
        this->settingsModal = QuestUI::BeatSaberUI::CreateModal(get_transform(), {65, 60}, nullptr);
        this->settingsModal->get_gameObject()->set_name(SettingsMenuWrapper);

        auto sourcesView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform *>();
        sourcesView->get_gameObject()->set_name(SourcesSettingsWrapper);
        sourcesView->SetParent(this->settingsModal->get_transform(), false);

        auto floatingView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform *>();
        floatingView->get_gameObject()->set_name(FloatingSettingsWrapper);
        floatingView->SetParent(this->settingsModal->get_transform(), false);

        // Setup canvas
        auto canvas = QuestUI::BeatSaberUI::CreateCanvas();
        canvas->get_transform()->SetParent(this->settingsModal->get_transform(), false);
        auto controlRect = reinterpret_cast<UnityEngine::RectTransform *>(canvas->get_transform());
        controlRect->set_anchoredPosition({0, 0});

        controlRect->set_anchorMin(UnityEngine::Vector2(0.0f, 0.0f));
        controlRect->set_anchorMax(UnityEngine::Vector2(1.0f, 1.0f));

        controlRect->set_sizeDelta({50, 8.5});
        controlRect->set_localScale({1, 1, 1});
        // canvas->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredWidth(60.0);
        // canvas->GetComponent<UnityEngine::UI::LayoutElement*>()->set_preferredHeight(60.0);

        // Create tabs control
        ArrayW<StringW> options(2);
        options[0] = "sources";
        options[1] = "floating";
        this->tabsSwitch = Nya::Utils::CreateTextSegmentedControl(
            controlRect->get_transform(),
            {0, -5.5},
            {45, 5.5},
            options,
            std::bind(
                &SettingsMenu::SwitchTab,
                this,
                std::placeholders::_1));

        // Create a text that says "Hello World!" and set the parent to the container.
        UnityEngine::UI::VerticalLayoutGroup *sourcesViewLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(sourcesView);
        sourcesViewLayout->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        sourcesViewLayout->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
        sourcesViewLayout->GetComponent<UnityEngine::UI::LayoutElement *>()->set_preferredWidth(60.0);

        // TMPro::TextMeshProUGUI* title = QuestUI::BeatSaberUI::CreateText(vert->get_transform(), "Settings");
        // title->GetComponent<TMPro::TMP_Text*>()->set_alignment(TMPro::TextAlignmentOptions::Center);
        // title->GetComponent<TMPro::TMP_Text*>()->set_fontSize(7.0);

        // Get platform helper for scrolling
        auto platformHelper = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelCollectionTableView*>().First()->GetComponentInChildren<HMUI::ScrollView*>()->platformHelper;

        // Nya configuration
        {
            // API Selection (nothing to select for now)
            std::string API = getNyaConfig().API.GetValue();
            this->api_switch = QuestUI::BeatSaberUI::CreateDropdown(sourcesViewLayout->get_transform(), to_utf16("API"), "Loading..", {"Loading.."}, [this](StringW value)
                {
                    getNyaConfig().API.SetValue(value);

                    auto source = NyaAPI::get_data_source(value);

                    if (source->SfwEndpoints.size() == 0)
                    {
                        this->sfw_endpoint->button->set_interactable(false);
                        // Reset the view
                        this->sfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                        this->sfw_endpoint->SelectCellWithIdx(0);
                    }
                    else
                    {
                        this->sfw_endpoint->button->set_interactable(true);
                        auto sfwList = NyaAPI::listEndpointLabels(&source->SfwEndpoints);

                        // SFW endpoints
                        this->sfw_endpoint->SetTexts(sfwList->i_IReadOnlyList_1_T());

                        std::string endpoint_sfw = EndpointConfig::getEndpointValue(getNyaConfig().config, value, false);
                        int sfw_index = -1;
                        if (endpoint_sfw != "")
                        {
                            sfw_index = NyaAPI::findSourceIndexInListC(&source->SfwEndpoints, endpoint_sfw);
                        }

                        if (sfw_index >= 0)
                        {
                            this->sfw_endpoint->SelectCellWithIdx(sfw_index);
                        }
                    }

                if (getNyaConfig().NSFWUI.GetValue()) {
                    // Restore nsfw state
                    if (source->NsfwEndpoints.size() == 0)
                    {
                        this->nsfw_endpoint->button->set_interactable(false);
                        // Reset the view
                        this->nsfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                        this->nsfw_endpoint->SelectCellWithIdx(0);
                    }
                    else
                    {
                        this->nsfw_endpoint->button->set_interactable(true);
                        auto nsfwList = NyaAPI::listEndpointLabels(&source->NsfwEndpoints);

                        this->nsfw_endpoint->SetTexts(nsfwList->i_IReadOnlyList_1_T());

                        std::string endpoint_nsfw = EndpointConfig::getEndpointValue(getNyaConfig().config, value, true);
                        int nsfw_index = -1;
                        if (endpoint_nsfw != "")
                        {
                            nsfw_index = NyaAPI::findSourceIndexInListC(&source->NsfwEndpoints, endpoint_nsfw);
                        }

                        if (nsfw_index >= 0)
                        {
                            this->nsfw_endpoint->SelectCellWithIdx(nsfw_index);
                        }
                    }

                    this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
                }
            });

            // Add scrolling
            if (platformHelper != nullptr) {
                this->api_switch->tableView->scrollView->platformHelper = platformHelper;
            }

            // SFW endpoint switch
            this->sfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(sourcesViewLayout->get_transform(), to_utf16("SFW endpoint"), "Loading..", {"Loading.."}, [](StringW value)
                                                                      {
                std::string API = getNyaConfig().API.GetValue();
                
                EndpointConfig::updateEndpointValue(getNyaConfig().config, API, false, value); });

            // Add scrolling
            if (platformHelper != nullptr) {
                this->sfw_endpoint->tableView->scrollView->platformHelper = platformHelper;
            }

            if (getNyaConfig().NSFWUI.GetValue()) {
                // NSFW endpoint selector
                this->nsfw_endpoint = QuestUI::BeatSaberUI::CreateDropdown(
                    sourcesViewLayout->get_transform(),
                    to_utf16("NSFW endpoint"),
                    "Loading..",
                    {"Loading.."},
                    [](StringW value)
                    {
                        // Get current endpoint
                        std::string API = getNyaConfig().API.GetValue();
                        EndpointConfig::updateEndpointValue(getNyaConfig().config, API, true, value); 
                    }
                );

                // NSFW toggle
                bool NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
                this->nsfw_toggle = QuestUI::BeatSaberUI::CreateToggle(sourcesViewLayout->get_transform(), to_utf16("NSFW toggle"), NSFWEnabled, [](bool isChecked)
                                                                    { getNyaConfig().NSFWEnabled.SetValue(isChecked); });
                // Add scrolling
                if (platformHelper != nullptr) {
                    this->nsfw_endpoint->tableView->scrollView->platformHelper = platformHelper;
                }
            }

            UnityEngine::UI::HorizontalLayoutGroup *horz = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->set_spacing(10);
            {
                this->downloadButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Download Nya"), "PracticeButton",
                    [this](){
                        auto imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView *>();
                        imageView->SaveImage();
                        this->downloadButton->set_interactable(false);
                        this->settingsModal->Hide(true, nullptr);
                });

                UnityEngine::UI::Button *closeButton = QuestUI::BeatSaberUI::CreateUIButton(horz->get_transform(), to_utf16("Close"), "PracticeButton",
                [this]()
                {
                    this->settingsModal->Hide(true, nullptr);
                });
            }
            UnityEngine::UI::HorizontalLayoutGroup *horz2 = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            horz->set_spacing(10);

            {
                autoNyaButton = QuestUI::BeatSaberUI::CreateToggle(horz2, "AutoNya",  getNyaConfig().AutoNya.GetValue(), [this](bool value){
                    getNyaConfig().AutoNya.SetValue(value);
                    if (value) {
                        NyaUtils::ImageView* imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView *>();
                        if (imageView) {
                            imageView->OnEnable();
                        }
                    }
                });
                
            }
            
        }

        {
            UnityEngine::UI::VerticalLayoutGroup *floatingViewLayout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(floatingView);
            floatingViewLayout->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            floatingViewLayout->GetComponent<UnityEngine::UI::ContentSizeFitter *>()->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
            floatingViewLayout->GetComponent<UnityEngine::UI::LayoutElement *>()->set_preferredWidth(60.0);
            UnityEngine::UI::Button *faceHeadset = QuestUI::BeatSaberUI::CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Face headset"), "PracticeButton",
                                                                                        [this]()
                                                                                        {
                                                                                            if (Main::NyaFloatingUI != nullptr)
                                                                                            {
                                                                                                Main::NyaFloatingUI->hoverClickHelper->LookAtCamera();
                                                                                            }
                                                                                        });
            QuestUI::BeatSaberUI::CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Set upright"), "PracticeButton",
                                                 [this]()
                                                 {
                                                     if (Main::NyaFloatingUI != nullptr)
                                                     {
                                                         Main::NyaFloatingUI->hoverClickHelper->SetUpRight();
                                                     }
                                                 });
            QuestUI::BeatSaberUI::CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Default position"), "PracticeButton",
                                                 [this]()
                                                 {
                                                     if (Main::NyaFloatingUI != nullptr)
                                                     {
                                                         Main::NyaFloatingUI->SetDefaultPos();
                                                     }
                                                 });

            UnityEngine::UI::Button *closeButton = QuestUI::BeatSaberUI::CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Close"), "PracticeButton",
                                                                                        [this]()
                                                                                        {
                                                                                            this->settingsModal->Hide(true, nullptr);
                                                                                        });

            QuestUI::BeatSaberUI::CreateToggle(floatingViewLayout->get_transform(), "Show handle",
                                               getNyaConfig().ShowHandle.GetValue(),
                                               [](bool value)
                                               {
                                                   getNyaConfig().ShowHandle.SetValue(value);
                                                   if (
                                                       Main::NyaFloatingUI != nullptr &&
                                                       Main::NyaFloatingUI->UIScreen != nullptr)
                                                   {
                                                       Main::NyaFloatingUI->UpdateHandleVisibility();
                                                   }
                                               });

            auto slider = QuestUI::BeatSaberUI::CreateSliderSetting(floatingViewLayout->get_transform(), "Floating Screen Scale", 0.1f,
                                                                    getNyaConfig().FloatingScreenScale.GetValue(), 0.1f, 2.0f, [](float value)
                                                                    {
                    getNyaConfig().FloatingScreenScale.SetValue(value);
                    if (Main::NyaFloatingUI != nullptr) {
                        Main::NyaFloatingUI->UpdateScale();
                    } }

            );
        }
    }

    bool SettingsMenu::isShown()
    {
        if (this->settingsModal == nullptr)
            return false;
        return this->settingsModal->isShown;
    }

    void SettingsMenu::SwitchTab(int idx)
    {

        this->settingsModal->get_transform()->FindChild(SourcesSettingsWrapper)->get_gameObject()->SetActive(idx == 0);
        this->settingsModal->get_transform()->FindChild(FloatingSettingsWrapper)->get_gameObject()->SetActive(idx == 1);
    }

    void SettingsMenu::Show()
    {
        INFO("Settings button clicked");
        auto imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView *>();
        this->downloadButton->set_interactable(imageView->HasImageToSave());

        // Run UI on the main thread
        QuestUI::MainThreadScheduler::Schedule([this]
                                               {
            this->tabsSwitch->segmentedControl->SelectCellWithNumber(0);
            // Autonya
            autoNyaButton->set_isOn(getNyaConfig().AutoNya.GetValue());
            
            std::string API = getNyaConfig().API.GetValue();
            SourceData* source = nullptr;

            // Catch for invalid apis
            try
            {
                source = NyaAPI::get_data_source(API);
            }
            catch(...)
            {
                API = "waifu.pics";
                getNyaConfig().API.SetValue(API);
                source = NyaAPI::get_data_source(API);
            }

            auto sources = Nya::Utils::vectorToList(NyaAPI::get_source_list());

            this->api_switch->SetTexts(reinterpret_cast<System::Collections::Generic::IReadOnlyList_1<StringW>*>(sources));
        
            int index = Nya::Utils::findStrIndexInList(sources, API);
            if (index != -1) {
                this->api_switch->SelectCellWithIdx(index);
            }
            
            // Set SFW picker
            if (source->SfwEndpoints.size() == 0) {
                this->sfw_endpoint->button->set_interactable(false);
                // Reset the view
                this->sfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                this->sfw_endpoint->SelectCellWithIdx(0); 
            } else {
                this->sfw_endpoint->button->set_interactable(true);
                auto sfwList = NyaAPI::listEndpointLabels(&source->SfwEndpoints);

                // SFW endpoints
                this->sfw_endpoint->SetTexts(sfwList->i_IReadOnlyList_1_T());
                
                std::string endpoint_sfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, false);
                int sfw_index = -1;
                if (endpoint_sfw != "") {
                    sfw_index = NyaAPI::findSourceIndexInListC(&source->SfwEndpoints ,endpoint_sfw);
                }

                if (sfw_index >= 0) {
                    this->sfw_endpoint->SelectCellWithIdx(sfw_index); 
                }
            }

#ifdef NSFW
            if (getNyaConfig().NSFWUI.GetValue()) {
                // Restore nsfw state
                if (source->NsfwEndpoints.size() == 0) {
                    this->nsfw_endpoint->button->set_interactable(false);
                    // Reset the view
                    this->nsfw_endpoint->SetTexts(List<StringW>::New_ctor()->i_IReadOnlyList_1_T());
                    this->nsfw_endpoint->SelectCellWithIdx(0); 
                } else {
                    this->nsfw_endpoint->button->set_interactable(true);
                    auto nsfwList = NyaAPI::listEndpointLabels(&source->NsfwEndpoints);

                    this->nsfw_endpoint->SetTexts(nsfwList->i_IReadOnlyList_1_T());

                    std::string endpoint_nsfw = EndpointConfig::getEndpointValue(getNyaConfig().config, API, true);
                    int nsfw_index = -1;
                    if (endpoint_nsfw != "") {
                        nsfw_index = NyaAPI::findSourceIndexInListC(&source->NsfwEndpoints, endpoint_nsfw);
                    }
                    
                    if (nsfw_index != -1) {
                        nsfw_endpoint->SelectCellWithIdx(nsfw_index); 
                    }
                }

                this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
            }
#endif
            
            this->SwitchTab(0);
            settingsModal->Show(true, true, nullptr); });
    }

    void SettingsMenu::Hide()
    {
        this->settingsModal->Hide(true, nullptr);
    }

}