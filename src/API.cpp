#include "API.hpp"
#include "WebUtils.hpp"
#include "main.hpp"
using namespace Nya;



inline std::map<std::string, SourceData> endpoint_data = {
    {"waifu.pics",
        {
            "https://api.waifu.pics/",
            DataMode::Json,
            {
                {"neko", "sfw/neko"},
                {"waifu", "sfw/waifu"},
                {"awoo", "sfw/awoo"},
                {"shinobu", "sfw/shinobu"},
                {"megumin", "sfw/megumin"},
                {"cuddle", "sfw/cuddle"},
                {"cry", "sfw/cry"},
                {"hug", "sfw/hug"},
                {"kiss", "sfw/kiss"},
                {"lick", "sfw/lick"},
                {"pat", "sfw/pat"},
                {"smug", "sfw/smug"},
                {"bonk", "sfw/bonk"},
                {"yeet", "sfw/yeet"},
                {"blush", "sfw/blush"},
                {"smile", "sfw/smile"},
                {"wave", "sfw/wave"},
                {"highfive", "sfw/highfive"},
                {"nom", "sfw/nom"},
                {"bite", "sfw/bite"},
                {"glomp", "sfw/glomp"},
                {"slap", "sfw/slap"},
                {"kick", "sfw/kick"},
                {"happy", "sfw/happy"},
                {"wink", "sfw/wink"},
                {"poke", "sfw/poke"},
                {"dance", "sfw/dance"},
            }, // Not including 'kill' because it's bad
            {
                {"neko", "nsfw/neko"},
                {"waifu", "nsfw/waifu"},
                {"trap", "nsfw/trap"},
                {"blowjob", "nsfw/blowjob"},
            },
            "url"
        }
    },
    {"Anime-Images API",
        {
            "https://anime-api.hisoka17.repl.co/img/",
            DataMode::Json,
            {
                {"hug", "hug"},
                {"kiss", "kiss"},
                {"slap", "slap"},
                {"wink", "wink"},
                {"pat", "pat"},
                {"kill", "kill"},
                {"cuddle", "cuddle"},
                {"punch", "punch"},
                {"waifu", "waifu"},
            },
            {
                {"hentai", "hentai"},
                {"boobs", "nsfw/boobs"},
                {"lesbian", "nsfw/lesbian"},
            },
            "url"
        }
    },
    {"Catboys",
        {
            "https://api.catboys.com/",
            DataMode::Json,
            {
                {"catboys", "img"},
                {"baka", "baka"},
            },
            {},
            "url"
        }
    },
    {"nekos.life",
        {
            "https://nekos.life/api/v2/img/",
            DataMode::Json,
            {
                {"neko", "neko"},
                {"waifu", "waifu"},
                {"tickle", "tickle"},
                {"slap", "slap"},
                {"pat", "pat"},
                {"meow", "meow"},
                {"lizard", "lizard"},
                {"kiss", "kiss"},
                {"hug", "hug"},
                {"fox girl", "fox_girl"},
                {"feed", "feed"},
                {"cuddle", "cuddle"},
                {"ngif", "ngif"},
                {"smug", "smug"},
                {"woof", "woof"},
                {"wallpaper", "wallpaper"},
                {"goose", "goose"},
                {"gecg", "gecg"},
                {"avatar", "avatar"}
            },
            {},
            "url"
        }
    },
    {"Bocchi",
        {
            "https://boccher.pixelboom.dev/api/",
            DataMode::Json,
            {
                {"all", "frames"},
                {"episode 1", "frames?episode=1"},
                {"episode 2", "frames?episode=2"},
                {"episode 3", "frames?episode=3"},
                {"episode 4", "frames?episode=4"},
                {"episode 5", "frames?episode=5"},
                {"episode 6", "frames?episode=6"},
                {"episode 7", "frames?episode=7"},
                {"episode 8", "frames?episode=8"},
                {"episode 9", "frames?episode=9"},
                {"episode 10", "frames?episode=10"},
                {"episode 11", "frames?episode=11"},
                {"episode 12", "frames?episode=12"},
                {"opening", "frames?episode=OP"},
                {"ending 1", "frames?episode=ED1"},
                {"ending 2", "frames?episode=ED2"},
                {"ending 3",  "frames?episode=ED3"},   
            },
            {},
            "url"
        }
    }, 
    {"Animals",
        {
            "https://some-random-api.ml/animal/",
            DataMode::Json,
            {
                {"cat", "cat"},
                {"dog", "dog"},
                {"fox", "fox"},
                {"panda", "panda"},
                {"red panda", "red_panda"},
                {"bird", "bird"},
                {"koala", "koala"},
                {"kangaroo", "kangaroo"},
                {"raccoon", "raccoon"}
            },
            {},
            "image"
        }
    },
    {"Local Files",
        {
            "",
            DataMode::Local,
            {},
            {},
            ""
        }
    }
};

std::map<std::string, SourceData>* NyaAPI::getEndpoints() {
    return &endpoint_data;
}

SourceData* NyaAPI::get_data_source(std::string name) {
    if (name.empty()) {
        throw "String is empty";
    }
    if (endpoint_data.find(name) == endpoint_data.end()) {
        throw "Endpoint not found";
    }
    else return &(endpoint_data[name]);
}

std::vector<StringW> NyaAPI::get_source_list() {
    std::vector<StringW> keys;
    for (const auto& [key, _] : endpoint_data) {
        keys.push_back(key);
    }
    return keys;
}

// Grabs path to image from api
void NyaAPI::get_path_from_json_api(
    SourceData* source,
    std::string url,
    float timeoutInSeconds,
    std::function<void(bool success, std::string url)> finished) {
    WebUtils::GetAsync(url, 10.0, [&, finished, source](long code, std::string result){
        if (code != 200) {
            if(finished != nullptr) finished(false, "");
            return;
        }

        rapidjson::Document document;
        document.Parse(result);
        if(document.HasParseError() || !document.IsObject()) {
            if(finished != nullptr) finished(false, "");
            return;
        }
            
        std::string url = "";
        if(document.HasMember(source->propertyName))
        {
            url = document.FindMember(source->propertyName)->value.GetString();
            if(finished != nullptr) finished(true, url);
        } else {
            if(finished != nullptr) finished(false, "");
            return;
        }
        
    });
}

/**
     * @brief Finds the string in a list
     * 
     * @param values 
     * @param string 
     * @return int -1 if not found anything or index of the element if the item is found
     */
    int  NyaAPI::findSourceIndexInListC(std::vector<EndpointCategory>* values, StringW string ) {
        
        // Create iterator pointing to first element
        std::vector<EndpointCategory>::iterator it = values->begin();

        for (int i = 0; i < values->size(); i++){
            auto value = *it;
            if (value.url == string) {
                return i;
            }
            std::advance(it, 1);
        }
        return -1;
    }

    ListWrapper<StringW> NyaAPI::listEndpointLabels(std::vector<EndpointCategory>* values) {
        // TODO: Fix
        int count = values->size();
        
        // Convert stuff to list
        ListWrapper<StringW> list(List<StringW>::New_ctor());
        if (count == 0) {
            return list;
        }
        list->EnsureCapacity(count);
        for (auto item : *values) {list->Add(item.label);};

        return list;
    }