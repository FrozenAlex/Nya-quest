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
    // Stolen from https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    int random(int min, int max) //range : [min, max]
    {
        static bool first = true;
        if (first) 
        {  
            srand( time(NULL) ); //seeding for the first time only!
            first = false;
        }
        return min + rand() % (( max + 1 ) - min);
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

    /// @brief Downloads data and returns it. If it does not get the data, 
    /// @param uri File url
    /// @param path Path to a new file on local storage
    /// @param onFinished 
    /// @return 
    custom_types::Helpers::Coroutine DownloadFileCoroutine(std::string uri, std::string path, std::function<void(bool success, std::string path)> onFinished) {

        DEBUG("GetReq");
        auto www = UnityWebRequest::Get(uri);
        // I suppose it's in seconds
        www->set_timeout(10);
        DEBUG("SendReq");
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        
        
        auto error = www->GetError();
        bool failed = error != UnityEngine::Networking::UnityWebRequest::UnityWebRequestError::OK;
        
        if (failed) {
            ERROR("Failed to get the data WebError: {}", UnityEngine::Networking::UnityWebRequest::GetWebErrorString(error));
            if (onFinished) 
                onFinished(false, path);
            
        }

        DEBUG("Trying to get downloadHandler");
        UnityEngine::Networking::DownloadHandler* downloadHandler = www->get_downloadHandler();
        if (downloadHandler == nullptr) {
            ERROR("Download handler is null");
            if (onFinished) 
                onFinished(false, path);
            co_return;
        }
        
        auto arr = downloadHandler->GetData();
        if (arr.size() == 0) {
            ERROR("Failed to get the data");
            if (onFinished) 
                onFinished(false, path);
            co_return;
        }

        
        // Check free space
        fs::path fsPath(path);
        auto parentPath = fsPath.parent_path();
        auto space = fs::space(parentPath);
        if (space.available < arr.size()) {
            ERROR("Not enough space to save the file, available: {}, needed: {}", space.available, arr.size());
            if (onFinished) 
                onFinished(false, path);
            co_return;
        }
        
        try {
            std::ofstream f(path,  std::ios_base::binary | std::ios_base::trunc);
            f.write((char*)arr.begin(), arr.size());
            f.flush();
            f.close();
        } catch (std::exception& e) {
            ERROR("Failed to save the file: {}", e.what());
            if (onFinished) 
                onFinished(false, path);
            co_return;
        }
        
        if (!fs::exists(path)) {
            ERROR("Failed to save the file, it does not exist after saving");
            if (onFinished) 
                onFinished(false, path);
            co_return;
        }

        if (onFinished)
            onFinished(true, path);

        co_return;
    }

    void DownloadFile(std::string uri, std::string path, std::function<void(bool success, std::string path)> onFinished) {
        if (!getNyaConfig().NSFWEnabled.GetValue()){
            INFO("Getting data from uri: {}", uri.c_str());
        }
        
        if (!onFinished) {
            ERROR("Can't get data async without a callback to use it with");
            return;
        }

        // TODO: Implement a global coro starter for Nya
         BSML::SharedCoroutineStarter::StartCoroutine(
            custom_types::Helpers::CoroutineHelper::New(
             DownloadFileCoroutine(uri, path, onFinished)
            ));
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
        UnityW<BSML::AnimationStateUpdater> oldStateUpdater = image->GetComponent<BSML::AnimationStateUpdater*>();
        if (oldStateUpdater) {
            Object::DestroyImmediate(oldStateUpdater);
        }
    }
}

