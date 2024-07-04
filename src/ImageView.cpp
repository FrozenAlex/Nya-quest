#define RAPIDJSON_HAS_STDSTRING 1
// OurClass.cpp
#include "main.hpp"
#include <fstream>
#include <iostream>
#include "ImageView.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "HMUI/ImageView.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "UnityEngine/SpriteMeshType.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "bsml/shared/BSML-Lite/Creation/Misc.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "assets.hpp"

#include "bsml/shared/Helpers/utilities.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Utils.hpp"
#include "UnityEngine/Coroutine.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/Shader.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "Events.hpp"
#include "System/GC.hpp"


// Necessary
DEFINE_TYPE(NyaUtils, ImageView);

using namespace UnityEngine;
using namespace NyaAPI;

// Start
void NyaUtils::ImageView::ctor()
{
    this->lastImageURL = "";
    this->isNSFW = false;
    // Temp File name
    // WARNING: Sometimes this temp name does not make sense, so I validate it
    this->tempName = "";
    this->autoNyaRunning = false;
    this->isLoading = false;
    // Create callback
    this->imageLoadingChange = UnorderedEventCallback<bool>();
}

// Awake
void NyaUtils::ImageView::Awake()
{
    // Get the image view
    imageView = this->get_gameObject()->GetComponent<HMUI::ImageView *>();
}

bool NyaUtils::ImageView::HasImageToSave() {
    return (this->lastImageURL != "" && this->tempName != "" && Nya::Utils::IsImage(this->tempName));
}


// TODO: review crash related to this (https://analyzer.questmodding.com/crashes/4V2U)
//  filesystem error: in rename: No such file or directory [/sdcard/ModData/com.beatgames.beatsaber/Mods/Nya/Temp/edOwqtb4.png] [/sdcard/ModData/com.beatgames.beatsaber/Mods/Nya/Images/nsfw/edOwqtb4.png]
void NyaUtils::ImageView::SaveImage() {
    if (this->lastImageURL != "" && this->tempName != "" && Nya::Utils::IsImage(this->tempName)) {
        INFO("MOVING FILE");
        std::string original = NyaGlobals::tempPath + this->tempName;
        if (this->isNSFW) {
            FileUtils::moveFile(original, NyaGlobals::imagesPathNSFW + this->tempName);
        } else {
            INFO("MOVING FROM {} TO {}", original,  NyaGlobals::imagesPathSFW + this->tempName);
            FileUtils::moveFile(original, NyaGlobals::imagesPathSFW + this->tempName);
        }
        
        INFO("MOVED FILE");
        // Cleanup 
        this->lastImageURL = "";
        this->tempName = "";
    }
}

// Update
void NyaUtils::ImageView::GetImage(std::function<void(bool success)> finished = nullptr)
{
    this->isLoading = true;
    if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(true);

    // Delete the last downloaded image
    if (
        !this->tempName.empty() &&
        this->lastImageURL.empty() &&
        Nya::Utils::IsImage(this->tempName)
    ) {
        std::string original = NyaGlobals::tempPath + this->tempName;
        FileUtils::deleteFile(original);
        
        // Reset 
        this->lastImageURL = "";
        this->tempName = "";
    }


    // Download file, get the file from internal storage
    bool NSFWEnabled = false;

    #ifdef NSFW
        if (getNyaConfig().NSFWUI.GetValue()) {
            NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
        }
    #endif
    // Get value
    std::string currentAPI = getNyaConfig().API.GetValue();
    
    SourceData* source = nullptr;

    // Catch for invalid apis
    try
    {
        source = NyaAPI::get_data_source(currentAPI);
    }
    catch(...)
    {
        currentAPI = "waifu.pics";
        getNyaConfig().API.SetValue(currentAPI);
        source = NyaAPI::get_data_source(currentAPI);
    }
    

  // Local files
  if (source->Mode == DataMode::Local) {
      std::string imageFolderPath = "";
      if (NSFWEnabled){ 
        imageFolderPath = NyaGlobals::imagesPathNSFW;
      } else {
        imageFolderPath = NyaGlobals::imagesPathSFW;
      }

      auto fileList = FileUtils::getAllFilesInFolder(imageFolderPath);

      // No files found
      if (fileList.size() == 0) {
        ERROR("Selected local folder is empty");
        this->SetErrorImage();

        // Set is loading status
        this->isLoading = false;
        if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);

        if (finished != nullptr) finished(false);
        
      } else {
        int randomIndex = Nya::Utils::random(0, fileList.size()-1);

        auto path = fileList[randomIndex];
        DEBUG("Selected file: {}", path);
        BSML::Utilities::SetImage(this->imageView,  "file://" + path, true, BSML::Utilities::ScaleOptions(), false, [finished, this]() {
            DEBUG("Image loaded");
            // Set is loading status
            this->isLoading = false;
            if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);

            if (finished != nullptr) finished(true);
        }, [finished, this](BSML::Utilities::ImageLoadError error) {
            this->SetErrorImage();

            // Set is loading status
            this->isLoading = false;
            if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);

            if (finished != nullptr) finished(false);
        });
      }

     
  } else if (source->Mode == DataMode::Json || source->Mode == DataMode::Authenticated) {

    // API is authenticated
    bool authenticated = source->Mode == DataMode::Authenticated;
      
    // Construct the url
    // TODO: check if endpoint from the setting exists and make it dynamic

    std::string endpointValue = EndpointConfigUtils::getEndpointValue(currentAPI, NSFWEnabled);
    
    // Fallback to sfw if nsfw is enabled and no nsfw endpoint is found
    if (endpointValue == "" && NSFWEnabled) {
        endpointValue = EndpointConfigUtils::getEndpointValue(currentAPI, false);
    }

    bool nsfwEmpty = source->NsfwEndpoints.empty();
    bool sfwEmpty = source->SfwEndpoints.empty();

    // If the endpoint is random, get a random endpoint
    if (endpointValue == "random") {
        if (NSFWEnabled) {
            if (nsfwEmpty) {
                auto endpoint = NyaAPI::getRandomEndpoint(&source->SfwEndpoints);
                if (endpoint != nullptr) endpointValue = endpoint->url;
            } else {
                auto endpoint = NyaAPI::getRandomEndpoint(&source->NsfwEndpoints);
                if (endpoint != nullptr) endpointValue = endpoint->url;
            }
        } else {
            if (sfwEmpty) {
                // DO NOT FALLBACK TO NSFW since it is disabled
                endpointValue = "";
            } else {
                auto endpoint = NyaAPI::getRandomEndpoint(&source->SfwEndpoints);
                if (endpoint != nullptr) endpointValue = endpoint->url;
            }
        }
    }

    std::string endpointURL = source->BaseEndpoint + endpointValue;

    if (!NSFWEnabled) INFO("Endpoint URL: {}", endpointURL);
    
    // Get the image url from the api
    NyaAPI::get_path_from_json_api(source, endpointURL, 10.0f, [this, finished, NSFWEnabled](bool success, std::string url) {
        // If we failed to get the image url
        if (!success) {
            // Error getting things
            ERROR("Failed to load image from api");

            // getLogger().Backtrace(20);
            BSML::MainThreadScheduler::Schedule([this, finished]{
                this->SetErrorImage();
                
                // Set is loading status
                this->isLoading = false;
                if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);
                
                if (finished != nullptr) finished(false);
            });
            return;
        }

        if (!NSFWEnabled) INFO("Image URL: {}", url);

        BSML::MainThreadScheduler::Schedule([this, url, finished, NSFWEnabled]{
            // Make temp file name
            std::string fileExtension = FileUtils::GetFileFormat(url);
            std::string fileName = Nya::Utils::RandomString(8);

            std::string filePath = NyaGlobals::tempPath + fileName  + fileExtension;
            std::string fileFullName = fileName  + fileExtension;

            Nya::Utils::DownloadFile(url, filePath, [this, finished, url, NSFWEnabled, fileFullName](bool success, std::string path) {
                if (!success ) {
                    this->SetErrorImage();

                    // Set is loading status
                    this->isLoading = false;
                    if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);

                    if (finished != nullptr) finished(false);
                } else {
                    this->lastImageURL = url;
                    this->tempName = fileFullName;
                    this->isNSFW = NSFWEnabled;

                    BSML::Utilities::SetImage(this->imageView,  "file://" + path, true, BSML::Utilities::ScaleOptions(), false, [finished, this]() {
                        // Set is loading status
                        this->isLoading = false;
                        if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);
                 
                        if (finished != nullptr) finished(true);
                    }, [finished, this](BSML::Utilities::ImageLoadError error) {
                      this->SetErrorImage();

                      // Set is loading status
                      this->isLoading = false;
                      if (this->imageLoadingChange.size() > 0) this->imageLoadingChange.invoke(false);

                      if (finished != nullptr) finished(false);
                  });
                }
                
            });
            
        });
            
     
    }, authenticated ? "FP-Public-naEjca70OhKMtq67WpzaN8Gs" : "");
  }           
}

void NyaUtils::ImageView::SetErrorImage()
{
    Nya::Utils::RemoveAnimationUpdater(this->imageView);
    this->imageView->set_sprite(BSML::Lite::ArrayToSprite(Assets::Chocola_Dead_png));
}

void NyaUtils::ImageView::dtor()
{
}

void NyaUtils::ImageView::OnNyaPhysicalClick(){
    if (this->isLoading) {
        return;
    }

    this->GetImage(nullptr);
}

void NyaUtils::ImageView::OnEnable()
{
    // Subscribe to physical click
    Nya::GlobalEvents::onControllerNya += {&NyaUtils::ImageView::OnNyaPhysicalClick ,this};

     if (getNyaConfig().AutoNya.GetValue() && this->autoNyaRunning == false) {
         this->StartCoroutine(custom_types::Helpers::new_coro(this->AutoNyaCoro()));
     }
}

void NyaUtils::ImageView::OnDisable()
{
    this->autoNyaRunning = false;
    // Unsubscribe to physical click
    Nya::GlobalEvents::onControllerNya -= {&NyaUtils::ImageView::OnNyaPhysicalClick ,this};
}


// Coroutine for autonya
custom_types::Helpers::Coroutine NyaUtils::ImageView::AutoNyaCoro()
{
    // Extra guard agains simultaneous autonyas just to be sure
    if (this->autoNyaRunning) {
        co_return;
    };
    autoNyaNewImage = true;
    this->autoNyaRunning = true;

    while (true) {
        // Check if it's enabled
        bool enabled = getNyaConfig().AutoNya.GetValue();

        // Quit if not enabled or is not running
        if (!enabled ||  !this->autoNyaRunning) {
            this->autoNyaRunning = false;
            co_return;
        }

        if (autoNyaNewImage) {
            autoNyaNewImage = false;
            int waitTime = getNyaConfig().AutoNyaDelay.GetValue();
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(waitTime));

            // Check again cause if it's disabled we want to quit after the delay
            enabled = getNyaConfig().AutoNya.GetValue();
            if (!enabled ||  !this->autoNyaRunning) {
                this->autoNyaRunning = false;
                co_return;
            }
            this->GetImage([this](bool success){
                this->autoNyaNewImage = true;
            });
        }

        co_yield nullptr;
    }
    
    
}