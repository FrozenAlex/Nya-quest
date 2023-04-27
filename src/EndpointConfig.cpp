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
                    if (data.Mode == DataMode::Json || data.Mode == DataMode::Authenticated) {
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

        // If the source setting key does not exist, create it
        if (!endpoints.HasMember(name)) {

            // Create field
            rapidjson::Value fieldname;
            fieldname.SetString(name, allocator);
            endpoints.AddMember(fieldname, rapidjson::Value(rapidjson::kObjectType), allocator);
        }

        // Get the endpoint json object
        auto endpoint = endpoints[name].GetObject();
    
        // Check if it's sfw or nsfw and get appropriate stuff
        std::string member = nsfw ? "nsfw" : "sfw";
        std::vector<NyaAPI::EndpointCategory>* endpoint_data = nsfw ? &source_data->NsfwEndpoints : &source_data->SfwEndpoints;       

        // If the value is present, load it
        if (endpoint.HasMember(member)) {
            std::string value = endpoint[member].GetString();
            endpointValue = value;
        // If not, then try to find it an initialize or skip
        } else {
            if (endpoint_data->size() > 0) {
                rapidjson::Value member_fieldname;
                member_fieldname.SetString(member, allocator);
                // Process sfw default
                endpoint.AddMember(member_fieldname, rapidjson::Value(rapidjson::kStringType), allocator);
                endpoint[member_fieldname].SetString(endpoint_data->front().url, allocator);
                // Mark to save at the end
                save = true;
                endpointValue = endpoint_data->front().url;
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
    /// @param endpointUrl endpoint category url chunk
    void updateEndpointValue(Configuration* config, std::string name, bool nsfw, std::string endpointUrl) {
        auto& cfg = config->config;
        auto& allocator = cfg.GetAllocator();

        bool save = false;

        auto endpoints = cfg["endpoints"].GetObject();

        // If the source setting key exists
        // We don't need to init because it's done on load of the settings page
        if (endpoints.HasMember(name)) {
            auto endpoint = endpoints[name].GetObject();
  
            // Variable to store the endpoint url
            std::string url = endpointUrl;

            // Check if it's sfw or nsfw
            std::string member = nsfw ? "nsfw" : "sfw";
            
            // Save the endpoint to appropriate slot
            if (endpoint.HasMember(member)) {
                endpoint[member].SetString(url, allocator);
                save = true;
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