#pragma once

#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Color32.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "System/Uri.hpp"

namespace FSML::Utilities {
    /// @brief Finds a sprite by name.
    UnityEngine::Sprite* FindSpriteCached(StringW name);

    /// @brief Finds a texture by name.
    UnityEngine::Texture* FindTextureCached(StringW name);

    struct ScaleOptions {
        int width;
        int height;
        bool maintainRatio;
        bool shouldScale;

        ScaleOptions(int width = 0, int height = 0, bool maintainRatio = true, bool shouldScale = false)
            : width(width)
            , height(height)
            , maintainRatio(maintainRatio)
            , shouldScale(shouldScale)
        {}
    };

    /// @brief Downscales a texture using a rendertexture (I know, not the most efficient way to do it)
    /// @param tex the texture to copy and downscale
    /// @param options the options to use for downscaling
    /// @return downscaled texture, or tex if it was not scaled
    UnityEngine::Texture2D* DownScaleTexture(UnityEngine::Texture2D* tex, const ScaleOptions& options);

    /// @brief Downscales a sprite using DownScaleTexture
    /// @param sprite the sprite to copy and downscale
    /// @param options the options to use for downscaling
    /// @return downscaled sprite
    UnityEngine::Sprite* DownScaleSprite(UnityEngine::Sprite* sprite, const ScaleOptions& options);


    /// @brief sets the sprite from a path, this does not yet bother with animated textures so it will only work with base textures
    /// @param image the image to set the sprite on
    /// @param path the path, basegame name, or URI to an image
    void SetImage(UnityEngine::UI::Image* image, StringW path);

    /// @brief sets the sprite from a path, this does not yet bother with animated textures so it will only work with base textures
    /// @param image the image to set the sprite on
    /// @param path the path, basegame name, or URI to an image
    /// @param loadingAnimation currently unused, but might eventually be used for a loading animation while stuff is downloading
    /// @param scaleOptions the scale options to use, if any
    /// @param onFinished a callback to call when the image is finished loading, if any
    void SetImage(UnityEngine::UI::Image* image, StringW path, bool loadingAnimation, ScaleOptions scaleOptions, std::function<void()> onFinished = nullptr);

    /// @brief function to get data at a URI, this is not blocking as it dispatches a coroutine
    /// @param uri the URI to get data from
    /// @param onFinished what to do with the downloaded bytes
    void DownloadData(StringW uri, std::function<void(bool success, ArrayW<uint8_t>)> onFinished = nullptr);

    /// @brief function to get data from the key to data cache, this is blocking due to the data being available immediately
    /// @param key the key to get the data with
    /// @param onFinished what to do with the downloaded bytes
    void GetData(StringW key, std::function<void(bool success, ArrayW<uint8_t>)> onFinished = nullptr);

    /// @brief Function to load a sprite from a data array
    /// @param data the data to load the sprite from
    /// @param pixelsPerUnit the pixels per unit
    /// @return created sprite
    UnityEngine::Sprite* LoadSpriteRaw(ArrayW<uint8_t> data, float pixelsPerUnit = 100.0f);

    /// @brief Function to load a sprite from a texture
    /// @param texture the texture to use
    /// @param pixelsPerUnit the pixels per unit
    /// @return created sprite
    UnityEngine::Sprite* LoadSpriteFromTexture(UnityEngine::Texture2D* texture, float pixelsPerUnit = 100.0f);

    /// @brief Function to load a texture from a data array
    /// @param data the data to load the texture from
    /// @return created texture
    UnityEngine::Texture2D* LoadTextureRaw(ArrayW<uint8_t> data);

    /// @brief function to copy the values from other to comp
    /// @param comp the component to copy to
    /// @param other the component to copy from
    /// @return comp, or nullptr if types are different
    UnityEngine::Component* GetCopyOfComponent(UnityEngine::Component* comp, UnityEngine::Component* other);

    /// @brief function to copy the values from other to comp, casted to the correct type
    /// @tparam T the type of the component
    /// @param comp the component to copy to
    /// @param other the component to copy from
    /// @return comp, or nullptr if types are different
    template<typename T>
    requires(std::is_convertible_v<T, UnityEngine::Component*>)
    T GetCopyOf(UnityEngine::Component* comp, T other) {
        return reinterpret_cast<T>(GetCopyOfComponent(comp, other));
    }

    /// @brief function to copy a component onto gameObject
    /// @tparam T the type of the component
    /// @param gameObject the gameObject to add the component to
    /// @param original the component to copy from
    /// @return newly added component
    template<typename T>
    requires(std::is_convertible_v<T, UnityEngine::Component*>)
    T AddComponent(UnityEngine::GameObject* gameObject, T original) {
        return GetCopyOf<T>(gameObject->AddComponent<T>(), original);
    }

    void RemoveAnimationUpdater(UnityEngine::UI::Image* image);
    
    namespace ImageResources {
        /// @brief method that gets a blank sprite
        /// @return black texture to sprite
        UnityEngine::Sprite* GetBlankSprite();
        
        /// @brief method that gets a white pixel sprite
        /// @return white pixel to sprite
        UnityEngine::Sprite* GetWhitePixel();
    }
}