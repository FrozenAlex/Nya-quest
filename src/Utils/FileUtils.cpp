#include "Utils/FileUtils.hpp"
#include "NyaConfig.hpp"
#include "Utils/Utils.hpp"
#include <vector>
#include <filesystem>
#include "System/IO/Path.hpp"
#include "logging.hpp"
#include "main.hpp"
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
    bool deleteFile(std::string path){
        if (fs::exists((std::string) path)) {
            return fs::remove((std::string) path);
        }
        return false;
    }

    // Gets all supported files
    void moveFile(std::string oldPath,std::string newPath){
        if (fs::exists((std::string) oldPath)) {
            fs::rename((std::string) oldPath, (std::string) newPath);
        }
    }

    std::string GetFileFormat(std::string path) {
        return System::IO::Path::GetExtension(path);
    }

    // Create a directory if it does not exist (recursively)
    bool createDirectoryIfNotExists(const std::string& path) {
        if (!fs::exists(path)) {
            return fs::create_directories(path);
        }
        return true;
    }

    bool exists(const std::string& path) {
        return fs::exists(path);
    }
    std::string getImageDir(bool isNSFW) {
        std::string path;
        if (isNSFW) {
            path = NyaGlobals::imagesPathNSFW;
        } else {
            path = NyaGlobals::imagesPathSFW;
        }
        if (!fs::exists(path)) {
            INFO("Image directory does not exist, creating: {}", path);
            createDirectoryIfNotExists(path);
            if (!fs::exists(path)) {
                ERROR("Failed to create image directory: {}", path);
                return "";
            }
            Nya::ApplyIndexingRules();
        }

        return path;
    }
}