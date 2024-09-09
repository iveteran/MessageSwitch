#ifndef _SWITCH_SERVER_H
#define _SWITCH_SERVER_H

#include "switch_message.h"
#include "switch_endpoint.h"
#include "switch_context.h"
#include "el.h"

using namespace evt_loop;

class SwitchServer
{
    public:
    SwitchServer(const char* host="0.0.0.0", uint16_t port=10000);

    void OnSignal(SignalHandler* sh, uint32_t signo)
    {
        printf("SwitchServer::Shutdown\n");
        EV_Singleton->StopLoop();
    }
    HeaderDescriptionPtr GetMessageHeaderDescription() const
    {
        return server_.GetMessageHeaderDescription();
    }
    bool IsMessagePayloadLengthIncludingSelf() const
    {
        return server_.GetMessageHeaderDescription()->is_payload_len_including_self;
    }

    private:
    HeaderDescriptionPtr CreateMessageHeaderDescription();

    void OnConnectionReady(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);
    void OnMessageRecvd(TcpConnection* conn, const Message* msg);

    void HandleCommand(TcpConnection* conn, const Message* msg);

    private:
    uint32_t node_id_;
    TcpServer server_;
    SwitchContextPtr context_;
};

#endif // _SWITCH_SERVER_H
