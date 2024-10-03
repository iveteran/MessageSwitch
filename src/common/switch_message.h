#ifndef _SWITCH_MESSAGE_H_
#define _SWITCH_MESSAGE_H_

#include <cstdint>
#include <cstring>
#include <utility>

#pragma pack(1)
struct CommandMessage {
    uint8_t cmd;            // ECommand
    struct {
        uint8_t unused:5;
        uint8_t codec:2;    // 2 bits, codec of above layer, 0: undefined, 1: json, 2: protobuf, 3: unused
        uint8_t req_rsp:1;  // 1 bit,  request or response,  0: request, 1: response
    } flag;
    uint8_t svc_type;       // service type of up layer, 0: undefined, used for service routing
    uint32_t payload_len;   // payload_len supports including self size
    char payload[0];        // placeholder field
 
    CommandMessage() { memset(this, 0, sizeof(*this)); }

    void SetResponseFlag() { flag.req_rsp = 1; }
    bool HasResponseFlag() const { return flag.req_rsp; }

    void SetToJSON() { flag.codec = 1; }
    bool IsJSON() const { return flag.codec == 1; }

    void SetToPB() { flag.codec = 2; }
    bool IsPB() const { return flag.codec == 2; }

    void ResetCodec() { flag.codec = 0; }
};
#pragma pack()

namespace evt_loop {
    class Message;
}
using evt_loop::Message;

CommandMessage*
convertMessageToCommandMessage(const Message* msg, bool isMsgPayloadLengthIncludingSelf);
// Reverse to network message without header of CommandMessage
std::pair<size_t, const char*>
extractMessagePayload(CommandMessage* cmdMsg, bool isMsgPayloadLengthIncludingSelf);
// Reverse to network message with header of CommandMessage
Message*
reverseToNetworkMessage(CommandMessage* cmdMsg, bool isMsgPayloadLengthIncludingSelf);

CommandMessage createHeartbeatRequest(bool isMsgPayloadLengthIncludingSelf);
CommandMessage createHeartbeatResponse(bool isMsgPayloadLengthIncludingSelf);

#pragma pack(1)
struct ResultMessage {
    int8_t errcode;
    char data[0];       // placeholder field

    ResultMessage() { memset(this, 0, sizeof(*this)); }
};
#pragma pack()

enum class ECommand : uint8_t {
    UNDEFINED,
    ECHO,
    REG,
    FWD,   // publish targets
    UNFWD,
    SUB,
    UNSUB,
    REJECT,
    UNREJECT,
    PUBLISH,    // publish data
    SVC,   // service request
    INFO,
    EP_INFO,
    SETUP,
    PROXY,
    KICKOUT,
    EXIT,
    RELOAD,
    HEARTBEAT = 254,
    RESULT = 255,
};
const char* CommandToTag(ECommand cmd);

#endif  // _SWITCH_MESSAGE_H_
