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
#include "WebUtils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "assets.hpp"

#include "Helpers/utilities.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Utils.hpp"

// Necessary
DEFINE_TYPE(NyaUtils, ImageView);

using namespace UnityEngine;


// Start
void NyaUtils::ImageView::ctor()
{
    this->lastImageURL = "";
    this->isNSFW = false;
    // Temp File name
    this->tempName= "";
    imageView = this->get_gameObject()->GetComponent<HMUI::ImageView *>();
}

bool NyaUtils::ImageView::HasImageToSave() {
    return (this->lastImageURL != "" && this->tempName != "");
}

void NyaUtils::ImageView::SaveImage() {
    if (this->lastImageURL != "" && this->tempName != "") {
        INFO("MOVING FILE");
        StringW original = StringW(NyaGlobals::tempPath) + this->tempName;
        if (this->isNSFW) {
            INFO("MOVING FROM {} TO {}", (std::string) original, (std::string) (StringW(NyaGlobals::imagesPathNSFW) + this->tempName) );
            FileUtils::moveFile(original, StringW(NyaGlobals::imagesPathNSFW) + this->tempName);
            INFO("MOVED FILE NSFW");
        } else {
            INFO("MOVING FROM {} TO {}", (std::string) original, (std::string) (StringW(NyaGlobals::imagesPathNSFW) + this->tempName) );
            FileUtils::moveFile(original, StringW(NyaGlobals::imagesPathSFW) + this->tempName);
            INFO("MOVED FILE SFW");
        }
        
        // Cleanup 
        this->lastImageURL = "";
        this->tempName = "";
    }
}

// Update
void NyaUtils::ImageView::GetImage(std::function<void(bool success)> finished)
{
    // Delete the last downloaded image
    if (this->lastImageURL != "" && this->tempName != "") {
        StringW original = StringW(NyaGlobals::tempPath) + this->tempName;
        FileUtils::deleteFile(original);
        
        // Reset 
        this->lastImageURL = "";
        this->tempName = "";
    }


    // Download file, get the file from internal storage
    bool NSFWEnabled = false;

    #ifdef NSFW
        NSFWEnabled = getNyaConfig().NSFWEnabled.GetValue();
    #endif
    // Get value
    std::string currentAPI = getNyaConfig().API.GetValue();
    // TODO: Make dynamic
    SourceData* source =  NyaAPI::get_data_source(currentAPI);

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
        if (finished != nullptr) finished(false);
        
      } else {
        int randomIndex = Nya::Utils::random(0, fileList.size()-1);

        auto path = fileList[randomIndex];
        BSML::Utilities::SetImage(this->imageView, "file://" + path,  true, BSML::Utilities::ScaleOptions(),[finished, this]() {
            if (finished != nullptr) finished(true);
        });
      }

     
  } else if (source->Mode == DataMode::Json) {
    
      
    // Construct the url
    // TODO: check if endpoint from the setting exists and make it dynamic

    std::string endpointValue = EndpointConfig::getEndpointValue(getNyaConfig().config, currentAPI, NSFWEnabled);

    // If we found no nsfw, show sfw
    if (endpointValue == "" && NSFWEnabled) {
        endpointValue = EndpointConfig::getEndpointValue(getNyaConfig().config, currentAPI, false);
    }

    std::string endpointURL = source->BaseEndpoint + endpointValue;

    INFO("Endpoint URL: {}", endpointURL);
    NyaAPI::get_path_from_json_api(source, endpointURL, 10.0f, [this, finished, NSFWEnabled](bool success, std::string url) {
        INFO("Image URL: {}", url);
        if (success) {
            QuestUI::MainThreadScheduler::Schedule([this, url, finished, NSFWEnabled]{
                // Make temp file name
                StringW fileExtension = FileUtils::GetFileFormat(url);
                StringW fileName = Nya::Utils::RandomString(8);

                StringW filePath = StringW(NyaGlobals::tempPath) + fileName  + fileExtension;
                StringW fileFullName = fileName  + fileExtension;

                BSML::Utilities::DownloadFile(url, filePath, [this, finished, url, NSFWEnabled, fileFullName](bool success, StringW path) {
                    if (!success ) {
                        this->SetErrorImage();
                        if (finished != nullptr) finished(false);
                    } else {
                        this->lastImageURL = url;
                        this->tempName = fileFullName;
                        this->isNSFW = NSFWEnabled;

                        BSML::Utilities::SetImage(this->imageView, "file://" + path,  true, BSML::Utilities::ScaleOptions(),[finished, this]() {
                            if (finished != nullptr) finished(true);
                        });
                    }
                  
                });
                
            });
            
        } else {
            // Error getting things
            ERROR("Failed to load image from api");
            // getLogger().Backtrace(20);
            QuestUI::MainThreadScheduler::Schedule([this, finished]{
                this->SetErrorImage();
                if (finished != nullptr) finished(false);
            });
        }
    });
  }

           
}

void NyaUtils::ImageView::SetErrorImage()
{
    BSML::Utilities::RemoveAnimationUpdater(this->imageView);
    this->imageView->set_sprite(QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::Chocola_Dead_png));
}

void NyaUtils::ImageView::dtor()
{
}