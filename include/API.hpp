#pragma once

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "beatsaber-hook/shared/utils/typedefs.h"

namespace NyaAPI {
struct EndpointCategory {
    std::string label;
    std::string url;
};

enum DataMode { Unsupported, Json, Local, Authenticated };

struct SourceData {
    // SourceData(
    //     std::string i,
    //     DataMode j,
    //     std::list<std::string> k,
    //     std::list<std::string> l) {
    //     BaseEndpoint = i;
    //     Mode = j;
    //     SfwEndpoints = k;
    //     NsfwEndpoints = l;
    // };
    std::string BaseEndpoint;
    DataMode Mode;
    std::vector<EndpointCategory> SfwEndpoints;
    std::vector<EndpointCategory> NsfwEndpoints;
    std::string propertyName;
};

// Function gets url for the current selected category
SourceData* get_data_source(std::string name);
std::vector<StringW> get_source_list();
void get_path_from_json_api(SourceData* source,
                            std::string url,
                            float timeoutInSeconds,
                            std::function<void(bool success, std::string url)> finished,
                            std::string apiKey = "");
// void NyaAPI::downloadImageFile();
std::map<std::string, SourceData>* getEndpoints();

int findSourceIndexInListC(std::vector<EndpointCategory>* values, StringW string);

ListW<StringW> listEndpointLabels(std::vector<EndpointCategory>* values);
ListW<StringW> listEndpointUrls(std::vector<EndpointCategory>* values);

// Get random endpoint from the list
EndpointCategory* getRandomEndpoint(std::vector<EndpointCategory>* values);
}  // namespace NyaAPI
