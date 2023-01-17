#include "Utils/Utils.hpp"
#include <random>
#include "Helpers/utilities.hpp"
#include "System/StringComparison.hpp"
#include "System/Uri.hpp"
#include <fstream>
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "System/Uri.hpp"
#include "System/StringComparison.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp"

#include "custom-types/shared/coroutine.hpp"
#define coro(coroutine) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(coroutine))
using namespace UnityEngine;
using namespace UnityEngine::Networking;

namespace Nya::Utils {
    /**
     * @brief Converts vector to list
     * 
     * @param values 
     * @return List<StringW>* 
     */
    List<StringW>* vectorToList(std::vector<StringW> values) {
        List<StringW>* list = List<StringW>::New_ctor();
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

    ListWrapper<StringW> listStringToStringW(std::list<std::string> values) {
        // TODO: Fix
        int count = values.size();
        
        // Convert stuff to list
        ListWrapper<StringW> list(List<StringW>::New_ctor());
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
        std::vector<StringW> vector = std::vector<StringW>(values->items);
        return vector;
    }

    /**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int findStrIndexInList(List<StringW>* values, StringW string ) {
        for (int i = 0; i < values->size; i++){
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

    // Download new picture
    void onNyaClick(UnityEngine::UI::Button* button, NyaUtils::ImageView* imageView) {
        try
        {   
            button->set_interactable(false);
            imageView->GetImage([button, imageView](bool success){
                button->set_interactable(true);
            });
        }
        // TODO: If the source is not set up, set up the default
        catch(const std::exception& e)
        {
            ERROR("Custom fail");
            getLogger().Backtrace(20);
            button->set_interactable(true);
        }  
    }

    // Checks if the path is animated
    bool IsAnimated(StringW str)
    {
        return  str->EndsWith(".gif", System::StringComparison::OrdinalIgnoreCase) || 
                str->EndsWith("_gif", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_apng", System::StringComparison::OrdinalIgnoreCase);
    }
    bool IsImage(StringW str) {
     
        return  str->EndsWith(".gif", System::StringComparison::OrdinalIgnoreCase) || 
                str->EndsWith("_gif", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_apng", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith(".png", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith("_png", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".jpeg", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_jpeg", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".webp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_webp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith(".tiff", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_tiff", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".jpg", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_jpg", System::StringComparison::OrdinalIgnoreCase) ||
                str->EndsWith(".bmp", System::StringComparison::OrdinalIgnoreCase)||
                str->EndsWith("_bmp", System::StringComparison::OrdinalIgnoreCase);
  
    }

    // Got tired of pointers. If we can get a controller from a pointer, it means it's valid
    VRUIControls::VRPointer* getAnyPointerWithController(){
        // Get all pointers
        auto pointers = UnityEngine::Resources::FindObjectsOfTypeAll<VRUIControls::VRPointer*>();

        int count = pointers.Length();
        DEBUG("Total number of pointers: {}", count );

        for (int i = 0; i < count; i++)
        {
            auto pointer = pointers.get(i);
            
            // Game clones the pointers all the time and disables the original pointer, so we need to only look at active pointers
            if (pointer->get_isActiveAndEnabled()) {
                // VR conroller is sometimes null after leaving multiplayer?
                auto vrController = pointer->get_vrController();
                if (vrController  && vrController->m_CachedPtr.m_value) {
                    DEBUG("Found vr conroller on {}", i );
                    return pointer;
                }
            }
            
        }   

        return nullptr;
    }


    /// @brief Downloads data and returns it. If it does not get the data, 
    /// @param uri File url
    /// @param path Path to a new file on local storage
    /// @param onFinished 
    /// @return 
    custom_types::Helpers::Coroutine DownloadFileCoroutine(StringW uri, StringW path, std::function<void(bool success, StringW path)> onFinished) {

        DEBUG("GetReq");
        auto www = UnityWebRequest::Get(uri);
        // I suppose it's in seconds
        www->set_timeout(10);
        DEBUG("SendReq");
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        
        
        
        if (!www->get_isNetworkError()) {
            DEBUG("Got data, callback");
            // Saving files 
            std::ofstream f(path,  std::ios_base::binary | std::ios_base::trunc);
            auto arr = www->get_downloadHandler()->GetData();
            f.write((char*)arr.begin(), arr.size());
            f.flush();
            f.close();

            if (onFinished)
                onFinished(true, path);
        } else {
            DEBUG("Failed to get the data");
            if (onFinished) 
                onFinished(false, path);
        }

        co_return;
    }

    void DownloadFile(StringW uri, StringW path, std::function<void(bool success, StringW path)> onFinished) {
        if (!getNyaConfig().NSFWEnabled.GetValue()){
            INFO("Getting data from uri: {}", (std::string) uri);
        }
        
        if (!onFinished) {
            ERROR("Can't get data async without a callback to use it with");
            return;
        }
        coro(DownloadFileCoroutine(uri, path, onFinished));
    }
}

