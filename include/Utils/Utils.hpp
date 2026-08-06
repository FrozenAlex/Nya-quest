#pragma once
#include <string>

#include "beatsaber-hook/shared/stringw.hpp"
#include "beatsaber-hook/shared/listw.hpp"

#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/Vector2.hpp"
#include "components/CustomTextSegmentedControlData.hpp"

namespace Nya::Utils {
    bool IsGif(std::string str);
    std::string ToLowercase(std::string str);
    ListW<StringW> vectorToList(std::vector<StringW> values);
    int findStrIndexInList(ListW<StringW> values, StringW string );
    NyaUI::CustomTextSegmentedControlData* CreateTextSegmentedControl(UnityEngine::Transform* parent, UnityEngine::Vector2 anchoredPosition, UnityEngine::Vector2 sizeDelta, ArrayW<StringW> values, std::function<void(int)> onCellWithIdxClicked);
    // Kill me
    int findStrIndexInListC(std::list<std::string> values, StringW string );
    int random(int min, int max);
    ListW<StringW> listStringToStringW(std::list<std::string> values);

    bool IsAnimated(std::string str);

    bool IsImage(std::string str);
    std::string RandomString(const int len);

    /// @brief function to get data at a URI and save it to a file
    /// @param uri the URI to get data from
    /// @param path path to the file
    /// @param onFinished what to do with the downloaded bytes
    void DownloadFile(std::string uri, std::string path, std::function<void(bool success, std::string path)> onFinished);

    void SetButtonSize(UnityW<UnityEngine::UI::Button> button, UnityEngine::Vector2 size);

    /// @brief Function to remove the bsml animation updater from an image
    /// @param image the image to remove the updater from
    /// @attention This function should only be called on the main thread
    void RemoveAnimationUpdater(UnityW<UnityEngine::UI::Image> image);
}
