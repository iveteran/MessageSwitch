#ifndef _SWITCH_COMMAND_HANDLER_H
#define _SWITCH_COMMAND_HANDLER_H

#include <cstdint>
#include <string>

#include "switch_endpoint.h"
#include "switch_message.h"
#include "switch_context.h"

using std::string;

namespace evt_loop {
    class Message;
}
using evt_loop::Message;

class CommandHandler {
public:
    CommandHandler(SwitchContextPtr context) : context_(context)
    {}

    int handleEcho(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleRegister(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleForward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleData(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleSetup(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleProxy(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleKickout(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleExit(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);

    size_t sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode,
            const char* data = NULL, size_t data_len = 0);

    // Reverse to network message without header of CommandMessage
    std::pair<size_t, const char*>
        extractMessagePayload(CommandMessage* cmdMsg);
    // Reverse to network message with header of CommandMessage
    Message* reverseToNetworkMessage(CommandMessage* cmdMsg);

private:
    SwitchContextPtr context_;
};
typedef std::shared_ptr<CommandHandler> CommandHandlerPtr;

#endif  // _SWITCH_COMMAND_HANDLER_H
