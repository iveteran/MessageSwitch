#include "uac_messages.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void UserBaseInfoReq::DecodeFromJSON(const char* data, size_t size)
{
    json j = json::parse(string(data, size));
    if (j.contains("uid")) {
        uid = j["uid"];
    }
}

string UserBaseInfoReq::EncodeToJSON()
{
    json j = {
        {"uid", uid},
    };
    return j.dump();
}

void UserBaseInfo::DecodeFromJSON(const char* data, size_t size)
{
    json j = json::parse(string(data, size));
    if (j.contains("uid")) {
        uid = j["uid"];
    }
    if (j.contains("name")) {
        name = j["name"];
    }
    if (j.contains("age")) {
        age = j["age"];
    }
    if (j.contains("gender")) {
        gender = j["gender"];
    }
    if (j.contains("creation_time")) {
        creation_time = j["creation_time"];
    }
    if (j.contains("lang")) {
        lang = j["lang"];
    }
    if (j.contains("country")) {
        country = j["country"];
    }
    if (j.contains("address")) {
        country = j["address"];
    }
}

string UserBaseInfo::EncodeToJSON()
{
    json j = {
        {"uid", uid},
        {"name", name},
        {"age", age},
        {"gender", gender},
        {"creation_time", creation_time},
        {"lang", lang},
        {"country", country},
        {"address", address},
    };
    return j.dump();
}
