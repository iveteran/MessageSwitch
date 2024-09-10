#include <sstream>
#include "switch_command_handler.h"
#include "switch_server.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

int CommandHandler::handleEcho(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data)
{
    printf("------> cmd: Echo\n");
    printf("------> payload_len: %d\n", cmdMsg->payload_len);
    printf("------> payload: %s\n", cmdMsg->payload);

    const ECommand cmd = (ECommand)cmdMsg->cmd;
    int8_t errcode = 0;
    sendResultMessage(conn, cmd, errcode, cmdMsg->payload, cmdMsg->payload_len);

    return 0;
}

int CommandHandler::handleRegister(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data)
{
    printf("------> cmd: Register\n");
    printf("------> payload_len: %d\n", cmdMsg->payload_len);
    printf("------> payload: %s\n", cmdMsg->payload);

    // 1) authorize by access token
    // 2) set endpoint role - endpoint, admin, proxy, rproxy, cluster node
    // 3) set endpoint id

    const ECommand cmd = (ECommand)cmdMsg->cmd;

    string payload(cmdMsg->payload, cmdMsg->payload_len);
    json params = json::parse(payload);

    if (! ((params.contains("access_code") || params.contains("admin_code"))
                && params.contains("id") && params.contains("role"))) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Missing required parameter(s)");
        fprintf(stderr, "[handleRegister] Error: %s\n", errmsg);
        sendResultMessage(conn, cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    string role_str = params["role"];
    EEndpointRole role = TagToEndpointRole(role_str);
    if (role == EEndpointRole::Admin) {
        if (! params.contains("admin_code") || params["admin_code"] != context_->admin_code) {
            int8_t errcode = 1;
            string errmsg("Authentication failed");
            fprintf(stderr, "[handleRegister] Error: %s\n", errmsg.c_str());
            sendResultMessage(conn, cmd, errcode, errmsg);
            return errcode;
        }
    } else if (role == EEndpointRole::Endpoint) {
        string access_code = params["access_code"];
        if (! params.contains("access_code") || params["access_code"] != context_->access_code) {
            int8_t errcode = 1;
            string errmsg("Authentication failed");
            fprintf(stderr, "[handleRegister] Error: %s\n", errmsg.c_str());
            sendResultMessage(conn, cmd, errcode, errmsg);
            return errcode;
        }
    } else if (role == EEndpointRole::Undefined) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Has invalid parameter: role = %s", role_str.c_str());
        fprintf(stderr, "[handleRegister] Error: %s\n", errmsg);
        sendResultMessage(conn, cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    uint32_t ep_id = params.at("id");
    if (ep_id <= 0) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Has invalid parameter: endpoint id = %d", ep_id);
        fprintf(stderr, "[handleRegister] Error: %s\n", errmsg);
        sendResultMessage(conn, cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    auto ep = std::make_shared<Endpoint>(ep_id, conn);
    ep->SetRole(role);
    context_->endpoints.insert(std::make_pair(ep_id, ep));
    if (role == EEndpointRole::Admin) {
        context_->admin_clients.insert(std::make_pair(ep_id, ep));
    }
    context_->pending_clients.erase(conn->FD());

    int8_t errcode = 0;
    sendResultMessage(conn, cmd, errcode);

    return errcode;
}

int CommandHandler::handleForward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = (ECommand)cmdMsg->cmd;

    string payload(cmdMsg->payload, cmdMsg->payload_len);
    printf("[handleForward] forwarding targets: %s\n", payload.c_str());
    json params = json::parse(payload);

    if (! (params.contains("targets") && params["targets"].is_array())) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Missing required parameter or the parameter is invalid");
        fprintf(stderr, "[handleForward] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    vector<uint32_t> targets = params["targets"];
    if (! targets.empty()) {
        ep->SetForwardTargets(targets);
    }

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode);

    return 0;
}

int CommandHandler::handleData(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    printf("------> cmd: DATA\n");
    printf("------> payload_len: %d\n", cmdMsg->payload_len);
    printf("------> payload: %s\n", cmdMsg->payload);

    const ECommand cmd = (ECommand)cmdMsg->cmd;
    reverseToNetworkMessage((CommandMessage*)cmdMsg);

    auto fwd_targets = ep->GetForwardTargets();
    if (fwd_targets.empty() || fwd_targets[0] == 0) {
        // broadcast
        for (auto iter : context_->endpoints) {
            EndpointId ep_id = iter.first;
            auto target_ep = iter.second;
            if (ep_id == ep->Id()) {
                // It's self
                continue;
            }
            printf("[handleData] forward message: size: %ld\n", data.size());
            target_ep->Connection()->Send(data);
        }
    } else {
        // multicast
        for (auto ep_id : fwd_targets) {
            auto iter = context_->endpoints.find(ep_id);
            if (iter == context_->endpoints.end()) {
                continue;
            }
            auto target_ep = iter->second;
            printf("[handleData] forward message, size: %ld\n", data.size());
            target_ep->Connection()->Send(data);
        }
    }

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode);

    return 0;
}

int CommandHandler::handleInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    // 1) endpoints number -> current, connected and left of total and per endpoint
    // 2) tx/rx bytes -> total, per endpoint
    // 3) uptime
    // 4) pending connections, rejected by timeout of unregister
    // 5) command stats
    // *) other context info

    const ECommand cmd = (ECommand)cmdMsg->cmd;

    if (ep->GetRole() != EEndpointRole::Admin) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Failed: Operation not allowed for role: %s", EndpointRoleToTag(ep->GetRole()));
        fprintf(stderr, "[handleInfo] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    json rsp;
    rsp["mode"] = context_->serving_mode;
    rsp["id"] = context_->switch_server->NodeId();
    rsp["uptime"] = Now() - context_->born_time;
    rsp["access_code"] = context_->access_code;
    //rsp["admin_code"] = context_->admin_code;  // XXX: Dangerous, remove this on product version

    rsp["endpoints"]["number"] = context_->endpoints.size();
    rsp["admin_clients"]["number"] = context_->admin_clients.size();
    rsp["pending_clients"]["number"] = context_->pending_clients.size();

    size_t rx_bytes = 0;
    size_t tx_bytes = 0;
    for (auto [ep_id, ep] : context_->endpoints) {
        rx_bytes += ep->Connection()->StatsRxBytes();
        tx_bytes += ep->Connection()->StatsTxBytes();
    }
    rsp["endpoints"]["rx_bytes"] = rx_bytes;
    rsp["endpoints"]["tx_bytes"] = tx_bytes;

    if (cmdMsg->payload_len > 0) {
        string payload(cmdMsg->payload, cmdMsg->payload_len);
        json params = json::parse(payload);
        if (params.contains("details")) {
            bool show_details = params["details"];
            if (show_details) {
                rsp["details"]["dummy"].push_back(11);
                for (auto [ep_id, _] : context_->endpoints) {
                    rsp["details"]["endpoints"][std::to_string(ep_id)] = 22;
                }
            }
        }
    }

    string rsp_data = rsp.dump();
    printf("[handleInfo] response:\n");
    printf("%s\n", rsp_data.c_str());

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode, rsp_data);

    return 0;
}

int CommandHandler::handleSetup(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    // 0) authorize admin token
    // 1) set/generate admin token
    // 2) set/generate endpoint access token
    // 3) -- set serving mode

    const ECommand cmd = (ECommand)cmdMsg->cmd;

    if (ep->GetRole() != EEndpointRole::Admin) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Failed: Operation not allowed for role: %s", EndpointRoleToTag(ep->GetRole()));
        fprintf(stderr, "[handleSetup] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    string payload(cmdMsg->payload, cmdMsg->payload_len);
    json params = json::parse(payload);

    if (params.contains("new_admin_code")) {
        if (! params.contains("admin_code") || params["admin_code"] != context_->admin_code) {
            int8_t errcode = 1;
            string errmsg("Authentication failed");
            fprintf(stderr, "[handleSetup] Error: %s\n", errmsg.c_str());
            sendResultMessage(ep->Connection(), cmd, errcode, errmsg);
            return errcode;
        }

        context_->admin_code = params["new_admin_code"];
    }

    if (params.contains("new_access_code")) {
        context_->access_code = params["new_access_code"];
    }

    if (params.contains("mode")) {
        string mode_str = params["mode"];
        EServingMode mode = TagToServingMode(mode_str);
        if (mode == EServingMode::Undefined) {
            int8_t errcode = 1;
            char errmsg[64];
            snprintf(errmsg, sizeof(errmsg), "Has invalid parameter: mode = %s", mode_str.c_str());
            fprintf(stderr, "[handleSetup] Error: %s\n", errmsg);
            sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
            return errcode;
        }
        context_->serving_mode = mode;
    }

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode);

    return 0;
}

int CommandHandler::handleProxy(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleKickout(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    // 1) kickout endpoint

    const ECommand cmd = (ECommand)cmdMsg->cmd;

    if (ep->GetRole() != EEndpointRole::Admin) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Failed: Operation not allowed for role: %s", EndpointRoleToTag(ep->GetRole()));
        fprintf(stderr, "[handleKickout] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    string payload(cmdMsg->payload, cmdMsg->payload_len);
    json params = json::parse(payload);

    if (! (params.contains("targets") && params["targets"].is_array())) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Missing required parameter or the parameter is invalid");
        fprintf(stderr, "[handleKickout] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    vector<uint32_t> targets = params["targets"];
    for (auto ep_id : targets) {
        auto iter = context_->endpoints.find(ep_id);
        if (iter == context_->endpoints.end()) {
            continue;
        }
        auto target_ep_id = iter->first;
        printf("[handleKickout] kickout endpoint, target endpoint id: %d\n", target_ep_id);
        auto target_ep = iter->second;
        printf("[handleKickout] kickout endpoint, id: %d\n", target_ep->Id());
        target_ep->Connection()->Disconnect();
    }

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode);

    return 0;
}

int CommandHandler::handleReload(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = (ECommand)cmdMsg->cmd;

    if (ep->GetRole() != EEndpointRole::Admin) {
        int8_t errcode = 1;
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Failed: Operation not allowed for role: %s", EndpointRoleToTag(ep->GetRole()));
        fprintf(stderr, "[handleReload] Error: %s\n", errmsg);
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg, strlen(errmsg));
        return errcode;
    }

    return 0;
}

size_t CommandHandler::sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode, const string& data)
{
    return sendResultMessage(conn, cmd, errcode, data.data(), data.size());
}

size_t CommandHandler::sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode,
        const char* data, size_t data_len)
{
    size_t sent_bytes = 0;

    CommandMessage cmdMsg;
    cmdMsg.cmd = (uint8_t)cmd;
    cmdMsg.SetResponseFlag();
    uint32_t payload_len = sizeof(ResultMessage) + data_len;
    if (context_->switch_server->IsMessagePayloadLengthIncludingSelf()) {
        payload_len += sizeof(CommandMessage::payload_len);
    }
    cmdMsg.payload_len = htonl(payload_len);

    ResultMessage resultMsg;
    resultMsg.errcode = errcode;

    conn->Send((char*)&cmdMsg, sizeof(cmdMsg));
    sent_bytes += sizeof(cmdMsg);
    conn->Send((char*)&resultMsg, sizeof(resultMsg));
    sent_bytes += sizeof(resultMsg);
    if (data && data_len > 0) {
        conn->Send(data, data_len);
        sent_bytes += sizeof(data_len);
    }

    return sent_bytes;
}

// Reverse to network message without header of CommandMessage
std::pair<size_t, const char*>
    CommandHandler::extractMessagePayload(CommandMessage* cmdMsg)
{
    char* payload_ptr = cmdMsg->payload;
    size_t payload_len = cmdMsg->payload_len;
    if (context_->switch_server->IsMessagePayloadLengthIncludingSelf()) {
        payload_ptr -= sizeof(cmdMsg->payload_len);
        payload_len += sizeof(cmdMsg->payload_len);
    }
    cmdMsg->payload_len = htonl(payload_len);
    return std::make_pair(payload_len, payload_ptr);
}

// Reverse to network message with header of CommandMessage
Message* CommandHandler::reverseToNetworkMessage(CommandMessage* cmdMsg)
{
    if (context_->switch_server->IsMessagePayloadLengthIncludingSelf()) {
        cmdMsg->payload_len += sizeof(cmdMsg->payload_len);
    }
    cmdMsg->payload_len = htonl(cmdMsg->payload_len);
    auto msg = (Message*)cmdMsg;
    return msg;
}
