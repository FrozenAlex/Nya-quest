
#include "Utils/FileUtils.hpp"
#include "Utils/Utils.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "codegen/include/System/IO/Path.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
namespace fs = std::filesystem;

namespace FileUtils {

    std::string FixIlegalName(std::string_view path) {
        static const std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()[]{}%&.:,;=!-_ ";
        std::string newPath;
        newPath.reserve(path.size());  
        std::copy_if(path.cbegin(), path.cend(), std::back_inserter(newPath), 
            [] (char c) { 
                return allowedChars.find(c) != std::string::npos;
            }
        );
        return newPath;
    }

    std::vector<std::string> getAllFoldersInFolder(const std::string& path) {
        std::vector<std::string> strings;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                strings.push_back(entry.path());
            }
        }
        return strings;
    }


    /*
    * Find Case Insensitive Sub String in a given substring
    */
    size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0)
    {
        // Convert complete given String to lower case
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        // Convert complete given Sub String to lower case
        std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
        // Find sub string in given string
        return data.find(toSearch, pos);
    }

    // Gets all supported files
    std::vector<std::string> getAllFilesInFolder(const std::string& path){
         std::vector<std::string> strings;
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                StringW path = StringW(entry.path().c_str());
                if (Nya::Utils::IsImage(path)) {
                    strings.push_back(entry.path());
                }            
            }
        }
        INFO("Found {} files", strings.size());
        return strings;
    }

    // Gets all supported files
    bool deleteFile(StringW path){
        if (fs::exists((std::string) path)) {
            return fs::remove((std::string) path);
        }
        return false;
    }

    // Gets all supported files
    void moveFile(StringW oldPath,StringW newPath){
        if (fs::exists((std::string) oldPath)) {
            fs::rename((std::string) oldPath, (std::string) newPath);
        }
    }

    StringW GetFileFormat(StringW path) {
        return System::IO::Path::GetExtension(path);
    }
}