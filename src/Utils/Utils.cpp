#include "Utils/Utils.hpp"
#include <random>
#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "bsml/shared/BSML/Animations/AnimationStateUpdater.hpp"
#include "System/StringComparison.hpp"
#include "System/Uri.hpp"
#include <fstream>
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "web-utils/shared/WebUtils.hpp"
#include "NyaConfig.hpp"
#include "UnityEngine/Resources.hpp"

#include "custom-types/shared/coroutine.hpp"
using namespace UnityEngine;
using namespace UnityEngine::Networking;

namespace fs = std::filesystem;
 

// Extensions for images
static std::unordered_set<std::string> ImageExtensions = {
    ".png",
    ".jpg",
    ".jpeg",
    ".gif",
    ".apng",
    ".webp",
    ".tiff",
    ".bmp"
};  

// Extensions for animated images
static std::unordered_set<std::string> AnimatedImageExtensions = {
    ".gif",
    ".apng"
};

namespace Nya::Utils {
    // Lowercase a string
    std::string ToLowercase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
        return str;
    }

    /**
     * @brief Converts vector to list
     * 
     * @param values 
     * @return List<StringW>* 
     */
    ListW<StringW> vectorToList(std::vector<StringW> values) {
        ListW<StringW> list = ListW<StringW>::New();
        for (int i = 0; i < values.size(); i++){
            auto value = values[i];
            list->Add(value);
        }
        return list;
    }

    
    NyaUI::CustomTextSegmentedControlData* CreateTextSegmentedControl(UnityEngine::Transform* parent, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, ArrayW<StringW> values, std::function<void(int)> onCellWithIdxClicked) {
        // TODO: Optimize getting the template
        static SafePtrUnity<HMUI::TextSegmentedControl> segmentedControlTemplate;
        if (!segmentedControlTemplate)
        {
            segmentedControlTemplate = UnityEngine::Resources::FindObjectsOfTypeAll<HMUI::TextSegmentedControl *>()->First([](auto x)
                                                                                                       {
                // INFO: This selector could break in any new update. If you came here, try to modify the selection
                if (x->get_name() != "TextSegmentedControl") return false;
                auto parent = x->get_transform()->get_parent();
                if (!parent) return false;
                auto parentName = parent->get_name();
                // Oculus PC settings menu seems pog. Lets me fix my view
                return parentName == "GameplaySetupViewController" || parentName == "BaseGameplaySetupWrapper" || parentName == "OculusPCSettingsMenu"; });
        }

        auto segmentedControlObj = Object::Instantiate(segmentedControlTemplate->get_gameObject(), parent, false);
        segmentedControlObj->SetActive(false);
        static ConstString NyaUITextSegmentedControl("NyaUITextSegmentedControl");
        segmentedControlObj->set_name(NyaUITextSegmentedControl);
        auto rectTransform = segmentedControlObj->get_transform().cast<RectTransform>();
        rectTransform->set_sizeDelta(sizeDelta);
        rectTransform->set_anchoredPosition(anchoredPosition);

        Object::DestroyImmediate(segmentedControlObj->GetComponent<HMUI::TextSegmentedControl *>());
        auto control = segmentedControlObj->AddComponent<HMUI::SegmentedControl *>();
        auto result = segmentedControlObj->AddComponent<NyaUI::CustomTextSegmentedControlData *>();

        result->firstCellPrefab = segmentedControlTemplate->_firstCellPrefab;
        result->lastCellPrefab = segmentedControlTemplate->_lastCellPrefab;
        result->middleCellPrefab = segmentedControlTemplate->_middleCellPrefab;
        result->singleCellPrefab = segmentedControlTemplate->_singleCellPrefab;

        result->segmentedControl = control;
        control->dataSource = reinterpret_cast<HMUI::SegmentedControl::IDataSource*>(result);

        if (onCellWithIdxClicked)
        {
            using DelegateType = System::Action_2<UnityW<HMUI::SegmentedControl>, int> *;
            std::function<void(HMUI::SegmentedControl *, int)> fun = [onCellWithIdxClicked](UnityW<HMUI::SegmentedControl> cell, int idx)
            { onCellWithIdxClicked(idx); };
            auto delegate = BSML::MakeDelegate<DelegateType>(fun);
            control->add_didSelectCellEvent(delegate);
        }

        int childCount = result->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            Object::DestroyImmediate(result->get_transform()->GetChild(0)->get_gameObject());
        }

        result->set_texts(values);

        segmentedControlObj->SetActive(true);
        return result;
    }
    
    
    std::string RandomString(const int len) {
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::string tmp_s;
        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i) {
            tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        
        return tmp_s;
    }

    ListW<StringW> listStringToStringW(std::list<std::string> values) {
        // TODO: Fix
        int count = values.size();
        
        // Convert stuff to list
        ListW<StringW> list = ListW<StringW>::New();
        if (count == 0) {
            return list;
        }
        list->EnsureCapacity(count);
        for (auto item : values) {list->Add(item);};

        return list;
    }

    /**
     * @brief Converts listW to vector
     * 
     * @param values 
     * @return List<StringW> 
     */
    std::vector<StringW> listWToVector(List<StringW>* values) {
        std::vector<StringW> vector = std::vector<StringW>(values->_items);
        return vector;
    }

    /**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int findStrIndexInList(ListW<StringW> values, StringW string ) {
        for (int i = 0; i < values->get_Count(); i++){
            auto value = values->get_Item(i);
            if (value == string) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int findStrIndexInListC(std::list<std::string> values, StringW string ) {
        
        // Create iterator pointing to first element
        std::list<std::string>::iterator it = values.begin();

        for (int i = 0; i < values.size(); i++){
            auto value = *it;
            if (value == string) {
                return i;
            }
            std::advance(it, 1);
        }
        return -1;
    }

    // Generate random number 
    int random(int min, int max)
    {
        static std::random_device rd;
        static std::mt19937 generator(rd());

        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }

    // Checks if the path is animated
    bool IsAnimated(std::string str)
    {
        
        auto path = fs::path(str);
        auto extension = path.extension().string();
        
        // Convert to lowercase
        extension = ToLowercase(extension);
        
        return AnimatedImageExtensions.contains(extension);
    }

    // Checks if the path is animated
    bool IsGif(std::string str)
    {   
        auto path = fs::path(str);
        auto extension = path.extension().string();
        
        return extension == ".gif";
    }

    bool IsImage(std::string str) {
        auto path = fs::path(str);
        auto extension = path.extension().string();

        // Convert to lowercase
        extension = ToLowercase(extension);
        
        return ImageExtensions.contains(extension);
    }

    void DownloadFile(std::string uri, std::string path, std::function<void(bool success, std::string path)> onFinished) {
        if (!getNyaConfig().NSFWEnabled.GetValue()){
            INFO("Getting data from uri: {}", uri.c_str());
        }
        
        if (!onFinished) {
            ERROR("Can't get data async without a callback to use it with");
            return;
        }

        std::thread([uri, path, onFinished](){
            auto response = WebUtils::Get<WebUtils::DataResponse>(
                WebUtils::URLOptions(uri)
            );

            if (!response.IsSuccessful()) {
                ERROR("Failed to get the data status: {}", response.curlStatus);
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){ onFinished(false, path); });
                }
                return;
            }

            if (!response.responseData.has_value()) {
                ERROR("Failed to get the data, no data");
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){ onFinished(false, path); });
                }
                return;
            }

            auto& data = response.responseData.value();
            if (data.size() == 0) {
                ERROR("Failed to get the data, data size is 0");
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){ onFinished(false, path); });
                }
                return;
            }

            // Check free space
            fs::path fsPath(path);
            auto parentPath = fsPath.parent_path();
            auto space = fs::space(parentPath);
            if (space.available < data.size()) {
                ERROR("Not enough space to save the file, available: {}, needed: {}", space.available, data.size());
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){ onFinished(false, path); });
                }
                return;
            }

            try {
                std::ofstream f(path,  std::ios_base::binary | std::ios_base::trunc);
                f.write(reinterpret_cast<const char*>(data.data()), data.size());
                f.flush();
                f.close();
            } catch (std::exception& e) {
                ERROR("Failed to save the file: {}", e.what());
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){ onFinished(false, path); });
                }
                return;
            }
            
            if (!fs::exists(path)) {
                ERROR("Failed to save the file, it does not exist after saving");
                if (onFinished) {
                    BSML::MainThreadScheduler::Schedule([path, onFinished](){
                        onFinished(false, path);
                    });
                }
                return;
            }

            if (onFinished) {
                BSML::MainThreadScheduler::Schedule([path, onFinished](){
                    onFinished(true, path);
                });
            }
        }).detach();
    }

    void SetButtonSize(UnityW<UnityEngine::UI::Button> button, UnityEngine::Vector2 size) {
        if (!button) {
            ERROR("Button is null");
            return;
        }
        UnityW<UnityEngine::UI::LayoutElement> layoutElement = button->get_gameObject()->GetComponent<UnityEngine::UI::LayoutElement*>();
        if(!layoutElement)
            layoutElement = button->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
        if (!layoutElement) {
            ERROR("Failed to get or add layout element");
            return;
        }

        layoutElement->set_minWidth(size.x);
        layoutElement->set_minHeight(size.y);
        layoutElement->set_preferredWidth(size.x);
        layoutElement->set_preferredHeight(size.y);
        layoutElement->set_flexibleWidth(size.x);
        layoutElement->set_flexibleHeight(size.y);
    }

    void RemoveAnimationUpdater(UnityW<UnityEngine::UI::Image> image) {
        if (!image) {
            ERROR("Image is null when trying to remove animation updater");
            return;
        }
        UnityW<BSML::AnimationStateUpdater> oldStateUpdater = image->GetComponent<BSML::AnimationStateUpdater*>();
        if (oldStateUpdater) {
            Object::DestroyImmediate(oldStateUpdater);
        }
    }
}

