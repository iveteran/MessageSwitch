#include "switch_message.h"
#include <eventloop/message.h>
#include <arpa/inet.h>
#include <cassert>
#include <cmath>

const char* CommandToTag(ECommand cmd) {
    const char* cmd_tag = "UNDEFINED";
    switch (cmd) {
        case ECommand::ECHO:
            cmd_tag = "ECHO";
            break;
        case ECommand::REG:
            cmd_tag = "REG";
            break;
        case ECommand::FWD:
            cmd_tag = "FWD";
            break;
        case ECommand::SUB:
            cmd_tag = "SUB";
            break;
        case ECommand::UNSUB:
            cmd_tag = "UNSUB";
            break;
        case ECommand::REJECT:
            cmd_tag = "REJECT";
            break;
        case ECommand::UNREJECT:
            cmd_tag = "UNREJECT";
            break;
        case ECommand::PUBLISH:
            cmd_tag = "PUBLISH";
            break;
        case ECommand::SVC:
            cmd_tag = "SVC";
            break;
        case ECommand::INFO:
            cmd_tag = "INFO";
            break;
        case ECommand::EP_INFO:
            cmd_tag = "EP_INFO";
            break;
        case ECommand::SETUP:
            cmd_tag = "SETUP";
            break;
        case ECommand::PROXY:
            cmd_tag = "PROXY";
            break;
        case ECommand::KICKOUT:
            cmd_tag = "KICKOUT";
            break;
        case ECommand::EXIT:
            cmd_tag = "EXIT";
            break;
        case ECommand::RELOAD:
            cmd_tag = "RELOAD";
            break;
        case ECommand::RESULT:
            cmd_tag = "RESULT";
            break;
        default:
            cmd_tag = "UNDEFINED";
            break;
    }
    return cmd_tag;
}

std::pair<const char*, payload_size_t>
CommandMessage::Payload() const
{
    auto payload_len = payload_len_;
    auto payload = (char*)payload_;
    return { payload, payload_len };
}

payload_size_t CommandMessage::PayloadLen() const
{
    auto [_, payload_len] = Payload();
    return payload_len;
}

const ResultMessage*
CommandMessage::GetResultMessage() const
{
    if (HasResponseFlag()) {
        {
            auto [payload, _] = Payload();
            //auto payload = payload_;
            return (const ResultMessage*)(payload);
        }
    } else {
        return nullptr;
    }
}

size_t CommandMessage::GetResultMessageContentSize() const
{
    if (HasResponseFlag()) {
        auto payload_len = PayloadLen();
        //auto payload_len = payload_len_;
        return payload_len - sizeof(ResultMessage);
    } else {
        return 0;
    }
}

// Convert to network message with header of CommandMessage
Message*
CommandMessage::ConvertToNetworkMessage(bool isMsgPayloadLengthIncludingSelf)
{
    int payload_len_bytes = sizeof(payload_len_);
    auto payload_len = payload_len_;
    if (payload_len > 0 && isMsgPayloadLengthIncludingSelf) {
        payload_len += payload_len_bytes;
    }
    assert (payload_len < exp2(payload_len_bytes * 8) && "protocol error: payload length integer overflow");
    payload_len_ =
        (payload_len_bytes == sizeof(int32_t) ? htonl(payload_len) :
        (payload_len_bytes == sizeof(int16_t) ? htons(payload_len) :
        (payload_len_bytes == sizeof(int8_t) ? payload_len : 0)));
    return (Message*)this;
}

CommandMessage*
CommandMessage::FromNetworkMessage(const Message* msg, bool isMsgPayloadLengthIncludingSelf)
{
    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    int payload_len_bytes = sizeof(cmdMsg->payload_len_);
    auto payload_len = cmdMsg->payload_len_;
    cmdMsg->payload_len_ =
        (payload_len_bytes == sizeof(int32_t) ? ntohl(payload_len) :
        (payload_len_bytes == sizeof(int16_t) ? ntohs(payload_len) :
        (payload_len_bytes == sizeof(int8_t) ? payload_len : 0)));
    if (cmdMsg->payload_len_ >= payload_len_bytes && isMsgPayloadLengthIncludingSelf) {
        cmdMsg->payload_len_ -= payload_len_bytes;
    }
    return cmdMsg;
}

CommandMessage CommandMessage::CreateHeartbeatRequest()
{
    CommandMessage msg;
    msg.cmd_ = uint8_t(ECommand::HEARTBEAT);
    msg.payload_len_ = 0;
    return msg;
}

CommandMessage CommandMessage::CreateHeartbeatResponse()
{
    CommandMessage msg;
    msg.cmd_ = uint8_t(ECommand::HEARTBEAT);
    msg.SetResponseFlag();
    msg.payload_len_ = 0;
    return msg;
}
