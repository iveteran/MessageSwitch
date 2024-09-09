#ifndef _SWITCH_MESSAGE_H_
#define _SWITCH_MESSAGE_H_

#include <cstdint>

#pragma pack(1)
struct CommandMessage {
    uint8_t cmd;
    uint32_t payload_len;   // payload_len supports including self size
    char payload[0];        // placehoder field
};

struct ResultMessage {
    int8_t errcode;
    char data[0];       // placehoder field
};
#pragma pack() 

enum ECommand {
    CMD_UNDEFINED,
    ECHO,
    REG,
    FWD,
    DATA,
    INFO,
    SETUP,
    PROXY,
    KICKOUT,
    EXIT,
    NEW_EP,
    RESULT = 255,
};

#endif  // _SWITCH_MESSAGE_H_
