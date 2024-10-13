#ifndef _UA_STORER_H
#define _UA_STORER_H

#include <string>
#include <map>
#include <tuple>
#include <memory>
#include <nlohmann/json.hpp>

using std::string;
using std::map;
using std::tuple;
using json = nlohmann::json;

string generate_random_string();
string generate_password_md5(const string& salt, const string& password);

struct UserInfo {
    uint32_t uid;
    string username;
    string password;
    string salt;
    uint32_t created_time;
};
using UserInfoPtr = std::shared_ptr<UserInfo>;

struct UserSession {
    uint32_t uid;
    string session_id;
    uint32_t created_time;
    uint32_t last_activity_time;
};
using UserSessionPtr = std::shared_ptr<UserSession>;

/*
 * file example:
 * [
 * {"uid": 1, "username": "abc@def.com", "salt": "SxESs", "password": "5b592e5f5772bc49b1b6ad88ae30b279", "created_time": 1728697208},
 * ...
 * ]
*/
class UAStorer {
public:
    UAStorer(const char* users_file, const char* sess_file)
        : users_file_(users_file), sess_file_(sess_file)
    {
        LoadUsers();
        LoadUsersSession();
    }
    void LoadUsers();
    void DumpUsers();

    void LoadUsersSession();
    void DumpSession(); 

    string GetUsers(int indent=-1) const;
    string GetSessions(int indent=-1) const;

    UserInfoPtr GetUser(const string& username) const;
    UserInfoPtr GetUser(uint32_t user_id) const;
    bool VerifyUser(const string& username, const string& password) const;
    UserInfoPtr CreateUser(const string& username, const string& password);
    void RemoveUser(uint32_t user_id);
    bool IsUserExists(const string& username) const;

    UserSessionPtr GetSession(uint32_t user_id) const;
    bool VerifySession(uint32_t user_id, const string& sess_id) const;
    UserSessionPtr CreateSession(uint32_t user_id); 
    void RemoveSession(uint32_t user_id);
    bool IsSessionExists(uint32_t user_id) const;

private:
    string users_file_;
    uint32_t max_user_id_ = 0;

    json users_;
    map<string, json>   users_name_index_;
    map<uint32_t, json> users_id_index_;

    string sess_file_;
    json users_session_;
    map<uint32_t, json> users_session_index_;  // user_id -> {session_id, created_time, last_activity_time}
};

#endif  // _UA_STORER_H
