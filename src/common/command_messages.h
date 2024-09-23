#ifndef _COMMAND_MESSAGES_H
#define _COMMAND_MESSAGES_H

#include <string>
#include <vector>
#include <map>
#include <memory>

using std::string;
using std::vector;
using std::map;

struct CommandRegister {
    uint32_t id;
    string role;
    string access_code;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandForward {
    vector<uint32_t> targets;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandInfoReq {
    bool is_details;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandInfo {
    uint32_t id;
    time_t uptime;
    string serving_mode;
    string access_code;
    string admin_code;
    struct {
        uint32_t total;
        uint32_t rx_bytes;
        uint32_t tx_bytes;
    } endpoints;
    struct {
        uint32_t total;
    } admin_clients;
    struct {
        uint32_t total;
    } pending_clients;
    struct {
        vector<uint32_t> dummy_arr;
        map<uint32_t, uint32_t> endpoints;
    } details;

    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};
using CommandInfoPtr = std::shared_ptr<CommandInfo>;

struct CommandSetup {
    string access_code;
    string new_admin_code;
    string new_access_code;
    string mode;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandKickout {
    vector<uint32_t> targets;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

#endif  // _COMMAND_MESSAGES_H
