#define RAPIDJSON_HAS_STDSTRING 1
// OurClass.cpp
#include "ImageView.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/GameObject.hpp"
#include "HMUI/ImageView.hpp"

#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "assets.hpp"

#include "bsml/shared/Helpers/utilities.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Utils.hpp"
#include "UnityEngine/Coroutine.hpp"

#include "Events.hpp"
#include "NyaConfig.hpp"
#include "EndpointConfigUtils.hpp"
#include "API.hpp"

// Necessary
DEFINE_TYPE(Nya, ImageView);

using namespace UnityEngine;
using namespace NyaAPI;

// Start
void Nya::ImageView::ctor()
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
void Nya::ImageView::Awake()
{
    // Get the image view
    imageView = this->get_gameObject()->GetComponent<HMUI::ImageView *>();
}

bool Nya::ImageView::HasImageToSave() {
    return (!this->lastImageURL.empty() && !this->tempName.empty() && Nya::Utils::IsImage(this->tempName));
}

// AutoNya related stuff
void Nya::ImageView::LateUpdate()
{
    bool autoNya = getNyaConfig().AutoNya.GetValue();
    if (!autoNya || this->isLoading) return;
    int autoNyaDelay = getNyaConfig().AutoNyaDelay.GetValue();
    // Time in seconds since the last frame
    float delta = Time::get_deltaTime();
    // Add the time since the last frame to the time since the last nya
    this->timeSinceLastNya += delta;
    
    if (this->timeSinceLastNya >= (float) autoNyaDelay) {
        // Get new image
        this->GetImage(nullptr);
    }
}

// TODO: review crash related to this (https://analyzer.questmodding.com/crashes/4V2U)
//  filesystem error: in rename: No such file or directory [/sdcard/ModData/com.beatgames.beatsaber/Mods/Nya/Temp/edOwqtb4.png] [/sdcard/ModData/com.beatgames.beatsaber/Mods/Nya/Images/nsfw/edOwqtb4.png]
void Nya::ImageView::SaveImage() {
    if (this->lastImageURL.empty() || this->tempName.empty() || !Nya::Utils::IsImage(this->tempName)) {
        ERROR("Invalid image data for saving");
        return;
    }

    try {
        std::string originalPath = NyaGlobals::tempPath + this->tempName;
        std::string destPath;
        
        // Verify source file exists
        if (!FileUtils::exists(originalPath)) {
            ERROR("Source file does not exist: {}", originalPath);
            return;
        }

        // Determine and create destination directory if needed
        destPath = FileUtils::getImageDir(this->isNSFW) + this->tempName;

        INFO("Moving file from {} to {}", originalPath, destPath);
        FileUtils::moveFile(originalPath, destPath);

        if (!FileUtils::exists(destPath)) {
            ERROR("File move operation failed - destination file not found");
        }
        
        INFO("MOVED FILE");
        // Cleanup 
        this->lastImageURL = "";
        this->tempName = "";

    } catch (const std::exception& e) {
        ERROR("Failed to save image: {}", e.what());
    }

    
}

// Update
void Nya::ImageView::GetImage(std::function<void(bool success)> finished = nullptr)
{
    this->isLoading = true;
    // Reset time since last nya
    this->timeSinceLastNya = 0.0f;
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
        BSML::Utilities::SetImage(this->imageView,  "file://" + path, false, BSML::Utilities::ScaleOptions(), false, [finished, this]() {
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

     
  } else if (source->Mode == DataMode::Json) {
    std::string selectedEndpoint = EndpointConfigUtils::getEndpointValue(currentAPI, NSFWEnabled);

    // Fallback to sfw if nsfw is enabled and no nsfw endpoint is found
    if (selectedEndpoint == "" && NSFWEnabled) {
        selectedEndpoint = EndpointConfigUtils::getEndpointValue(currentAPI, false);
    }

    bool nsfwEmpty = source->NsfwEndpoints.empty();
    bool sfwEmpty = source->SfwEndpoints.empty();

    // If the endpoint is random, get a random endpoint
    if (selectedEndpoint == "random") {
        if (NSFWEnabled) {
            if (nsfwEmpty) {
                // Fallback to sfw since nsfw is empty
                NSFWEnabled = false;
                auto endpoint = NyaAPI::getRandomEndpoint(&source->SfwEndpoints);
                if (endpoint != nullptr) selectedEndpoint = endpoint->url;
            } else {
                auto endpoint = NyaAPI::getRandomEndpoint(&source->NsfwEndpoints);
                if (endpoint != nullptr) selectedEndpoint = endpoint->url;
            }
        } else {
            if (sfwEmpty) {
                selectedEndpoint = "";
            } else {
                auto endpoint = NyaAPI::getRandomEndpoint(&source->SfwEndpoints);
                if (endpoint != nullptr) selectedEndpoint = endpoint->url;
            }
        }
    }

    // Get the image url from the api
    NyaAPI::get_path_from_json_api(source, selectedEndpoint, NSFWEnabled, 10.0f, [this, finished, NSFWEnabled](bool success, std::string url) {
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
                    
                    // Check if image view is still valid and if not, don't set the image
                    if (this->imageView == nullptr || this->imageView->___m_CachedPtr.m_value == nullptr) {
                        if (finished != nullptr) finished(false);
                        return;
                    }

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
            
     
    });
  }
}

void Nya::ImageView::SetErrorImage()
{
    // If the image view is not valid, don't set the image
    if (this->imageView == nullptr || this->imageView->___m_CachedPtr.m_value == nullptr) return;
    
    Nya::Utils::RemoveAnimationUpdater(this->imageView);
    this->imageView->set_sprite(BSML::Lite::ArrayToSprite(IncludedAssets::Chocola_Dead_png));
}


void Nya::ImageView::OnNyaPhysicalClick(){
    if (this->isLoading) {
        return;
    }

    this->GetImage(nullptr);
}

void Nya::ImageView::OnEnable()
{
    // Subscribe to physical click
    Nya::GlobalEvents::onControllerNya += {&Nya::ImageView::OnNyaPhysicalClick ,this};
}

void Nya::ImageView::OnDisable()
{
    this->autoNyaRunning = false;
    // Unsubscribe to physical click
    Nya::GlobalEvents::onControllerNya -= {&Nya::ImageView::OnNyaPhysicalClick ,this};
}