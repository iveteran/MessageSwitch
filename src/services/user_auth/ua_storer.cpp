#include "ua_storer.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <eventloop/el.h>

string generate_random_string() {
    return "ABC";
}
string generate_password_md5(const string& salt, const string& password) {
    return salt + ":" + password;
}
string generate_session_id() {
    return "fake_session_id";
}

void UAStorer::LoadUsers() {
    std::ifstream f(users_file_);
    if(f.fail()) {
        return;
    }
    users_ = json::parse(f);

    for (auto& user_info : users_) {
        uint32_t user_id = user_info["uid"];
        const string& username = user_info["username"];
        if (user_id > max_user_id_) {
            max_user_id_ = user_id;
        }

        users_id_index_[user_id] = user_info;
        users_name_index_[username] = user_info;
    }
}

void UAStorer::DumpUsers() {
    string data = users_.dump();
    std::ofstream f(users_file_);
    f << data;
    f.close();
}

void UAStorer::LoadUsersSession() {
    std::ifstream f(sess_file_);
    if(f.fail()) {
        return;
    }
    users_session_ = json::parse(f);

    for (auto& user_sess : users_session_) {
        uint32_t user_id = user_sess["uid"];
        users_session_index_[user_id] = user_sess;
    }
}

void UAStorer::DumpSession() {
    string data = users_session_.dump();
    std::ofstream f(sess_file_);
    f << data;
    f.close();
}

string UAStorer::GetUsers(int indent) const
{
    return users_.dump(indent);
}

string UAStorer::GetSessions(int indent) const
{
    return users_session_.dump(indent);
}

UserInfoPtr
UAStorer::GetUser(const string& username) const {
    auto iter = users_name_index_.find(username);
    if (iter != users_name_index_.end()) {
        auto user_info = iter->second;
        /*
        auto ui = new UserInfo{
            user_info["uid"],
            user_info["username"],
            user_info["salt"],
            user_info["password"],
            user_info["created_time"]
        };
        */
        auto ui = new UserInfo();
        ui->uid = user_info["uid"];
        ui->username = user_info["username"];
        ui->salt = user_info["salt"];
        ui->password = user_info["password"];
        ui->created_time = user_info["created_time"];
        return std::shared_ptr<UserInfo>(ui);
    }
    return nullptr;
}

UserInfoPtr
UAStorer::GetUser(uint32_t user_id) const {
    auto iter = users_id_index_.find(user_id);
    if (iter != users_id_index_.end()) {
        auto user_info = iter->second;
        auto ui = new UserInfo{
            user_info["uid"],
            user_info["username"],
            user_info["salt"],
            user_info["password"],
            user_info["created_time"]
        };
        return std::shared_ptr<UserInfo>(ui);
    }
    return nullptr;
}

bool UAStorer::VerifyUser(const string& username, const string& password) const {
    auto user_info = GetUser(username);
    return user_info && user_info->password == generate_password_md5(user_info->salt, password);
}

UserInfoPtr
UAStorer::CreateUser(const string& username, const string& password) {
    uint32_t user_id = ++max_user_id_;
    string salt = generate_random_string();
    string pwd_md5 = generate_password_md5(salt, password);
    time_t now_ts = evt_loop::Now();
    json user_info = {
        {"uid", user_id},
        {"username", username},
        {"password", pwd_md5},
        {"salt", salt},
        {"created_time", now_ts}
    };
    users_.push_back(user_info);
    users_id_index_[user_id] = user_info;
    users_name_index_[username] = user_info;

    DumpUsers();

    return GetUser(user_id);
}

void UAStorer::RemoveUser(uint32_t user_id) {
    auto iter = users_id_index_.find(user_id);
    if (iter == users_id_index_.end()) {
        return;
    }
    auto user_info = iter->second;
    auto username = user_info["username"];
    users_name_index_.erase(username);
    users_id_index_.erase(iter);

    for (auto iter=users_.begin(); iter!=users_.end(); ++iter) {
        if ((*iter)["uid"] == user_id) {
            users_.erase(iter);
            break;
        }
    }

    DumpUsers();

    RemoveSession(user_id);
}

bool UAStorer::IsUserExists(const string& username) const {
    return GetUser(username) != nullptr;
}

UserSessionPtr
UAStorer::GetSession(uint32_t user_id) const {
    auto iter = users_session_index_.find(user_id);
    if (iter != users_session_index_.end()) {
        auto user_sess = iter->second;
        auto us = new UserSession{
            user_sess["uid"],
            user_sess["session_id"],
            user_sess["created_time"],
            user_sess["last_activity_time"]
        };
        return std::shared_ptr<UserSession>(us);
    }
    return nullptr;
}

bool UAStorer::VerifySession(uint32_t user_id, const string& sess_id) const {
    auto user_sess = GetSession(user_id);
    return user_sess && user_sess->session_id == sess_id;
}

UserSessionPtr
UAStorer::CreateSession(uint32_t user_id) {
    time_t now_ts = evt_loop::Now();
    string session_id = generate_session_id();
    json user_sess = {
        {"uid", user_id},
        {"session_id", session_id},
        {"created_time", now_ts},
        {"last_activity_time", now_ts}
    };
    if (users_session_index_.find(user_id) != users_session_index_.end()) {
        // it exists, replace it
        for (size_t i=0; i<users_session_.size(); i++) {
            if (users_session_[i]["uid"] == user_id) {
                users_session_[i] = user_sess;
            }
        }
    } else {
        // add session
        users_session_.push_back(user_sess);
    }
    users_session_index_[user_id] = user_sess;

    DumpSession();

    return GetSession(user_id);
}

void UAStorer::RemoveSession(uint32_t user_id) {
    users_session_index_.erase(user_id);

    for (auto iter=users_session_.begin(); iter!=users_session_.end(); ++iter) {
        if ((*iter)["uid"] == user_id) {
            users_session_.erase(iter);
            break;
        }
    }
    DumpSession();
}

bool UAStorer::IsSessionExists(uint32_t user_id) const {
    return GetSession(user_id) != nullptr;
}
