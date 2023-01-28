#include "EndpointConfig.hpp"

using namespace Nya;

namespace EndpointConfig {
    

    // you do one tiny little bit of jank in your config, and you end up with migration code in your mod forever
    void migrate(Configuration* config) {
        auto& cfg = config->config;
        auto& allocator = cfg.GetAllocator();
        // Has stuff?
        if (!cfg.HasMember("endpoints")) {
            
            cfg.AddMember("endpoints", rapidjson::Value(rapidjson::kObjectType), allocator);
            auto endpoints = cfg["endpoints"].GetObject();
            
            // Init endpoints
            auto endpoint_data = NyaAPI::getEndpoints();
            
            // Add default endpoints
            for (const auto& [key, data] : *endpoint_data) {
                if (!endpoints.HasMember(key)) {
                    rapidjson::Value fieldname;
                    fieldname.SetString(key, allocator);
                    // Value
                    endpoints.AddMember(fieldname, rapidjson::Value(rapidjson::kObjectType), allocator);
                    if (data.Mode == DataMode::Json) {
                        if (data.NsfwEndpoints.size() > 0 ||data.SfwEndpoints.size() >0 ) {
                            // Get the endpoint
                            auto endpoint = endpoints[key].GetObject();

                            if (data.SfwEndpoints.size() > 0) {
                                // Process sfw default
                                endpoint.AddMember("sfw", rapidjson::Value(rapidjson::kStringType), allocator);
                                endpoint["sfw"].SetString(data.SfwEndpoints.front().url, allocator);
                            }
                            if (data.NsfwEndpoints.size() > 0) {
                                // Process nsfw default
                                endpoint.AddMember("nsfw", rapidjson::Value(rapidjson::kStringType), allocator);
                                endpoint["nsfw"].SetString(data.NsfwEndpoints.front().url, allocator);
                            }
                            
                        }
                    }
                }
            }
            // save
            INFO("Migrated config");
            config->Write();
        }
    }


    // Gets endpoint from the config
    std::string getEndpointValue(Configuration* config, std::string name, bool nsfw) {
        auto& cfg = config->config;
        auto& allocator = cfg.GetAllocator();

        auto source_data = NyaAPI::get_data_source(name);
        bool save = false;

        // Store the endpoint value to return later (if none is found, just return "")
        std::string endpointValue = "";


        auto endpoints = cfg["endpoints"].GetObject();

        // If the source setting key exists
        if (endpoints.HasMember(name)) {

            auto endpoint = endpoints[name].GetObject();
        
            // Load the nsfw value
            if (nsfw) {

                // If the value is present, load it
                if (endpoint.HasMember("nsfw")) {
                    std::string value = endpoint["nsfw"].GetString();
                    endpointValue = value;
                // If not, then try to find it an initialize or skip
                } else {
                    if (source_data->NsfwEndpoints.size() > 0) {
                        // Process sfw default
                        endpoint.AddMember("nsfw", rapidjson::Value(rapidjson::kStringType), allocator);
                        endpoint["nsfw"].SetString(source_data->NsfwEndpoints.front().url, allocator);
                        // Mark to save at the end
                        save = true;
                        endpointValue = source_data->NsfwEndpoints.front().url;
                    }
                }
                
            } 
            if (!nsfw) {
                if (endpoint.HasMember("sfw")) {
                    std::string value = endpoint["sfw"].GetString();
                    endpointValue = value;
                } else {
                    if (source_data->SfwEndpoints.size() > 0) {
                        // Process sfw default
                        endpoint.AddMember("sfw", rapidjson::Value(rapidjson::kStringType), allocator);
                        endpoint["sfw"].SetString(source_data->SfwEndpoints.front().url, allocator);
                        save = true;
                        config->Write();
                        endpointValue = source_data->SfwEndpoints.front().url;
                    }
                }
            }
        // If source key does not exist then init and return default category for the thing
        } else {
            // Add endpoint and init it if it has no name

            rapidjson::Value fieldname;
            fieldname.SetString(name, allocator);
            // Value
            endpoints.AddMember(fieldname, rapidjson::Value(rapidjson::kObjectType), allocator);
            if (
                source_data->Mode == DataMode::Json &&
                (source_data->NsfwEndpoints.size() > 0 || source_data->SfwEndpoints.size() > 0)
            ) {
                // Get the endpoint
                auto endpoint = endpoints[name].GetObject();

                // Init sfw
                if (source_data->SfwEndpoints.size() > 0) {
                    // Process sfw default
                    endpoint.AddMember("sfw", rapidjson::Value(rapidjson::kStringType), allocator);
                    endpoint["sfw"].SetString(source_data->SfwEndpoints.front().url, allocator);
                    // Set endpoint value if it matches
                    if (!nsfw) {
                        endpointValue = source_data->SfwEndpoints.front().url;
                    }
                }

                // Init nsfw
                if (source_data->NsfwEndpoints.size() > 0) {
                    // Process nsfw default
                    endpoint.AddMember("nsfw", rapidjson::Value(rapidjson::kStringType), allocator);
                    endpoint["nsfw"].SetString(source_data->NsfwEndpoints.front().url, allocator);
                    // Set endpoint value if it matches
                    if (nsfw) {
                        endpointValue = source_data->NsfwEndpoints.front().url;
                    }
                }
            }
        }

        // If config needs saving, save
        if (save) {
            config->Write();
        }

        // TODO: Check if the value exists and reinitialize it to default if it does not
        return endpointValue;
    }

    /// @brief Updates API config value
    /// @param config config object
    /// @param name name of the selected API
    /// @param nsfw is nsfw or no
    /// @param value endpoint category
    void updateEndpointValue(Configuration* config, std::string name, bool nsfw, std::string value) {
        auto& cfg = config->config;
        auto& allocator = cfg.GetAllocator();

        auto source_data = NyaAPI::get_data_source(name);
        bool save = false;

        auto endpoints = cfg["endpoints"].GetObject();

        // If the source setting key exists
        // We don't need to init because it's done on load of the settings page
        if (endpoints.HasMember(name)) {
            
            auto endpoint = endpoints[name].GetObject();
            auto dataSource = NyaAPI::get_data_source(name);

            std::string url = "";

            if (nsfw) {
                auto it = std::find_if(dataSource->NsfwEndpoints.begin(), dataSource->NsfwEndpoints.end(), [value](const EndpointCategory& s) { return s.label == value; });
                if (it != dataSource->NsfwEndpoints.end()) {
                    url = it->url;
                } else {
                    WARNING("Source not found wtf lel");
                }
            } else {
                auto it = std::find_if(dataSource->SfwEndpoints.begin(), dataSource->SfwEndpoints.end(), [value](const EndpointCategory& s) { return s.label == value; });
                if (it != dataSource->SfwEndpoints.end()) {
                    url = it->url;
                } else {
                    WARNING("Source not found wtf lel");
                }
            }

            // Save values depending on the thing
            if (nsfw) {
                // If the value is present, load it
                if (endpoint.HasMember("nsfw")) {
                    endpoint["nsfw"].SetString(url, allocator);
                    save = true;
                }
            } 
            
            if (!nsfw) {
                if (endpoint.HasMember("sfw")) {
                    endpoint["sfw"].SetString(url, allocator);
                    save = true;
                }
            }
        }

        // If config needs saving, save
        if (save) {
            config->Write();
        }
    }

    void ResetPositions() {
        INFO("Resetting configs");
        NyaConfig_t & config = getNyaConfig();
        // Positions
        config.pausePositionX.SetValue(config.pausePositionX.GetDefaultValue());
        config.pausePositionY.SetValue(config.pausePositionY.GetDefaultValue());
        config.pausePositionZ.SetValue(config.pausePositionZ.GetDefaultValue());

        config.menuPositionX.SetValue(config.menuPositionX.GetDefaultValue());
        config.menuPositionY.SetValue(config.menuPositionY.GetDefaultValue());
        config.menuPositionZ.SetValue(config.menuPositionZ.GetDefaultValue());

        // Rotations
        config.pauseRotationX.SetValue(config.pauseRotationX.GetDefaultValue());
        config.pauseRotationY.SetValue(config.pauseRotationY.GetDefaultValue());
        config.pauseRotationZ.SetValue(config.pauseRotationZ.GetDefaultValue());

        config.menuRotationX.SetValue(config.menuRotationX.GetDefaultValue());
        config.menuRotationY.SetValue(config.menuRotationY.GetDefaultValue());
        config.menuRotationZ.SetValue(config.menuRotationZ.GetDefaultValue());
    }
}