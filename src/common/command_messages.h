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
    uint32_t id = 0;
    uint8_t role;
    string access_code;
    string token;
    uint8_t svc_type = 0;
    string _raw_data;

    bool decodeFromJSON(const string& data);
    string encodeToJSON();

    bool decodeFromPB(const string& data);
    string encodeToPB();
};

struct CommandResultRegister {
    uint32_t id;
    string token;
    uint8_t role;
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
        map<uint32_t, map<string, uint32_t>> eps;  // id -> {uptime, ...}
    } endpoints;
    struct {
        uint32_t total;
        vector<uint32_t> eps;  // id list
    } normal_endpoints;
    struct {
        uint32_t total;
        vector<uint32_t> eps;  // id list
    } admin_endpoints;
    struct {
        uint32_t svc_type_total;
        uint32_t svc_ep_total;
        map<uint8_t, vector<uint32_t>> eps;  // svc type -> [ep id list]
    } service_endpoints;
    struct {
        uint32_t total;
        vector<uint32_t> eps;  // id list
    } pending_clients;

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
    uint8_t svc_type;
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
