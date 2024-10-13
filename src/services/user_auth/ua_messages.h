#ifndef _UA_MESSAGES_H
#define _UA_MESSAGES_H

#include <string>

using std::string;

struct MessageUserAuth {
    string username;
    string password;

    void DecodeFromJSON(const char* data, size_t size);
    string EncodeToJSON();
};

struct MessageUserSession {
    uint32_t uid;
    string session_id;

    void DecodeFromJSON(const char* data, size_t size);
    string EncodeToJSON();
};

// Other messages, make alias for them
using Login = MessageUserAuth;
using CreateUser = MessageUserAuth;
using VerifyUser = MessageUserSession;
using Logout = MessageUserSession;
using RemoveUser = MessageUserSession;
using LoginResult = MessageUserSession;

#endif  // _UA_MESSAGES_H
