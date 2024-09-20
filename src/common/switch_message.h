#ifndef _SWITCH_MESSAGE_H_
#define _SWITCH_MESSAGE_H_

#include <cstdint>
#include <cstring>

#pragma pack(1)
struct CommandMessage {
    uint8_t cmd;            // ECommand
    struct {
        uint8_t unused:5;
        uint8_t codec:2;    // 2 bits, codec of above layer, 0: undefined, 1: json, 2: protobuf, 3: reserved
        uint8_t req_rsp:1;  // 1 bit,  request or response,  0: request, 1: response
    } flag;
    uint32_t payload_len;   // payload_len supports including self size
    char payload[0];        // placehoder field
 
    CommandMessage() { memset(this, 0, sizeof(*this)); }

    void SetResponseFlag() { flag.req_rsp = 1; }
    bool HasResponseFlag() const { return flag.req_rsp; }

    void SetToJSON() { flag.codec = 1; }
    bool IsJSON() const { return flag.codec == 1; }

    void SetToPB() { flag.codec = 2; }
    bool IsPB() const { return flag.codec == 2; }

    void ResetCodec() { flag.codec = 0; }
};

struct ResultMessage {
    int8_t errcode;
    char data[0];       // placehoder field

    ResultMessage() { memset(this, 0, sizeof(*this)); }
};
#pragma pack() 

enum class ECommand : uint8_t {
    UNDEFINED,
    ECHO,
    REG,
    FWD,
    DATA,
    INFO,
    SETUP,
    PROXY,
    KICKOUT,
    EXIT,
    RELOAD,
    RESULT = 255,
};
const char* CommandToTag(ECommand cmd);

#endif  // _SWITCH_MESSAGE_H_
