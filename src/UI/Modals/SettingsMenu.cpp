#include "UI/Modals/SettingsMenu.hpp"
#include "GlobalNamespace/LevelCollectionTableView.hpp"
#include "HMUI/ScrollView.hpp"
#include "NyaFloatingUI.hpp"
#include "Utils/Utils.hpp"
DEFINE_TYPE(Nya, SettingsMenu);

static ConstString SourcesSettingsWrapper("SourcesSettingsWrapper");
static ConstString SettingsMenuWrapper("SettingsMenuWrapper");
static ConstString FloatingSettingsWrapper("FloatingSettingsWrapper");

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI::BeatSaberUI;
using namespace std;

namespace Nya {
// Constructor
void SettingsMenu::ctor() {
    // Init modal
    this->settingsModal = CreateModal(get_transform(), {65, 60}, nullptr);
    this->settingsModal->get_gameObject()->set_name(SettingsMenuWrapper);

    auto sourcesView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform*>();
    sourcesView->get_gameObject()->set_name(SourcesSettingsWrapper);
    sourcesView->SetParent(this->settingsModal->get_transform(), false);

    auto floatingView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform*>();
    floatingView->get_gameObject()->set_name(FloatingSettingsWrapper);
    floatingView->SetParent(this->settingsModal->get_transform(), false);

    // Setup canvas
    auto canvas = CreateCanvas();
    canvas->get_transform()->SetParent(this->settingsModal->get_transform(), false);
    auto controlRect = reinterpret_cast<UnityEngine::RectTransform*>(canvas->get_transform());
    controlRect->set_anchoredPosition({0, 0});

    controlRect->set_anchorMin(UnityEngine::Vector2(0.0f, 0.0f));
    controlRect->set_anchorMax(UnityEngine::Vector2(1.0f, 1.0f));

    controlRect->set_sizeDelta({50, 8.5});
    controlRect->set_localScale({1, 1, 1});
    // canvas->GetComponent<LayoutElement*>()->set_preferredWidth(60.0);
    // canvas->GetComponent<LayoutElement*>()->set_preferredHeight(60.0);



    // Create tabs control
    ArrayW<StringW> options(2);
    options[0] = "sources";
    options[1] = "floating";
    this->tabsSwitch =
        QuestUI::BeatSaberUI::CreateTextSegmentedControl(controlRect->get_transform(), {0, -5.5}, {45, 5.5}, options,
                                               bind(&SettingsMenu::SwitchTab, this, placeholders::_1));

    // Create a text that says "Hello World!" and set the parent to the container.
    VerticalLayoutGroup* sourcesViewLayout = CreateVerticalLayoutGroup(sourcesView);
    sourcesViewLayout->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
    sourcesViewLayout->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    sourcesViewLayout->GetComponent<LayoutElement*>()->set_preferredWidth(60.0);

    // TMPro::TextMeshProUGUI* title = CreateText(vert->get_transform(), "Settings");
    // title->GetComponent<TMPro::TMP_Text*>()->set_alignment(TMPro::TextAlignmentOptions::Center);
    // title->GetComponent<TMPro::TMP_Text*>()->set_fontSize(7.0);

    // Get platform helper for scrolling
    auto platformHelper = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelCollectionTableView*>()
                              .First()
                              ->GetComponentInChildren<HMUI::ScrollView*>()
                              ->platformHelper;

    // Nya configuration
    {
        // API Selection (nothing to select for now)
        string API = getNyaConfig().API.GetValue();
        this->api_switch = CreateDropdown(sourcesViewLayout->get_transform(), to_utf16("API"), "Loading..",
                                          {"Loading.."}, [this](StringW value) {
                                              // Change the API in the config
                                              getNyaConfig().API.SetValue(value);

                                              // Get this source
                                              auto source = NyaAPI::get_data_source(value);

                                              this->selectedDataSource = source;
                                              this->selectedDataSourceName = std::string(value);

                                              this->UpdateEndpointLists();
                                          });

        // Add scrolling
        if (platformHelper != nullptr) {
            this->api_switch->tableView->scrollView->platformHelper = platformHelper;
        }

        // SFW endpoint switch
        this->sfw_endpoint =
            CreateDropdown(sourcesViewLayout->get_transform(), to_utf16("SFW endpoint"), "Loading..", {"Loading.."},
                           [this](StringW value) {
                               // Get current endpoint
                               string API = this->selectedDataSourceName;

                               // find url
                               int index = this->sfw_endpoint->selectedIndex;
                               StringW url = this->sfw_endpoint_urls->get_Item(index);

                               EndpointConfig::updateEndpointValue(getNyaConfig().config, API, false, url);
                           });

        // Add scrolling
        if (platformHelper != nullptr) {
            this->sfw_endpoint->tableView->scrollView->platformHelper = platformHelper;
        }

        if (getNyaConfig().NSFWUI.GetValue()) {
            // NSFW endpoint selector
            this->nsfw_endpoint =
                CreateDropdown(sourcesViewLayout->get_transform(), to_utf16("NSFW endpoint"), "Loading..",
                               {"Loading.."}, [this](StringW value) {
                                   // Get current endpoint
                                   string API = this->selectedDataSourceName;

                                   // find url
                                   int index = this->nsfw_endpoint->selectedIndex;
                                   StringW url = this->nsfw_endpoint_urls->get_Item(index);

                                   // Change the endpoint in the config
                                   EndpointConfig::updateEndpointValue(getNyaConfig().config, API, true, url);
                               });

            // NSFW toggle
            bool NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
            this->nsfw_toggle = CreateToggle(sourcesViewLayout->get_transform(), to_utf16("NSFW toggle"), NSFWEnabled,
                                             [](bool isChecked) { getNyaConfig().NSFWEnabled.SetValue(isChecked); });
            // Add scrolling
            if (platformHelper != nullptr) {
                this->nsfw_endpoint->tableView->scrollView->platformHelper = platformHelper;
            }
        }

        HorizontalLayoutGroup* horz = CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
        horz->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);
        {
            this->downloadButton =
                CreateUIButton(horz->get_transform(), to_utf16("Download Nya"), "PracticeButton", [this]() {
                    auto imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView*>();
                    imageView->SaveImage();
                    this->downloadButton->set_interactable(false);
                    this->settingsModal->Hide(true, nullptr);
                });

            Button* closeButton = CreateUIButton(horz->get_transform(), to_utf16("Close"), "PracticeButton",
                                                 [this]() { this->settingsModal->Hide(true, nullptr); });
        }
        HorizontalLayoutGroup* horz2 = CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
        horz->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);

        {
            autoNyaButton = CreateToggle(horz2, "AutoNya", getNyaConfig().AutoNya.GetValue(), [this](bool value) {
                getNyaConfig().AutoNya.SetValue(value);
                if (value) {
                    NyaUtils::ImageView* imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView*>();
                    if (imageView) {
                        imageView->OnEnable();
                    }
                }
            });
        }
    }

    {
        VerticalLayoutGroup* floatingViewLayout = CreateVerticalLayoutGroup(floatingView);
        floatingViewLayout->GetComponent<ContentSizeFitter*>()->set_verticalFit(
            ContentSizeFitter::FitMode::PreferredSize);
        floatingViewLayout->GetComponent<ContentSizeFitter*>()->set_horizontalFit(
            ContentSizeFitter::FitMode::PreferredSize);
        floatingViewLayout->GetComponent<LayoutElement*>()->set_preferredWidth(60.0);
        {
            auto* hor = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(floatingViewLayout->get_transform());

            Button* faceHeadset =
            CreateUIButton(hor->get_transform(), to_utf16("Face headset"), "PracticeButton", [this]() {
                if (Main::NyaFloatingUI != nullptr) {
                    Main::NyaFloatingUI->hoverClickHelper->LookAtCamera();
                }
            });
            CreateUIButton(hor->get_transform(), to_utf16("Set upright"), "PracticeButton", [this]() {
                if (Main::NyaFloatingUI != nullptr) {
                    Main::NyaFloatingUI->hoverClickHelper->SetUpRight();
                }
            });
        }
        CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Default position"), "PracticeButton", [this]() {
            if (Main::NyaFloatingUI != nullptr) {
                Main::NyaFloatingUI->SetDefaultPos();
            }
        });

        Button* closeButton = CreateUIButton(floatingViewLayout->get_transform(), to_utf16("Close"), "PracticeButton",
                                             [this]() { this->settingsModal->Hide(true, nullptr); });

        CreateToggle(floatingViewLayout->get_transform(), "Show handle", getNyaConfig().ShowHandle.GetValue(),
                     [](bool value) {
                         getNyaConfig().ShowHandle.SetValue(value);
                         if (Main::NyaFloatingUI != nullptr && Main::NyaFloatingUI->UIScreen != nullptr) {
                             Main::NyaFloatingUI->UpdateHandleVisibility();
                         }
                     });

        auto slider = CreateSliderSetting(floatingViewLayout->get_transform(), "Floating Screen Scale", 0.1f,
            getNyaConfig().FloatingScreenScale.GetValue(), 0.1f, 2.0f,
            [](float value) {
                getNyaConfig().FloatingScreenScale.SetValue(value);
                if (Main::NyaFloatingUI != nullptr) {
                    Main::NyaFloatingUI->UpdateScale();
                }
            }
        );
    }
}

void SettingsMenu::UpdateEndpointLists() {
    if (this->selectedDataSource == nullptr) {
        return;
    }

    // Init lists to store endpoint labels (endpoints and lables are 1:1)
    if (selectedDataSource->SfwEndpoints.size() == 0) {
        sfw_endpoint_labels = List<StringW>::New_ctor();
        sfw_endpoint_urls = List<StringW>::New_ctor();
    } else {
        sfw_endpoint_labels = NyaAPI::listEndpointLabels(&selectedDataSource->SfwEndpoints);
        sfw_endpoint_urls = NyaAPI::listEndpointUrls(&selectedDataSource->SfwEndpoints);

        // Don't add random to local
        if (selectedDataSource->Mode != DataMode::Local) {
            sfw_endpoint_labels->Add(to_utf16("random"));
            sfw_endpoint_urls->Add(to_utf16("random"));
        }
    }

    if (selectedDataSource->NsfwEndpoints.size() == 0) {
        nsfw_endpoint_labels = List<StringW>::New_ctor();
        nsfw_endpoint_urls = List<StringW>::New_ctor();
    } else {
        nsfw_endpoint_labels = NyaAPI::listEndpointLabels(&selectedDataSource->NsfwEndpoints);
        nsfw_endpoint_urls = NyaAPI::listEndpointUrls(&selectedDataSource->NsfwEndpoints);

        // Don't add random to local
        if (selectedDataSource->Mode != DataMode::Local) {
            nsfw_endpoint_labels->Add(to_utf16("random"));
            nsfw_endpoint_urls->Add(to_utf16("random"));
        }
    }

    // Update dropdowns
    // If we don't have any sources, disable the dropdown
    {
        bool empty = sfw_endpoint_labels->get_Count() == 0;

        this->sfw_endpoint->button->set_interactable(!empty);
        this->sfw_endpoint->SetTexts(sfw_endpoint_labels->i_IReadOnlyList_1_T());

        // Restore selected item
        if (empty) {
            this->sfw_endpoint->SelectCellWithIdx(0);
        } else {
            // Get the selected endpoint from the config
            string selected_url =
                EndpointConfig::getEndpointValue(getNyaConfig().config, selectedDataSourceName, false);

            // Find the index of the selected endpoint
            int index = sfw_endpoint_urls->IndexOf(to_utf16(selected_url));

            if (index >= 0) {
                this->sfw_endpoint->SelectCellWithIdx(index);
            }
        }
    }

    if (getNyaConfig().NSFWUI.GetValue()) {
        bool empty = nsfw_endpoint_labels->get_Count() == 0;

        this->nsfw_endpoint->button->set_interactable(!empty);
        this->nsfw_endpoint->SetTexts(nsfw_endpoint_labels->i_IReadOnlyList_1_T());

        // Restore selected item
        if (empty) {
            this->nsfw_endpoint->SelectCellWithIdx(0);
        } else {
            // Get the selected endpoint from the config
            string selected_url = EndpointConfig::getEndpointValue(getNyaConfig().config, selectedDataSourceName, true);

            // Find the index of the selected endpoint
            int index = nsfw_endpoint_urls->IndexOf(to_utf16(selected_url));

            if (index >= 0) {
                this->nsfw_endpoint->SelectCellWithIdx(index);
            }
        }

        // Enable/disable the toggle
        this->nsfw_toggle->set_isOn(getNyaConfig().NSFWEnabled.GetValue());
    }
}

bool SettingsMenu::isShown() {
    if (this->settingsModal == nullptr)
        return false;
    return this->settingsModal->isShown;
}

void SettingsMenu::SwitchTab(int idx) {
    this->settingsModal->get_transform()->FindChild(SourcesSettingsWrapper)->get_gameObject()->SetActive(idx == 0);
    this->settingsModal->get_transform()->FindChild(FloatingSettingsWrapper)->get_gameObject()->SetActive(idx == 1);
}

void SettingsMenu::Show() {
    INFO("Settings button clicked");
    auto imageView = this->get_gameObject()->GetComponent<NyaUtils::ImageView*>();
    this->downloadButton->set_interactable(imageView->HasImageToSave());

    // Run UI on the main thread
    QuestUI::MainThreadScheduler::Schedule([this] {
        this->tabsSwitch->segmentedControl->SelectCellWithNumber(0);
        // Autonya
        autoNyaButton->set_isOn(getNyaConfig().AutoNya.GetValue());

        string API = getNyaConfig().API.GetValue();
        SourceData* source = nullptr;

        // Catch for invalid apis
        try {
            source = NyaAPI::get_data_source(API);
        } catch (...) {
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

        this->selectedDataSourceName = API;
        this->selectedDataSource = source;

        this->UpdateEndpointLists();

        this->SwitchTab(0);

        settingsModal->Show(true, true, nullptr);
    });
}

void SettingsMenu::Hide() {
    this->settingsModal->Hide(true, nullptr);
}

}  // namespace Nya
