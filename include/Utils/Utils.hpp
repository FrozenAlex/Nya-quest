#pragma once
#include <string>

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "main.hpp"
#include "ImageView.hpp"
#include "Utils/FileUtils.hpp"

namespace Nya::Utils {
    List<StringW>* vectorToList(std::vector<StringW> values);
    std::vector<StringW> listWToVector(List<StringW>* values);
    int findStrIndexInList(List<StringW>* values, StringW string );
    // Kill me
    int findStrIndexInListC(std::list<std::string> values, StringW string );
    int random(int min, int max);
    ListWrapper<StringW> listStringToStringW(std::list<std::string> values);

    // Gets the next image
    void onNyaClick(UnityEngine::UI::Button * button,NyaUtils::ImageView * imageView);
    void onSettingsClick(
        HMUI::ModalView* settingsModal,
        HMUI::SimpleTextDropdown* api_switch,
        HMUI::SimpleTextDropdown* sfw_endpoint,
        HMUI::SimpleTextDropdown* nsfw_endpoint,
        UnityEngine::UI::Toggle* nsfw_toggle
    );
    bool IsAnimated(StringW str);

    bool IsImage(StringW str);
}

