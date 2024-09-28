#include "switch_message.h"
#include <eventloop/message.h>
#include <arpa/inet.h>

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
        case ECommand::DATA:
            cmd_tag = "DATA";
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

CommandMessage*
convertMessageToCommandMessage(const Message* msg, bool isMsgPayloadLengthIncludingSelf)
{
    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    cmdMsg->payload_len = ntohl(cmdMsg->payload_len);
    if (isMsgPayloadLengthIncludingSelf) {
        cmdMsg->payload_len -= sizeof(cmdMsg->payload_len);
    }
    return cmdMsg;
}

// Reverse to network message without header of CommandMessage
std::pair<size_t, const char*>
extractMessagePayload(CommandMessage* cmdMsg, bool isMsgPayloadLengthIncludingSelf)
{
    char* payload_ptr = cmdMsg->payload;
    size_t payload_len = cmdMsg->payload_len;
    if (isMsgPayloadLengthIncludingSelf) {
        payload_ptr -= sizeof(cmdMsg->payload_len);
        payload_len += sizeof(cmdMsg->payload_len);
    }
    cmdMsg->payload_len = htonl(payload_len);
    return std::make_pair(payload_len, payload_ptr);
}

// Reverse to network message with header of CommandMessage
Message*
reverseToNetworkMessage(CommandMessage* cmdMsg, bool isMsgPayloadLengthIncludingSelf)
{
    if (isMsgPayloadLengthIncludingSelf) {
        cmdMsg->payload_len += sizeof(cmdMsg->payload_len);
    }
    cmdMsg->payload_len = htonl(cmdMsg->payload_len);
    return (Message*)cmdMsg;
}
