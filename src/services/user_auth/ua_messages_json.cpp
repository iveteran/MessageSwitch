#include "ua_messages.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void MessageUserAuth::DecodeFromJSON(const char* data, size_t size)
{
    json j = json::parse(string(data, size));
    if (j.contains("username")) {
        username = j["username"];
    }
    if (j.contains("password")) {
        password = j["password"];
    }
}

string MessageUserAuth::EncodeToJSON()
{
    json j = {
        {"username", username},
        {"password", password},
    };
    return j.dump();
}

void MessageUserSession::DecodeFromJSON(const char* data, size_t size)
{
    json j = json::parse(string(data, size));
    if (j.contains("uid")) {
        uid = j["uid"];
    }
    if (j.contains("session_id")) {
        session_id = j["session_id"];
    }
}

string MessageUserSession::EncodeToJSON()
{
    json j = {
        {"uid", uid},
        {"session_id", session_id},
    };
    return j.dump();
}
