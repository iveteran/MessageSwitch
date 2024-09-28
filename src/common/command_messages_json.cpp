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
    if (params.contains("token")) {
        token = params["token"];
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
    if (! token.empty()) {
        json_obj["token"] = token;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandResultRegister::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("id")) {
        id = params["id"];
    }
    if (params.contains("token")) {
        token = params["token"];
    }
    if (params.contains("role")) {
        role = params["role"];
    }
    return true;
}

string CommandResultRegister::encodeToJSON() {
    json json_obj;
    json_obj["id"] = id;
    if (! token.empty()) {
        json_obj["token"] = token;
    }
    if (! role.empty()) {
        json_obj["role"] = role;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
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

bool CommandSubUnsubRejUnrej::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params["sources"].is_array()) {
        sources = params["sources"].template get<std::vector<uint32_t>>();
    }
    if (params["messages"].is_array()) {
        messages = params["messages"].template get<std::vector<uint8_t>>();
    }
    if (sources.empty() && messages.empty()) {
        return false;
    }
    return true;
}

string CommandSubUnsubRejUnrej::encodeToJSON() {
    json json_obj;
    if (! sources.empty()) {
        json_obj["sources"] = sources;
    }
    if (! messages.empty()) {
        json_obj["messages"] = messages;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
}

bool CommandInfoReq::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("is_details")) {
        is_details = params["is_details"];
    }
    if (params.contains("endpoint_id")) {
        endpoint_id = params["endpoint_id"];
    }
    return true;
}

string CommandInfoReq::encodeToJSON() {
    json json_obj;
    json_obj["is_details"] = is_details;
    if (endpoint_id > 0) {
        json_obj["endpoint_id"] = endpoint_id;
    }
    _raw_data = json_obj.dump();
    return _raw_data;
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

bool CommandEndpointInfo::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);

    if (params.contains("id")) {
        id = params["id"];
    }
    if (params.contains("uptime")) {
        uptime = params["uptime"];
    }
    if (params.contains("role")) {
        role = params["role"];
    }

    if (params.contains("fwd_targets") && params["fwd_targets"].is_array()) {
        fwd_targets = params["fwd_targets"].template get<std::vector<uint32_t>>();
    }
    if (params.contains("subs_sources") && params["subs_sources"].is_array()) {
        subs_sources = params["subs_sources"].template get<std::vector<uint32_t>>();
    }
    if (params.contains("rej_sources") && params["rej_sources"].is_array()) {
        rej_sources = params["rej_sources"].template get<std::vector<uint32_t>>();
    }
    if (params.contains("subs_messages") && params["subs_messages"].is_array()) {
        subs_messages = params["subs_messages"].template get<std::vector<uint8_t>>();
    }
    if (params.contains("rej_messages") && params["rej_messages"].is_array()) {
        rej_messages = params["rej_messages"].template get<std::vector<uint8_t>>();
    }

    if (params.contains("rx_bytes")) {
        rx_bytes = params["rx_bytes"];
    }
    if (params.contains("tx_bytes")) {
        tx_bytes = params["tx_bytes"];
    }
    return true;
}

string CommandEndpointInfo::encodeToJSON() {
    json rsp;

    rsp["id"] = id;
    rsp["uptime"] = uptime;
    rsp["role"] = role;

    if (! fwd_targets.empty()) {
        rsp["fwd_targets"] = fwd_targets;
    }
    if (! subs_sources.empty()) {
        rsp["subs_sources"] = subs_sources;
    }
    if (! rej_sources.empty()) {
        rsp["rej_sources"] = rej_sources;
    }
    if (! subs_messages.empty()) {
        rsp["subs_messages"] = subs_messages;
    }
    if (! rej_messages.empty()) {
        rsp["rej_messages"] = rej_messages;
    }

    rsp["rx_bytes"] = rx_bytes;
    rsp["tx_bytes"] = tx_bytes;

    return rsp.dump();
};

bool CommandSetup::decodeFromJSON(const string& data) {
    _raw_data = data;
    json params = json::parse(_raw_data);
    if (params.contains("access_code")) {
        access_code = params["access_code"];
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
    if (! access_code.empty()) {
        json_obj["access_code"] = access_code;
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
