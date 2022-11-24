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
                "sfw/neko", "sfw/waifu", "sfw/awoo", "sfw/shinobu", "sfw/megumin", "sfw/cuddle", "sfw/cry",
                "sfw/hug", "sfw/kiss", "sfw/lick", "sfw/pat", "sfw/smug", "sfw/bonk", "sfw/yeet", "sfw/blush",
                "sfw/smile", "sfw/wave", "sfw/highfive", "sfw/nom", "sfw/bite", "sfw/glomp", "sfw/slap",
                "sfw/kick", "sfw/happy", "sfw/wink", "sfw/poke", "sfw/dance"
            }, // Not including 'kill' because it's bad
            {
                "nsfw/neko", "nsfw/waifu", "nsfw/trap", "nsfw/blowjob"
            },
            "url"
        }
    },
    {"Anime-Images API",
        {
            "https://anime-api.hisoka17.repl.co/img/",
            DataMode::Json,
            {
                "hug", "kiss", "slap", "wink", "pat", "kill", "cuddle", "punch", "waifu"
            },
            {
                "hentai", "nsfw/boobs", "nsfw/lesbian" 
            },
            "url"
        }
    },
    {"Catboys",
        {
            "https://api.catboys.com/",
            DataMode::Json,
            {
                "img", "baka"
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
                "neko", "waifu", "tickle", "slap", "pat", "meow", "lizard", "kiss", "hug", "fox_girl",
                "feed", "cuddle", "ngif", "smug", "woof", "wallpaper", "goose", "gecg", "avatar"
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
                "cat", "dog", "fox", "panda", "red_panda", "birb", "koala", "kangaroo", "raccoon"
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
