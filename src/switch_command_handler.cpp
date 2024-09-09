#include <sstream>
#include "switch_command_handler.h"
#include "switch_server.h"

int CommandHandler::handleEcho(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    printf("------> cmd: Echo\n");
    printf("------> payload_len: %d\n", cmdMsg->payload_len);
    printf("------> payload: %s\n", cmdMsg->payload);

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), ECommand::RESULT, errcode, cmdMsg->payload, cmdMsg->payload_len);

    return 0;
}

int CommandHandler::handleRegister(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    printf("------> cmd: Register\n");
    printf("------> payload_len: %d\n", cmdMsg->payload_len);
    printf("------> payload: %s\n", cmdMsg->payload);

    int8_t errcode = 0;
    sendResultMessage(ep->Connection(), ECommand::RESULT, errcode);

    return 0;
}

int CommandHandler::handleForward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    string payload(cmdMsg->payload, cmdMsg->payload_len);
    stringstream ss(payload);
    string item;
    vector<EndpointId> targets;
    while (std::getline(ss, item, ','))
        targets.push_back(std::stoi(item));
    ep->SetForwardTargets(targets);
    return 0;
}

int CommandHandler::handleData(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
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
    return 0;
}

int CommandHandler::handleInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleSetup(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleProxy(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleKickout(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

int CommandHandler::handleExit(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data)
{
    return 0;
}

size_t CommandHandler::sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode,
        const char* data, size_t data_len)
{
    size_t sent_bytes = 0;

    CommandMessage cmdMsg;
    cmdMsg.cmd = cmd;
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
