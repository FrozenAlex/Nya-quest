// OurClass.hpp
#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/ImageView.hpp"

#include <functional>

DECLARE_CLASS_CODEGEN(Nya, ImageView, UnityEngine::MonoBehaviour,
  public:
    
    std::string lastImageURL;
    std::string tempName;

    // AutoNya
    float timeSinceLastNya = 0.0f;

    DECLARE_CTOR(ctor);
    DECLARE_SIMPLE_DTOR();

    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, imageView);
    DECLARE_INSTANCE_FIELD(bool, autoNyaRunning);
    DECLARE_INSTANCE_FIELD(bool, autoNyaNewImage);
    DECLARE_INSTANCE_FIELD(bool, isLoading);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, LateUpdate);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, OnNyaPhysicalClick);
    DECLARE_INSTANCE_FIELD(bool, isNSFW);
    /**
     * @brief Get a new image from currently selected source
     * 
     * @param finished Callback when the image is loaded or failed to load
     */
    void GetImage(std::function<void(bool success)> finished);
    /**
     * @brief Save the image from the cache to the images folder
     */
    void SaveImage();
    bool HasImageToSave();
    void SetErrorImage();

    // Event to sub to when image started loading, returns isLoading, meaning that the image is loading
    UnorderedEventCallback<bool> imageLoadingChange;
);