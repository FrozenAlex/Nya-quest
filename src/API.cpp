#include "API.hpp"
#include "WebUtils.hpp"
#include "main.hpp"
using namespace Nya;



inline std::map<std::string, SourceData> endpoint_data = {
    {"fluxpoint.dev",
        {
            "https://gallery.fluxpoint.dev/api/",
            DataMode::Authenticated,
            {
                // Images
                {"anime", "sfw/img/anime"},
                {"azurlane", "sfw/img/azurlane"},
                {"chibi", "sfw/img/chibi"},
                {"christmas", "sfw/img/christmas"},
                // {"ddlc", "sfw/img/ddlc"},
                {"halloween", "sfw/img/halloween"},
                {"holo", "sfw/img/holo"},
                {"kitsune", "sfw/img/kitsune"},
                {"maid", "sfw/img/maid"},
                {"neko", "sfw/img/neko"},
                {"nekoboy", "sfw/img/nekoboy"},
                {"nekopara", "sfw/img/nekopara"},
                {"senko", "sfw/img/senko"},
                {"wallpaper", "sfw/img/wallpaper"},

                // Gifs
                {"baka", "sfw/gif/baka"},
                {"bite", "sfw/gif/bite"},
                {"blush", "sfw/gif/blush"},
                {"cry", "sfw/gif/cry"},
                {"dance", "sfw/gif/dance"},
                {"feed", "sfw/gif/feed"},
                {"fluff", "sfw/gif/fluff"},
                {"grab", "sfw/gif/grab"},
                {"handhold", "sfw/gif/handhold"},
                {"highfive", "sfw/gif/highfive"},
                {"hug", "sfw/gif/hug"},
                {"kiss", "sfw/gif/kiss"},
                {"lick", "sfw/gif/lick"},
                {"neko gif", "sfw/gif/neko"},
                {"pat", "sfw/gif/pat"},
                {"poke", "sfw/gif/poke"},
                {"punch", "sfw/gif/punch"},
                {"shrug", "sfw/gif/shrug"},
                {"slap", "sfw/gif/slap"},
                {"smug", "sfw/gif/smug"},
                {"stare", "sfw/gif/stare"},
                {"tickle", "sfw/gif/tickle"},
                {"wag", "sfw/gif/wag"},
                {"wasted", "sfw/gif/wasted"},
                {"wave", "sfw/gif/wave"},
                {"wink", "sfw/gif/wink"},
            },
            {
                // Images 
                {"anal", "nsfw/img/anal"},
                {"anus", "nsfw/img/anus"},
                {"ass", "nsfw/img/ass"},
                {"azurlane", "nsfw/img/azurlane"},
                {"bdsm", "nsfw/img/bdsm"},
                {"blowjob", "nsfw/img/blowjob"},
                {"boobs", "nsfw/img/boobs"},
                {"cosplay", "nsfw/img/cosplay"},
                {"cum", "nsfw/img/cum"},
                {"feet", "nsfw/img/feet"},
                // {"femdom", "nsfw/img/femdom"},
                {"futa", "nsfw/img/futa"},
                {"gasm", "nsfw/img/gasm"},
                {"holo", "nsfw/img/holo"},
                // {"kitsune", "nsfw/img/kitsune"},
                {"lewd", "nsfw/img/lewd"},
                {"neko", "nsfw/img/neko"},
                {"nekopara", "nsfw/img/nekopara"},
                // {"pantsu", "nsfw/img/pantsu"},
                {"pantyhose", "nsfw/img/pantyhose"},
                {"peeing", "nsfw/img/peeing"},
                {"petplay", "nsfw/img/petplay"},
                {"pussy", "nsfw/img/pussy"},
                {"slimes", "nsfw/img/slimes"},
                {"solo", "nsfw/img/solo"},
                // {"swimsuit", "nsfw/img/swimsuit"},
                // {"tentacle", "nsfw/img/tentacle"},
                // {"thighs", "nsfw/img/thighs"},
                {"trap", "nsfw/img/trap"},
                {"yaoi", "nsfw/img/yaoi"},
                {"yuri", "nsfw/img/yuri"},

                // GIFs
                {"anal gif", "nsfw/gif/anal"},
                {"ass gif", "nsfw/gif/ass"},
                {"bdsm gif", "nsfw/gif/bdsm"},
                {"blowjob gif", "nsfw/gif/blowjob"},
                {"boobjob", "nsfw/gif/boobjob"},
                {"boobs gif", "nsfw/gif/boobs"},
                {"cum gif", "nsfw/gif/cum"},
                {"feet gif", "nsfw/gif/feet"},
                {"futa gif", "nsfw/gif/futa"},
                {"handjob", "nsfw/gif/handjob"},
                {"hentai", "nsfw/gif/hentai"},
                // {"kitsune (not implemented yet)", "nsfw/gif/kitsune"},
                {"kuni", "nsfw/gif/kuni"},
                {"neko gif", "nsfw/gif/neko"},
                {"pussy gif", "nsfw/gif/pussy"},
                {"wank", "nsfw/gif/wank"},
                {"solo gif", "nsfw/gif/solo"},
                {"spank", "nsfw/gif/spank"},
                // {"femdom gif", "nsfw/gif/femdom"},
                {"tentacle gif", "nsfw/gif/tentacle"},
                {"toys", "nsfw/gif/toys"},
                {"yuri gif", "nsfw/gif/yuri"},
            },
            "file"
        }
    },
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
    std::function<void(bool success, std::string url)> finished,
    std::string apiKey) {
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
        
    }, nullptr, apiKey);
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