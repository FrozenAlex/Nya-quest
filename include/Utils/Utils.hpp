#pragma once
#include <string>

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "main.hpp"
#include "ImageView.hpp"
#include "Utils/FileUtils.hpp"

namespace Nya::Utils {
    bool IsGif(std::string str);
    std::string ToLowercase(std::string str);
    ListW<StringW> vectorToList(std::vector<StringW> values);
    std::vector<StringW> listWToVector(List<StringW>* values);
    int findStrIndexInList(ListW<StringW> values, StringW string );
    // Kill me
    int findStrIndexInListC(std::list<std::string> values, StringW string );
    int random(int min, int max);
    ListW<StringW> listStringToStringW(std::list<std::string> values);

    bool IsAnimated(std::string str);

    bool IsImage(std::string str);
    std::string RandomString(const int len);
    VRUIControls::VRPointer* getAnyPointerWithController();

    /// @brief function to get data at a URI and save it to a file
    /// @param uri the URI to get data from
    /// @param path path to the file
    /// @param onFinished what to do with the downloaded bytes
    void DownloadFile(std::string uri, std::string path, std::function<void(bool success, std::string path)> onFinished);
}

