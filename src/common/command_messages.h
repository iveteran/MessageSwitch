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
    string token;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandResultRegister {
    uint32_t id;
    string token;
    string role;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};
using CommandResultRegisterPtr = std::shared_ptr<CommandResultRegister>;

struct CommandForward {
    vector<uint32_t> targets;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};
using CommandUnforward = CommandForward;  // same as CommandForward

struct CommandSubUnsubRejUnrej {
    vector<uint32_t> sources;
    vector<uint8_t> messages;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};
// make aliases
using CommandSubscribe = CommandSubUnsubRejUnrej;
using CommandUnsubscribe = CommandSubUnsubRejUnrej;
using CommandReject = CommandSubUnsubRejUnrej;
using CommandUnreject = CommandSubUnsubRejUnrej;

struct CommandInfoReq {
    bool is_details;
    uint32_t endpoint_id;
    string _raw_data;

    CommandInfoReq() : is_details(false), endpoint_id(0) {}

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

struct CommandEndpointInfo {
    uint32_t id;
    time_t uptime;
    uint8_t role;
    vector<uint32_t> fwd_targets;
    vector<uint32_t> subs_sources;
    vector<uint32_t> rej_sources;
    vector<uint8_t> subs_messages;
    vector<uint8_t> rej_messages;
    uint32_t rx_bytes;
    uint32_t tx_bytes;

    string _raw_data;

    CommandEndpointInfo() : id(0), uptime(0), role(0), rx_bytes(0), tx_bytes(0) {}

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};
using CommandEndpointInfoPtr = std::shared_ptr<CommandEndpointInfo>;

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
