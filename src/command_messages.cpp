#include "command_messages.h"
#include <cassert>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

bool CommandRegister::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("id")) {
        id = params["id"];
    }
    if (params.contains("role")) {
        role = params["role"];
    }
    if (params.contains("access_code")) {
        access_code = params["access_code"];
    }
    if (params.contains("admin_code")) {
        admin_code = params["admin_code"];
    }
    return true;
}

string CommandRegister::encodeToJSON() {
    json json_obj;
    json_obj["id"] = id;
    if (! role.empty()) {
        json_obj["role"] = role;
    }
    if (! access_code.empty()) {
        json_obj["access_code"] = access_code;
    }
    if (! admin_code.empty()) {
        json_obj["admin_code"] = admin_code;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandRegister::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandRegister::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}

bool CommandForward::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (!params["targets"].is_array()) {
        return false;
    }
    targets = params["targets"].template get<std::vector<uint32_t>>();
    return true;
}

string CommandForward::encodeToJSON() {
    json json_obj;
    if (! targets.empty()) {
        json_obj["targets"] = targets;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandForward::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandForward::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}

bool CommandInfoReq::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("is_details")) {
        is_details = params["is_details"];
    }
    return true;
}

string CommandInfoReq::encodeToJSON() {
    json json_obj;
    json_obj["is_details"] = is_details;
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandInfoReq::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandInfoReq::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}

bool CommandInfo::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("id")) {
        id = params["id"];
    }
    if (params.contains("uptime")) {
        uptime = params["uptime"];
    }
    if (params.contains("mode")) {
        serving_mode = params["mode"];
    }
    if (params.contains("access_code")) {
        access_code = params["access_code"];
    }
    if (params.contains("admin_code")) {
        admin_code = params["admin_code"];
    }
    if (params.contains("endpoints")) {
        auto params_endpoints = params["endpoints"];
        if (params_endpoints.contains("total")) {
            endpoints.total = params_endpoints["total"];
        }
        if (params_endpoints.contains("rx_bytes")) {
            endpoints.rx_bytes = params_endpoints["rx_bytes"];
        }
        if (params_endpoints.contains("tx_bytes")) {
            endpoints.tx_bytes = params_endpoints["tx_bytes"];
        }
    }
    if (params.contains("admin_clients")) {
        auto params_admin_clients = params["admin_clients"];
        admin_clients.total = params_admin_clients["total"];
    }
    if (params.contains("pending_clients")) {
        auto params_pending_clients = params["pending_clients"];
        pending_clients.total = params_pending_clients["total"];
    }
    return true;
}

string CommandInfo::encodeToJSON() {
    json rsp;
    rsp["id"] = id;
    rsp["mode"] = serving_mode;
    rsp["uptime"] = uptime;
    rsp["access_code"] = access_code;
    //rsp["admin_code"] = admin_code;  // XXX: Dangerous, remove this on product

    rsp["endpoints"]["total"] = endpoints.total;
    rsp["endpoints"]["rx_bytes"] = endpoints.rx_bytes;
    rsp["endpoints"]["tx_bytes"] = endpoints.tx_bytes;

    rsp["admin_clients"]["total"] = admin_clients.total;
    rsp["pending_clients"]["total"] = pending_clients.total;

    for (auto value : details.dummy_arr) {
        rsp["details"]["dummy_arr"].push_back(value);
    }
    for (auto [ep_id, value] : details.endpoints) {
        rsp["details"]["endpoints"][std::to_string(ep_id)] = value;
    }
    return rsp.dump();
}

bool CommandInfo::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandInfo::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}

bool CommandSetup::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);
    if (params.contains("admin_code")) {
        admin_code = params["admin_code"];
    }
    if (params.contains("new_admin_code")) {
        new_admin_code = params["new_admin_code"];
    }
    if (params.contains("new_access_code")) {
        new_access_code = params["new_access_code"];
    }
    if (params.contains("mode")) {
        mode = params["mode"];
    }
    return true;
}

string CommandSetup::encodeToJSON() {
    json json_obj;
    if (! admin_code.empty()) {
        json_obj["admin_code"] = admin_code;
    }
    if (! new_admin_code.empty()) {
        json_obj["new_admin_code"] = new_admin_code;
    }
    if (! new_access_code.empty()) {
        json_obj["new_access_code"] = new_access_code;
    }
    if (! mode.empty()) {
        json_obj["mode"] = mode;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandSetup::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandSetup::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}

bool CommandKickout::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("targets") && params["targets"].is_array()) {
        targets = params["targets"].template get<std::vector<uint32_t>>();
    }
    return true;
}

string CommandKickout::encodeToJSON() {
    json json_obj;
    if (! targets.empty()) {
        json_obj["targets"] = targets;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandKickout::decodeFromPB(const string& data) {
    assert(false && "Not implemented");
    _raw_data = data;
    return false;
}

string CommandKickout::encodeToPB() {
    assert(false && "Not implemented");
    return "";
}
