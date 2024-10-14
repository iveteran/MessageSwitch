#ifndef _UAC_MESSAGES_H
#define _UAC_MESSAGES_H

#include <string>

using std::string;

struct UserBaseInfoReq {
    uint32_t uid;

    void DecodeFromJSON(const char* data, size_t size);
    string EncodeToJSON();
};

struct UserBaseInfo {
    uint32_t uid;
    string name;
    int age;
    int gender;
    time_t creation_time;
    string lang;
    string country;
    string address;

    void DecodeFromJSON(const char* data, size_t size);
    string EncodeToJSON();
};

#endif  // _UAC_MESSAGES_H
