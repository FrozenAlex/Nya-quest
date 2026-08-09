#include "UI/Modals/SettingsMenu.hpp"
#include "bsml/shared/BSML-Lite/Creation/Buttons.hpp"
#include "bsml/shared/BSML-Lite/Creation/Misc.hpp"
#include "bsml/shared/BSML-Lite/Creation/Settings.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "bsml/shared/BSML-Lite/Creation/Misc.hpp"
#include "beatsaber-hook/shared/stringw.hpp"
#include "System/String.hpp"

#include "HMUI/ScrollView.hpp"
#include "NyaFloatingUI.hpp"
#include "NyaConfig.hpp"
#include "EndpointConfigUtils.hpp"
#include "Utils/Utils.hpp"
#include "logging.hpp"
#include "main.hpp"
DEFINE_TYPE(Nya, SettingsMenu);

static ConstString SourcesSettingsWrapper("SourcesSettingsWrapper");
static ConstString SettingsMenuWrapper("SettingsMenuWrapper");
static ConstString FloatingSettingsWrapper("FloatingSettingsWrapper");

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
using namespace std;

namespace Nya {
// Constructor
void SettingsMenu::ctor() {
}


void SettingsMenu::Awake() {
    // Init modal
    this->settingsModal = BSML::Lite::CreateModal(get_transform(), { 0, 0 } ,{65, 60}, nullptr);
    this->settingsModal->get_gameObject()->set_name(SettingsMenuWrapper);

    auto sourcesView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform*>();
    sourcesView->get_gameObject()->set_name(SourcesSettingsWrapper);
    sourcesView->SetParent(this->settingsModal->get_transform(), false);

    auto floatingView = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::RectTransform*>();
    floatingView->get_gameObject()->set_name(FloatingSettingsWrapper);
    floatingView->SetParent(this->settingsModal->get_transform(), false);

    // Setup canvas
    auto canvas = BSML::Lite::CreateCanvas();
    canvas->get_transform()->SetParent(this->settingsModal->get_transform(), false);
    auto controlRect = canvas->get_transform().cast<UnityEngine::RectTransform>();
    controlRect->set_anchoredPosition({0, 0});

    controlRect->set_anchorMin(UnityEngine::Vector2(0.0f, 0.0f));
    controlRect->set_anchorMax(UnityEngine::Vector2(1.0f, 1.0f));

    controlRect->set_sizeDelta({50, 8.5});
    controlRect->set_localScale({1, 1, 1});

    auto canvasLayoutElement = canvas->GetComponent<LayoutElement*>();
    if (canvasLayoutElement) {
        canvasLayoutElement = canvas->AddComponent<LayoutElement*>();
        canvasLayoutElement->set_preferredWidth(60.0);
        canvasLayoutElement->set_preferredHeight(60.0);
    } else {
        INFO("Canvas layout element is null, adding..");
    }

    // Create tabs control
    ArrayW<StringW> options(2);
        options[0] = "sources";
        options[1] = "floating";
    this->tabsSwitch =
        Nya::Utils::CreateTextSegmentedControl(controlRect->get_transform(), {0, -5.5}, {45, 5.5}, options,
                                               bind(&SettingsMenu::SwitchTab, this, placeholders::_1));                         

    // Create a text that says "Hello World!" and set the parent to the container.
    VerticalLayoutGroup* sourcesViewLayout = BSML::Lite::CreateVerticalLayoutGroup(sourcesView);
    sourcesViewLayout->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
    sourcesViewLayout->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
    sourcesViewLayout->GetComponent<LayoutElement*>()->set_preferredWidth(60.0);

    static std::vector<std::string_view> loadingOptionStrings = {"Loading.."};
    std::span<std::string_view> loadingOptions(loadingOptionStrings);
    // Nya configuration
    {

        // API Selection (nothing to select for now)
        string API = getNyaConfig().API.GetValue();

        this->api_switch = BSML::Lite::CreateDropdown(sourcesViewLayout->get_transform(), "API", "Loading..", loadingOptions, [this](StringW value) {

            // Change the API in the config
                getNyaConfig().API.SetValue(value);
                // Get this source
                auto source = NyaAPI::get_data_source(value);

                this->selectedDataSource = source;
                this->selectedDataSourceName = std::string(value);

                this->UpdateEndpointLists();
            });

        // SFW endpoint switch
        this->sfw_endpoint =
            BSML::Lite::CreateDropdown(sourcesViewLayout->get_transform(), "SFW endpoint", "Loading..", loadingOptions,
            [this](StringW value) {
                // Get current endpoint
                string API = this->selectedDataSourceName;
                // find url
                int index = this->sfw_endpoint->index;
                StringW url = this->sfw_endpoint_urls->get_Item(index);
                EndpointConfigUtils::updateEndpointValue(API, false, url);
            });

        if (getNyaConfig().NSFWUI.GetValue()) {
            // NSFW endpoint selector
            this->nsfw_endpoint =
                BSML::Lite::CreateDropdown(sourcesViewLayout->get_transform(), "NSFW endpoint", "Loading..",
                               loadingOptions, [this](StringW value) {
                                   // Get current endpoint
                                   string API = this->selectedDataSourceName;

                                   // find url
                                   int index = this->nsfw_endpoint->index;
                                   StringW url = this->nsfw_endpoint_urls->get_Item(index);

                                   // Change the endpoint in the config
                                   EndpointConfigUtils::updateEndpointValue(API, true, url);
                               });

            // NSFW toggle
            bool NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
            this->nsfw_toggle = BSML::Lite::CreateToggle(sourcesViewLayout->get_transform(), "NSFW toggle", NSFWEnabled,
                                             [](bool isChecked) { getNyaConfig().NSFWEnabled.SetValue(isChecked); });
        }

        HorizontalLayoutGroup* horz = BSML::Lite::CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
        horz->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);
        {
            this->downloadButton =
                BSML::Lite::CreateUIButton(horz->get_transform(), "Download Nya", "PracticeButton", [this]() {
                    UnityW<Nya::ImageView> imageView = this->get_gameObject()->GetComponent<Nya::ImageView*>();
                    imageView->SaveImage();
                    this->downloadButton->set_interactable(false);
                    this->settingsModal->Hide(true, nullptr);
                });

            UnityW<Button> closeButton = BSML::Lite::CreateUIButton(horz->get_transform(), "Close", "PracticeButton",
                                                 [this]() { this->settingsModal->Hide(true, nullptr); });
            // Set button sizes
            UnityEngine::Vector2 sizeDelta = {25, 8};
            Nya::Utils::SetButtonSize(this->downloadButton, sizeDelta);
            Nya::Utils::SetButtonSize(closeButton, sizeDelta);
        }
        HorizontalLayoutGroup* horz2 = BSML::Lite::CreateHorizontalLayoutGroup(sourcesViewLayout->get_transform());
        horz->GetComponent<ContentSizeFitter*>()->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->GetComponent<ContentSizeFitter*>()->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        horz->set_spacing(10);

        {
            autoNyaButton = BSML::Lite::CreateToggle(horz2, "AutoNya", getNyaConfig().AutoNya.GetValue(), [this](bool value) {
                getNyaConfig().AutoNya.SetValue(value);
                if (value) {
                    UnityW<Nya::ImageView> imageView = this->get_gameObject()->GetComponent<Nya::ImageView*>();
                    if (imageView) {
                        imageView->OnEnable();
                    }
                }
            });
        }
    }

    {
        VerticalLayoutGroup* floatingViewLayout = BSML::Lite::CreateVerticalLayoutGroup(floatingView);
        floatingViewLayout->GetComponent<ContentSizeFitter*>()->set_verticalFit(
            ContentSizeFitter::FitMode::PreferredSize);
        floatingViewLayout->GetComponent<ContentSizeFitter*>()->set_horizontalFit(
            ContentSizeFitter::FitMode::PreferredSize);
        floatingViewLayout->GetComponent<LayoutElement*>()->set_preferredWidth(60.0);
        {
            auto* hor = BSML::Lite::CreateHorizontalLayoutGroup(floatingViewLayout->get_transform());

            Button* faceHeadset =
            BSML::Lite::CreateUIButton(hor->get_transform(), "Face headset", "PracticeButton", [this]() {
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->LookAtCamera();
                }
            });
            BSML::Lite::CreateUIButton(hor->get_transform(), "Set upright", "PracticeButton", [this]() {
                if (Main::NyaFloatingUI) {
                    Main::NyaFloatingUI->SetUpRight();
                }
            });
        }
        BSML::Lite::CreateUIButton(floatingViewLayout->get_transform(), "Default position", "PracticeButton", [this]() {
            if (Main::NyaFloatingUI) {
                Main::NyaFloatingUI->SetDefaultPos();
            }
        });

        Button* closeButton = BSML::Lite::CreateUIButton(floatingViewLayout->get_transform(), "Close", "PracticeButton",
                                             [this]() { this->settingsModal->Hide(true, nullptr); });

        BSML::Lite::CreateToggle(floatingViewLayout->get_transform(), "Show handle", getNyaConfig().ShowHandle.GetValue(),
                     [](bool value) {
                         getNyaConfig().ShowHandle.SetValue(value);
                         if (Main::NyaFloatingUI && Main::NyaFloatingUI->floatingScreen != nullptr) {
                             Main::NyaFloatingUI->UpdateHandleVisibility();
                         }
                     });

        auto slider = BSML::Lite::CreateSliderSetting(floatingViewLayout->get_transform(), "Screen Scale", 0.1f,
            getNyaConfig().FloatingScreenScale.GetValue(), 0.1f, 2.0f,
            [](float value) {
                getNyaConfig().FloatingScreenScale.SetValue(value);
                if (Main::NyaFloatingUI) {
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
        sfw_endpoint_labels = ListW<StringW>::New();
        sfw_endpoint_urls = ListW<StringW>::New();
    } else {
        sfw_endpoint_labels = NyaAPI::listEndpointLabels(&selectedDataSource->SfwEndpoints);
        sfw_endpoint_urls = NyaAPI::listEndpointUrls(&selectedDataSource->SfwEndpoints);

        // Don't add random to local
        if (selectedDataSource->Mode != NyaAPI::DataMode::Local) {
            sfw_endpoint_labels->Add("random");
            sfw_endpoint_urls->Add("random");
        }
    }

    if (selectedDataSource->NsfwEndpoints.size() == 0) {
        nsfw_endpoint_labels = ListW<StringW>::New();
        nsfw_endpoint_urls = ListW<StringW>::New();
    } else {
        nsfw_endpoint_labels = NyaAPI::listEndpointLabels(&selectedDataSource->NsfwEndpoints);
        nsfw_endpoint_urls = NyaAPI::listEndpointUrls(&selectedDataSource->NsfwEndpoints);

        // Don't add random to local
        if (selectedDataSource->Mode != NyaAPI::DataMode::Local) {
            nsfw_endpoint_labels->Add("random");
            nsfw_endpoint_urls->Add("random");
        }
    }

    // Update dropdowns
    // If we don't have any sources, disable the dropdown
    {
        bool empty = sfw_endpoint_labels->_size == 0;

        this->sfw_endpoint->set_interactable(!empty);
        this->sfw_endpoint->values = sfw_endpoint_labels->i___System__Collections__Generic__IReadOnlyList_1_T_();
        this->sfw_endpoint->UpdateChoices();

        // Restore selected item
        if (empty) {
            this->sfw_endpoint->index = 0;
            this->sfw_endpoint->UpdateState();
        } else {
            // Get the selected endpoint from the config
            string selected_url =
                EndpointConfigUtils::getEndpointValue(selectedDataSourceName, false);

            // Find the index of the selected endpoint
            auto index = sfw_endpoint_urls.index_of(selected_url);

            // Select the endpoint
            if (index >= 0) {
                this->sfw_endpoint->index = index;
                this->sfw_endpoint->UpdateState();
            }
        }
    }

    if (getNyaConfig().NSFWUI.GetValue()) {
        bool empty = nsfw_endpoint_labels.size() == 0;

        this->nsfw_endpoint->set_interactable(!empty);
        this->nsfw_endpoint->values = nsfw_endpoint_labels->i___System__Collections__Generic__IReadOnlyList_1_T_();
        this->nsfw_endpoint->UpdateChoices();

        // Restore selected item
        if (empty) {
            this->nsfw_endpoint->index = 0;
            this->nsfw_endpoint->UpdateState();
        } else {
            // Get the selected endpoint from the config
            string selected_url = EndpointConfigUtils::getEndpointValue(selectedDataSourceName, true);

            // Find the index of the selected endpoint
            auto index = nsfw_endpoint_urls.index_of(selected_url);
            if (index >= 0) {
                this->nsfw_endpoint->index = index;
                this->nsfw_endpoint->UpdateState();
            }
            
        }

        // Enable/disable the toggle
        this->nsfw_toggle->set_Value(getNyaConfig().NSFWEnabled.GetValue());
    }
}

bool SettingsMenu::isShown() {
    if (this->settingsModal == nullptr)
        return false;
    return this->settingsModal->_isShown;
}

void SettingsMenu::SwitchTab(int idx) {
    this->settingsModal->get_transform()->FindChild(SourcesSettingsWrapper)->get_gameObject()->SetActive(idx == 0);
    this->settingsModal->get_transform()->FindChild(FloatingSettingsWrapper)->get_gameObject()->SetActive(idx == 1);
}

void SettingsMenu::Show() {
    INFO("Settings button clicked");
    UnityW<Nya::ImageView> imageView = this->get_gameObject()->GetComponent<Nya::ImageView*>();
    this->downloadButton->set_interactable(imageView->HasImageToSave());

    // Run UI on the main thread
    BSML::MainThreadScheduler::Schedule([this] {
        this->tabsSwitch->segmentedControl->SelectCellWithNumber(0);
        // Autonya
        autoNyaButton->set_Value(getNyaConfig().AutoNya.GetValue());

        string API = getNyaConfig().API.GetValue();
        NyaAPI::SourceData* source = nullptr;

        // Catch for invalid apis
        try {
            source = NyaAPI::get_data_source(API);
        } catch (...) {
            API = "waifu.pics";
            getNyaConfig().API.SetValue(API);
            source = NyaAPI::get_data_source(API);
        }

        auto sources = Nya::Utils::vectorToList(NyaAPI::get_source_list());

        // Set values
        this->api_switch->values = sources->i___System__Collections__Generic__IReadOnlyList_1_T_();
        // Refresh choices
        this->api_switch->UpdateChoices();
        // Select choice with id
        this->api_switch->set_Value(StringW(API));

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
