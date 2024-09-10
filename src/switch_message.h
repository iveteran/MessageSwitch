#ifndef _SWITCH_MESSAGE_H_
#define _SWITCH_MESSAGE_H_

#include <cstdint>
#include <cstring>

#pragma pack(1)
struct CommandMessage {
    uint8_t cmd;            // ECommand
    uint8_t flag;           // flag[7]: 0 -> request, 1 -> response
    uint32_t payload_len;   // payload_len supports including self size
    char payload[0];        // placehoder field
 
    CommandMessage() { memset(this, 0, sizeof(*this)); }
    void SetResponseFlag() { flag |= 0b00000001; }
    bool HasResponseFlag() { return flag & 0b00000001; }
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
