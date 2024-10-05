#ifndef _SWITCH_COMMAND_HANDLER_H
#define _SWITCH_COMMAND_HANDLER_H

#include <cstdint>
#include <string>

#include "switch_endpoint.h"
#include "switch_message.h"
#include "switch_context.h"
#include "switch_service.h"

using std::string;

namespace evt_loop {
    class Message;
}
using evt_loop::Message;

class CommandRegister;

class CommandHandler {
public:
    CommandHandler(SwitchContextPtr context, SwitchServicePtr service) :
        context_(context), service_(service)
    {}

    void handleCommand(TcpConnection* conn, const Message* msg);

    int handleEcho(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data);
    int handleRegister(TcpConnection* conn, const CommandMessage* cmdMsg, const string& data);
    int handleForward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleUnforward(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleSubscribe(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData);
    int handleUnsubscribe(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData);
    int handleReject(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData);
    int handleUnreject(EndpointPtr ep, const CommandMessage* cmdMsg, const string& msgData);
    int handlePublishData(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handlePublishDataToTargets(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleServiceRequest(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleServiceResponse(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleEndpointInfo(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleSetup(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleProxy(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleKickout(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);
    int handleReload(EndpointPtr ep, const CommandMessage* cmdMsg, const string& data);

    size_t sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode, const string& data);
    size_t sendResultMessage(TcpConnection* conn, ECommand cmd, int8_t errcode,
            const char* data = NULL, size_t data_len = 0);

private:
    SwitchContextPtr context_;
    SwitchServicePtr service_;
};
typedef std::shared_ptr<CommandHandler> CommandHandlerPtr;

#endif  // _SWITCH_COMMAND_HANDLER_H
