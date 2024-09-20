#ifndef _SC_COMMAND_HANDLER_H
#define _SC_COMMAND_HANDLER_H

#include "switch_message.h"

#include <string>
using std::string;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

class CommandMessage;
class SwitchClient;

class SCCommandHandler {
    public:
    SCCommandHandler(SwitchClient* client) : client_(client) {}

    void Register();
    void GetInfo();
    void SendData();
    void ForwardData();
    void Setup();
    void Kickout();
    void Reload();

    void HandleGetInfoResult(CommandMessage* cmdMsg, const string& data);

    size_t SendCommandMessage(ECommand cmd, const string& payload);
    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& payload);

    void HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg);

    private:
    SwitchClient* client_;
};

#endif // _SC_COMMAND_HANDLER_H
