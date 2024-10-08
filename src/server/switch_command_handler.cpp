#include <sstream>
#include "switch_command_handler.h"
#include "switch_server.h"
#include "command_messages.h"

#define _DECODE_COMMAND_MESSAGE(func_name, cmd_msg, cmd_obj, conn) { \
    auto [_payload, _payload_len] = cmd_msg->Payload(); \
    if (_payload_len > 0) { \
        string payload(_payload, _payload_len); \
        if (cmd_msg->IsJSON()) { \
            cmd_obj.decodeFromJSON(payload); \
        } else if (cmd_msg->IsPB()) { \
            cmd_obj.decodeFromPB(payload); \
        } else { \
            int8_t errcode = 1; \
            const char* errmsg = "Unsupported codec of message payload"; \
            fprintf(stderr, "[%s] Error: %s\n", func_name, errmsg); \
            sendResultMessage(conn, cmd, errcode, errmsg); \
            return errcode; \
        } \
    } \
}

#define _CHECK_ROLE_PERMISSION(func_name, role, expect_role) { \
    if (role != expect_role) { \
        int8_t errcode = 1; \
        std::stringstream ss; \
        ss << "the operation not allowed for role: " << EndpointRoleToTag(role); \
        string errmsg = ss.str(); \
        fprintf(stderr, "[%s] Error: %s\n", func_name, errmsg.c_str()); \
        \
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg); \
        return errcode; \
    } \
}

void CommandHandler::handleCommand(TcpConnection* conn, const Message* msg)
{
    const string& msgData = msg->Data();
    auto cmdMsg = CommandMessage::FromNetworkMessage(msg,
            context_->switch_server->IsMessagePayloadLengthIncludingSelf());

    ECommand cmd = cmdMsg->Command();
    printf("[CommandHandler::HandleCommand] id: %d\n", conn->ID());
    printf("[CommandHandler::HandleCommand] cmd: %s(%d)\n", CommandToTag(cmd), (command_t)cmd);
    auto [payload, payload_len] = cmdMsg->Payload();
    printf("[CommandHandler::HandleCommand] payload len: %d\n", payload_len);
    cout << DumpHexWithChars(payload, payload_len, evt_loop::DUMP_MAX_BYTES) << endl;

    switch (cmd)
    {
        case ECommand::ECHO:
            handleEcho(conn, cmdMsg, msgData);
            return;
        case ECommand::REG:
            handleRegister(conn, cmdMsg, msgData);
            return;
        default:
            break;
    }

    auto iter = context_->endpoints.find(conn->ID());
    if (iter == context_->endpoints.end()) {
        fprintf(stderr, "[CommandHandler::HandleCommand] Error: the connection(id: %d) can not to match any endpoint, "
                "maybe the connection not be registered or occurred errors for endpoint manager\n", conn->ID());
        int errcode = 1;
        string errmsg("the client maybe not be registered");
        sendResultMessage(conn, cmd, errcode, errmsg);
        //conn->Disconnect();
        return;
    }
    auto ep = iter->second;

    switch (cmd)
    {
        case ECommand::FWD:
            handleForward(ep, cmdMsg, msgData);
            break;
        case ECommand::UNFWD:
            handleUnforward(ep, cmdMsg, msgData);
            break;
        case ECommand::SUB:
            handleSubscribe(ep, cmdMsg, msgData);
            break;
        case ECommand::UNSUB:
            handleUnsubscribe(ep, cmdMsg, msgData);
            break;
        case ECommand::REJECT:
            handleReject(ep, cmdMsg, msgData);
            break;
        case ECommand::UNREJECT:
            handleUnreject(ep, cmdMsg, msgData);
            break;
        case ECommand::PUBLISH:
            handlePublishData(ep, cmdMsg, msgData);
            break;
        case ECommand::PUBLISH_2:
            handlePublishDataToTargets(ep, cmdMsg, msgData);
            break;
        case ECommand::SVC:
            if (cmdMsg->HasResponseFlag()) {
                handleServiceResponse(ep, cmdMsg, msgData);
            } else {
                handleServiceRequest(ep, cmdMsg, msgData);
            }
            break;
        case ECommand::INFO:
            handleInfo(ep, cmdMsg, msgData);
            break;
        case ECommand::EP_INFO:
            handleEndpointInfo(ep, cmdMsg, msgData);
            break;
        case ECommand::SETUP:
            handleSetup(ep, cmdMsg, msgData);
            break;
        case ECommand::PROXY:
            handleProxy(ep, cmdMsg, msgData);
            break;
        case ECommand::KICKOUT:
            handleKickout(ep, cmdMsg, msgData);
            break;
        case ECommand::RELOAD:
            handleReload(ep, cmdMsg, msgData);
            break;
        default:
            fprintf(stderr, "[CommandHandler::HandleCommand] Error: Unsupported command: %s(%d)\n", CommandToTag(cmd), (command_t)cmd);
            break;
    }
}

int CommandHandler::handleEcho(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();
    int8_t errcode = 0;
    auto [payload, payload_len] = cmdMsg->Payload();
    sendResultMessage(conn, cmd, errcode, payload, payload_len);

    return 0;
}

int CommandHandler::handleRegister(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data)
{
    // 1) authorize by access token
    // 2) set endpoint role - normal, admin, service, cluster node
    // 3) set endpoint id

    const ECommand cmd = cmdMsg->Command();

    CommandRegister reg_cmd;
    _DECODE_COMMAND_MESSAGE("handleRegister", cmdMsg, reg_cmd, conn);

    auto [errcode, errmsg, reg_result] = service_->register_endpoint(conn, reg_cmd);
    if (! errmsg.empty()) {
        cerr << "[handleRegister] Error: " << errmsg << endl;
    }

    if (reg_result) {
        string rsp_data = reg_result->encodeToJSON();
        sendResultMessage(conn, cmd, errcode, rsp_data);
    } else {
        sendResultMessage(conn, cmd, errcode, errmsg);
    }

    return errcode;
}

int CommandHandler::handleForward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    CommandForward cmd_fwd;
    _DECODE_COMMAND_MESSAGE("handleForward", cmdMsg, cmd_fwd, ep->Connection());

    auto [errcode, errmsg] = service_->forward(ep.get(), cmd_fwd);
    if (! errmsg.empty()) {
        cerr << "[handleForward] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handleUnforward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    CommandUnforward cmd_unfwd;
    _DECODE_COMMAND_MESSAGE("handleUnforward", cmdMsg, cmd_unfwd, ep->Connection());

    auto [errcode, errmsg] = service_->unforward(ep.get(), cmd_unfwd);
    if (! errmsg.empty()) {
        cerr << "[handleUnforward] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handleSubscribe(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData)
{
    const ECommand cmd = cmdMsg->Command();

    CommandSubscribe cmd_sub;
    _DECODE_COMMAND_MESSAGE("handleSubscribe", cmdMsg, cmd_sub, ep->Connection());

    auto [errcode, errmsg] = service_->subscribe(ep.get(), cmd_sub);
    if (! errmsg.empty()) {
        cerr << "[handleSubscribe] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handleUnsubscribe(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData)
{
    const ECommand cmd = cmdMsg->Command();

    CommandUnsubscribe cmd_unsub;
    _DECODE_COMMAND_MESSAGE("handleUnsubscribe", cmdMsg, cmd_unsub, ep->Connection());

    auto [errcode, errmsg] = service_->unsubscribe(ep.get(), cmd_unsub);
    if (! errmsg.empty()) {
        cerr << "[handleUnsubscribe] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handleReject(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData)
{
    const ECommand cmd = cmdMsg->Command();

    CommandReject cmd_rej;
    _DECODE_COMMAND_MESSAGE("handleReject", cmdMsg, cmd_rej, ep->Connection());

    auto [errcode, errmsg] = service_->reject(ep.get(), cmd_rej);
    if (! errmsg.empty()) {
        cerr << "[handleReject] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handleUnreject(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData)
{
    const ECommand cmd = cmdMsg->Command();

    CommandUnreject cmd_unrej;
    _DECODE_COMMAND_MESSAGE("handleUnreject", cmdMsg, cmd_unrej, ep->Connection());

    auto [errcode, errmsg] = service_->unreject(ep.get(), cmd_unrej);
    if (! errmsg.empty()) {
        cerr << "[handleUnreject] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return errcode;
}

int CommandHandler::handlePublishData(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    vector<EndpointPtr> targets;
    auto fwd_targets = ep->GetForwardTargets();
    if (fwd_targets.empty() || fwd_targets.contains(0)) {
        // broadcast
        for (auto iter : context_->normal_endpoints) {
            auto target_ep = iter.second;
            if (! service_->is_forwarding_allowed(ep.get(), target_ep.get())) {
                continue;
            }
            targets.push_back(target_ep);
        }
    } else {
        // multicast
        for (auto ep_id : fwd_targets) {
            auto iter = context_->normal_endpoints.find(ep_id);
            if (iter == context_->normal_endpoints.end()) {
                continue;
            }
            auto target_ep = iter->second;
            if (! service_->is_forwarding_allowed(ep.get(), target_ep.get())) {
                continue;
            }
            targets.push_back(target_ep);
        }
    }

    ((CommandMessage*)cmdMsg)->ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());
    for (auto target_ep : targets) {
        printf("[handlePublishData] forward message: size: %ld\n", data.size());
        target_ep->Connection()->Send(data);
    }

    int8_t errcode = 0;
    char result[64];
    snprintf(result, sizeof(result), "{\"total\": %ld}", targets.size());
    sendResultMessage(ep->Connection(), cmd, errcode, result, strlen(result));

    return 0;
}

int CommandHandler::handlePublishDataToTargets(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    auto pub_msg = cmdMsg->GetPublishingMessage();
    if (! pub_msg) {
        return handlePublishData(ep, cmdMsg, data);
    }

    printf("[handlePublishDataToTargets] msg_type: %d\n", pub_msg->msg_type);
    printf("[handlePublishDataToTargets] source: %d\n", pub_msg->source);
    printf("[handlePublishDataToTargets] n_targets: %d\n", pub_msg->n_targets);
    for (int i=0; i<pub_msg->n_targets; i++) {
        printf("%d, ", pub_msg->targets[i]);
    }
    printf("\n");

    MessageId msg_type = pub_msg->msg_type;
    vector<EndpointPtr> targets;

    if (pub_msg->n_targets > 0) {
        for (int i=0; i<pub_msg->n_targets; i++) {
            auto ep_id = pub_msg->targets[i];
            auto iter = context_->endpoints.find(ep_id);
            if (iter == context_->endpoints.end()) {
                continue;
            }
            auto target_ep = iter->second;
            if (! service_->is_forwarding_allowed(ep.get(), target_ep.get(), msg_type)) {
                continue;
            }
            targets.push_back(target_ep);
        }
    } else if (! context_->message_subscribers.empty()) {
        auto iter = context_->message_subscribers.find(msg_type);
        if (iter != context_->message_subscribers.end()) {
            auto ep_set = iter->second;
            for (auto ep_id : ep_set) {
                auto iter2 = context_->endpoints.find(ep_id);
                if (iter2 != context_->endpoints.end()) {
                    auto target_ep = iter2->second;
                    if (! service_->is_forwarding_allowed(ep.get(), target_ep.get(), msg_type)) {
                        continue;
                    }
                    targets.push_back(target_ep);
                }
            }
        }
    } else {
        return handlePublishData(ep, cmdMsg, data);
    }

    ((CommandMessage*)cmdMsg)->ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());
    for (auto target_ep : targets) {
        printf("[handlePublishDataToTargets] forward message: source: %d -> target: %d, size: %ld\n",
                ep->Id(), target_ep->Id(), data.size());
        target_ep->Connection()->Send(data);
    }

    int8_t errcode = 0;
    char result[64];
    snprintf(result, sizeof(result), "{\"total\": %ld}", targets.size());
    sendResultMessage(ep->Connection(), cmd, errcode, result, strlen(result));

    return 0;
}

int CommandHandler::handleServiceRequest(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ServiceMessage* svc_msg = cmdMsg->GetServiceMessage();
    printf("[handleServiceRequest] svc_type: %d\n", svc_msg->svc_type);
    printf("[handleServiceRequest] svc_cmd: %d\n", svc_msg->svc_cmd);
    printf("[handleServiceRequest] sess_id: %d\n", svc_msg->sess_id);
    printf("[handleServiceRequest] source: %d\n", svc_msg->source);

    auto svc_cmd = svc_msg->svc_cmd;
    EndpointPtr svc_ep;
    auto iter = context_->service_endpoints.find(svc_msg->svc_type);
    if (iter != context_->service_endpoints.end()) {
        auto ep_set = iter->second;
        for (auto target_ep : ep_set) {
            if (! service_->is_forwarding_allowed(ep.get(), target_ep.get(), svc_cmd)) {
                continue;
            }
            svc_ep = target_ep;
            break;
        }
    } else {
        auto iter = context_->service_endpoints.find(0);  // 0: if svc_type is 0 means for all service type
        if (iter != context_->service_endpoints.end()) {
            auto ep_set = iter->second;
            for (auto target_ep : ep_set) {
                if (! service_->is_forwarding_allowed(ep.get(), target_ep.get(), svc_cmd)) {
                    continue;
                }
                svc_ep = target_ep;
                break;
            }
        }
    }

    if (svc_ep) {
        ((CommandMessage*)cmdMsg)->ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());
        printf("[handleServiceRequest] forward message: size: %ld\n", data.size());
        svc_ep->Connection()->Send(data);
    } else {
        // respond error message
        ResultMessage result_msg;
        result_msg.errcode = 1;
        string errmsg("Can not find service or request is not allowed");

        auto rspCmdMsg = ((CommandMessage*)cmdMsg);
        rspCmdMsg->SetResponseFlag();
        rspCmdMsg->SetPayloadLen(sizeof(ServiceMessage) + sizeof(ResultMessage) + errmsg.size());
        rspCmdMsg->ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());

        ep->Connection()->Send(rspCmdMsg->Data(), rspCmdMsg->HeaderSize());
        ep->Connection()->Send((char*)svc_msg, sizeof(ServiceMessage));
        ep->Connection()->Send((char*)&result_msg, sizeof(result_msg));
        ep->Connection()->Send(errmsg);
    }
    return 0;
}

int CommandHandler::handleServiceResponse(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ServiceMessage* svc_msg = cmdMsg->GetServiceMessage();
    printf("[handleServiceResponse] svc_type: %d\n", svc_msg->svc_type);
    printf("[handleServiceResponse] svc_cmd: %d\n", svc_msg->svc_cmd);
    printf("[handleServiceResponse] sess_id: %d\n", svc_msg->sess_id);
    printf("[handleServiceResponse] source: %d\n", svc_msg->source);

    auto iter = context_->endpoints.find(svc_msg->source);
    if (iter != context_->endpoints.end()) {
        auto source_ep = iter->second;
        ((CommandMessage*)cmdMsg)->ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());
        source_ep->Connection()->Send(data);
    } else {
        printf("[handleServiceResponse] Error: can not find service request source: %d\n", svc_msg->source);
    }
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

    const ECommand cmd = cmdMsg->Command();

    _CHECK_ROLE_PERMISSION("handleInfo", ep->GetRole(), EEndpointRole::Admin);

    CommandInfoReq cmd_info_req;
    _DECODE_COMMAND_MESSAGE("handleInfo", cmdMsg, cmd_info_req, ep->Connection());

    auto cmd_info = service_->get_stats(cmd_info_req);
    string rsp_data = cmd_info->encodeToJSON();
    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), cmd, errcode, rsp_data);

    return 0;
}

int CommandHandler::handleEndpointInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    CommandInfoReq cmd_info_req;
    _DECODE_COMMAND_MESSAGE("handleEndpointInfo", cmdMsg, cmd_info_req, ep->Connection());

    if (! (ep->GetRole() != EEndpointRole::Admin && cmd_info_req.endpoint_id == ep->Id())) {
        _CHECK_ROLE_PERMISSION("handleEndpointInfo", ep->GetRole(), EEndpointRole::Admin);
    }

    int8_t errcode = 0;
    auto [status, errmsg, cmd_ep_info] = service_->get_endpoint_stats(cmd_info_req);
    if (status != 0) {
        errcode = 1;
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg);
    } else {
        string rsp_data = cmd_ep_info->encodeToJSON();
        sendResultMessage(ep->Connection(), cmd, errcode, rsp_data);
    }

    return 0;
}

int CommandHandler::handleSetup(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    // 0) authorize admin token
    // 1) set/generate admin token
    // 2) set/generate endpoint access token
    // 3) -- set serving mode

    const ECommand cmd = cmdMsg->Command();

    _CHECK_ROLE_PERMISSION("handleSetup", ep->GetRole(), EEndpointRole::Admin);

    CommandSetup cmd_setup;
    _DECODE_COMMAND_MESSAGE("handleSetup", cmdMsg, cmd_setup, ep->Connection());

    auto [errcode, errmsg] = service_->setup(cmd_setup);
    if (! errmsg.empty()) {
        cerr << "[handleSetup] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return 0;
}

int CommandHandler::handleProxy(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleKickout(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    // 1) kickout endpoint(s)

    const ECommand cmd = cmdMsg->Command();

    _CHECK_ROLE_PERMISSION("handleKickout", ep->GetRole(), EEndpointRole::Admin);

    CommandKickout cmd_kickout;
    _DECODE_COMMAND_MESSAGE("handleKickout", cmdMsg, cmd_kickout, ep->Connection());

    auto [errcode, errmsg] = service_->kickout_endpoint(cmd_kickout);
    if (! errmsg.empty()) {
        cerr << "[handleKickout] Error: " << errmsg << endl;
    }

    sendResultMessage(ep->Connection(), cmd, errcode, errmsg);

    return 0;
}

int CommandHandler::handleReload(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    const ECommand cmd = cmdMsg->Command();

    if (ep->GetRole() != EEndpointRole::Admin) {
        int8_t errcode = 1;
        std::stringstream ss;
        ss << "Failed: Operation not allowed for role: " << EndpointRoleToTag(ep->GetRole());
        string errmsg = ss.str();
        cerr << "[handleReload] Error: " << errmsg << endl;
        sendResultMessage(ep->Connection(), cmd, errcode, errmsg);
        return errcode;
    }

    return 0;
}

size_t CommandHandler::sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode, const string& data)
{
    return sendResultMessage(conn, cmd, errcode, data.data(), data.size());
}

size_t CommandHandler::sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode,
        const char* payload, size_t payload_len)
{
    printf("[sendResultMessage] response, payload len: %ld\n", payload_len);
    if (payload_len > 0) {
        cout << DumpHexWithChars(payload, payload_len, evt_loop::DUMP_MAX_BYTES) << endl;
    }

    CommandMessage cmdMsg;
    cmdMsg.SetCommand(cmd);
    cmdMsg.SetResponseFlag();
    cmdMsg.SetToJSON();
    cmdMsg.SetPayloadLen(sizeof(ResultMessage) + payload_len);
    cmdMsg.ConvertToNetworkMessage(context_->switch_server->IsMessagePayloadLengthIncludingSelf());

    ResultMessage resultMsg;
    resultMsg.errcode = errcode;

    conn->Send((char*)&cmdMsg, sizeof(cmdMsg));
    size_t sent_bytes = sizeof(cmdMsg);
    conn->Send((char*)&resultMsg, sizeof(resultMsg));
    sent_bytes += sizeof(resultMsg);
    if (payload && payload_len > 0) {
        conn->Send(payload, payload_len);
        sent_bytes += sizeof(payload_len);
    }

    return sent_bytes;
}
