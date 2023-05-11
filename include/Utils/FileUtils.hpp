#pragma once
#include <string>
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace FileUtils {
    bool deleteFile(std::string path);
    void moveFile(std::string oldPath,std::string newPath);
    std::string GetFileFormat(std::string path);

    std::string FixIlegalName(std::string_view path);
    std::vector<std::string> getAllFoldersInFolder(const std::string& path);
    std::vector<std::string> getAllFilesInFolder(const std::string& path);
}