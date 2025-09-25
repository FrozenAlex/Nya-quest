#include "API.hpp"
#include "web-utils/shared/WebUtils.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/pointer.h"
#include "Utils/Utils.hpp"

using namespace NyaAPI;


inline std::map<std::string, NyaAPI::SourceData> endpoint_data = {
    {"fluxpoint.dev",
        {
            "https://gallery.fluxpoint.dev/api/",
            DataMode::Json,
            {
                // Images
                {"anime", "sfw/img/anime"},
                {"azurlane", "sfw/img/azurlane"},
                {"chibi", "sfw/img/chibi"},
                {"christmas", "sfw/img/christmas"},
                {"ddlc", "sfw/img/ddlc"},
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
                {"femdom", "nsfw/img/femdom"},
                {"futa", "nsfw/img/futa"},
                {"gasm", "nsfw/img/gasm"},
                {"holo", "nsfw/img/holo"},
                {"kitsune", "nsfw/img/kitsune"},
                {"lewd", "nsfw/img/lewd"},
                {"neko", "nsfw/img/neko"},
                {"nekopara", "nsfw/img/nekopara"},
                {"pantsu", "nsfw/img/pantsu"},
                {"pantyhose", "nsfw/img/pantyhose"},
                {"peeing", "nsfw/img/peeing"},
                {"petplay", "nsfw/img/petplay"},
                {"pussy", "nsfw/img/pussy"},
                {"slimes", "nsfw/img/slimes"},
                {"solo", "nsfw/img/solo"},
                {"swimsuit", "nsfw/img/swimsuit"},
                {"tentacle", "nsfw/img/tentacle"},
                {"thighs", "nsfw/img/thighs"},
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
                //{"kitsune", "nsfw/gif/kitsune"},
                {"kuni", "nsfw/gif/kuni"},
                {"neko gif", "nsfw/gif/neko"},
                {"pussy gif", "nsfw/gif/pussy"},
                {"wank", "nsfw/gif/wank"},
                {"solo gif", "nsfw/gif/solo"},
                {"spank", "nsfw/gif/spank"},
                //{"femdom gif", "nsfw/gif/femdom"},
                {"tentacle gif", "nsfw/gif/tentacle"},
                {"toys", "nsfw/gif/toys"},
                {"yuri gif", "nsfw/gif/yuri"},
            },
            "file",
            "FP-Public-naEjca70OhKMtq67WpzaN8Gs"
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
                // {"kill", "sfw/kill"}, // Not including 'kill' because it's bad
            },
            {
                {"neko", "nsfw/neko"},
                {"waifu", "nsfw/waifu"},
                {"trap", "nsfw/trap"},
                {"blowjob", "nsfw/blowjob"},
            },
            "url",
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
            "https://some-random-api.com/animal/",
            DataMode::Json,
            {
                {"cat", "cat"},
                {"dog", "dog"},
                {"fox", "fox"},
                {"panda", "panda"},
                {"red panda", "red_panda"},
                {"bird", "bird"},
                {"birb", "birb"},
                {"koala", "koala"},
                {"kangaroo", "kangaroo"},
                {"raccoon", "raccoon"},
                {"whale", "whale"},
            },
            {},
            "image"
        }
    },
    {"Animu",
        {
            "https://api.some-random-api.com/animu/",
            DataMode::Json,
            {
                {"nom", "nom"},
                {"poke", "poke"},
                {"cry", "cry"},
                {"kiss", "kiss"},
                {"pat", "pat"},
                {"hug", "hug"},
                {"wink", "wink"},
                {"face-palm", "face-palm"},
                {"quote", "quote"},
            },
            {},
            "link"
        }
    },
    {"nekosia.cat",
        {
            "https://api.nekosia.cat/api/v1/images/",
            DataMode::Json,
            {
                {"catgirl", "catgirl"},
                {"foxgirl", "foxgirl"},
                {"wolf girl", "wolf-girl"},
                {"animal ears", "animal-ears"},
                {"tail", "tail"},
                {"tail with ribbon", "tail-with-ribbon"},
                {"tail from under skirt", "tail-from-under-skirt"},
                {"cute", "cute"},
                {"cuteness is justice", "cuteness-is-justice"},
                {"blue archive", "blue-archive"},
                {"girl", "girl"},
                {"young girl", "young-girl"},
                {"maid", "maid"},
                {"maid uniform", "maid-uniform"},
                {"vtuber", "vtuber"},
                {"w sitting", "w-sitting"},
                {"lying down", "lying-down"},
                {"hands forming a <3", "hands-forming-a-heart"},
                {"wink", "wink"},
                {"valentine", "valentine"},
                {"thigh high socks", "thigh-high-socks"},
                {"headphones", "headphones"},
                {"knee high socks", "knee-high-socks"},
                {"white tights", "white-tights"},
                {"black tights", "black-tights"},
                {"heterochromia", "heterochromia"},
                {"uniform", "uniform"},
                {"sailor uniform", "sailor-uniform"},
                {"hoodie", "hoodie"},
                {"ribbon", "ribbon"},
                {"white hair", "white-hair"},
                {"blue hair", "blue-hair"},
                {"long hair", "long-hair"},
                {"blonde", "blonde"},
                {"blue eyes", "blue-eyes"},
                {"purple eyes", "purple-eyes"}
            },
            {},
            "image/original/url"
        }
    },
    {"nekos.best",
        {
            "https://nekos.best/api/v2/",
            DataMode::Json,
            {
                {"husbando", "husbando"},
                {"kitsune", "kitsune"},
                {"neko", "neko"},
                {"waifu", "waifu"},
                {"angry", "angry"},
                {"baka", "baka"},
                {"bite", "bite"},
                {"blush", "blush"},
                {"bored", "bored"},
                {"cry", "cry"},
                {"cuddle", "cuddle"},
                {"dance", "dance"},
                {"facepalm", "facepalm"},
                {"feed", "feed"},
                {"handhold", "handhold"},
                {"handshake", "handshake"},
                {"happy", "happy"},
                {"highfive", "highfive"},
                {"hug", "hug"},
                {"kick", "kick"},
                {"kiss", "kiss"},
                {"laugh", "laugh"},
                {"lurk", "lurk"},
                {"nod", "nod"},
                {"nom", "nom"},
                {"nope", "nope"},
                {"pat", "pat"},
                {"peck", "peck"},
                {"poke", "poke"},
                {"pout", "pout"},
                {"punch", "punch"},
                {"run", "run"},
                {"shoot", "shoot"},
                {"shrug", "shrug"},
                {"slap", "slap"},
                {"sleep", "sleep"},
                {"smile", "smile"},
                {"smug", "smug"},
                {"stare", "stare"},
                {"think", "think"},
                {"thumbsup", "thumbsup"},
                {"tickle", "tickle"},
                {"wave", "wave"},
                {"wink", "wink"},
                {"yawn", "yawn"},
                {"yeet", "yeet"},
            },
            {},
            "results/0/url"
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

/**
 * @brief Get the path from a json api
 * WARNING: This function runs finished not on the main thread
 * @param source The source data
 * @param endpoint The endpoint to use
 * @param timeoutInSeconds The timeout in seconds
 * @param finished The function to run when the request is finished
 */
void NyaAPI::get_path_from_json_api(
    SourceData* source,
    std::string endpoint,
    bool nsfw,
    float timeoutInSeconds,
    std::function<void(bool success, std::string url)> finished
) {
    if (finished == nullptr) {
        return ERROR("Can't get data async without a callback to use it with");
    }
    if (source == nullptr) {
        ERROR("Source is null");
        return finished(false, "");
    }

    std::string endpointURL = source->BaseEndpoint + endpoint;

    if (!nsfw) INFO("Endpoint URL: {}", endpointURL);

    std::thread([source, endpointURL, timeoutInSeconds, finished] {
        auto reqOptions = WebUtils::URLOptions(endpointURL);
        reqOptions.timeOut = timeoutInSeconds;
        if (!source->apiKey.empty()) {
            reqOptions.headers.emplace("Authorization", source->apiKey);
        }
        auto response = WebUtils::Get<WebUtils::JsonResponse>(reqOptions);

        if (
            !response.IsSuccessful() || 
            !response.responseData.has_value()
        ) return finished(false, "");

        auto& document = response.responseData.value();
        if (
            document.HasParseError() ||
            !document.IsObject()
        ) return finished(false, "");
      
        std::string formatedPropertyName = "";

        if (!source->propertyName.starts_with("/")) {
            formatedPropertyName = "/"+ source->propertyName;
        } else {
            formatedPropertyName = source->propertyName;
        }

        // https://rapidjson.org/md_doc_pointer.html
        rapidjson::Value* current = rapidjson::Pointer(formatedPropertyName).Get(document);
        if (current == nullptr || !current->IsString()) return finished(false, "");

        std::string result(current->GetString(), current->GetStringLength());
        return finished(true, result);
    }).detach();
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

    ListW<StringW> NyaAPI::listEndpointLabels(std::vector<EndpointCategory>* values) {
        // TODO: Fix
        int count = values->size();
        
        // Convert stuff to list
        ListW<StringW> list = ListW<StringW>::New();
        if (count == 0) {
            return list;
        }
        list->EnsureCapacity(count);
        for (auto item : *values) {list->Add(item.label);};

        return list;
    }

    ListW<StringW> NyaAPI::listEndpointUrls(std::vector<EndpointCategory>* values) {
        // TODO: Fix
        int count = values->size();
        
        // Convert stuff to list
        ListW<StringW> list = ListW<StringW>::New();
        if (count == 0) {
            return list;
        }
        list->EnsureCapacity(count);
        for (auto item : *values) {list->Add(item.url);};

        return list;
    }

EndpointCategory* NyaAPI::getRandomEndpoint(std::vector<EndpointCategory>* values) {
    int count = values->size();
    if (count == 0) return nullptr;

    int index = Nya::Utils::random(0, count - 1);
    return &values->at(index);
}
