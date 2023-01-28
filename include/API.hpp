#pragma once
#include "NyaConfig.hpp"
#include "main.hpp"


#include <string>
#include <map>
#include <list>
#include <vector>

#include <iostream>

struct EndpointCategory {
    std::string label;
    std::string url;
};

enum DataMode
{
    Unsupported,
    Json,
    Local
};

struct SourceData
{
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


namespace NyaAPI {
    // Function gets url for the current selected category
    SourceData* get_data_source(std::string name);
    std::vector<StringW> get_source_list();
    void get_path_from_json_api(SourceData* source, std::string url, float timeoutInSeconds,std::function<void(bool success, std::string url)> finished);
    // void NyaAPI::downloadImageFile();
    std::map<std::string, SourceData>* getEndpoints();

    int findSourceIndexInListC(std::vector<EndpointCategory>* values, StringW string );

    ListWrapper<StringW> listEndpointLabels(std::vector<EndpointCategory>* values);
}