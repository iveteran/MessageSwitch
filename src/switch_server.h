#ifndef _SWITCH_SERVER_H
#define _SWITCH_SERVER_H

#include "switch_message.h"
#include "switch_endpoint.h"
#include "switch_context.h"
#include "switch_service.h"
#include "switch_options.h"
#include <eventloop/el.h>

using namespace evt_loop;

class SwitchServer
{
    public:
    SwitchServer(const char* host="0.0.0.0", uint16_t port=10000);
    SwitchServer(const OptionsPtr& options);
    bool init(const char* host, uint16_t port);
    void OnSignal(SignalHandler* sh, uint32_t signo);

    HeaderDescriptionPtr GetMessageHeaderDescription() const
    {
        return server_->GetMessageHeaderDescription();
    }
    bool IsMessagePayloadLengthIncludingSelf() const
    {
        return server_->GetMessageHeaderDescription()->is_payload_len_including_self;
    }
    uint32_t NodeId() const { return node_id_; }
    OptionsPtr GetOptions() const { return options_; }
    SwitchContextPtr GetContext() const { return context_; }

    private:
    HeaderDescriptionPtr CreateMessageHeaderDescription();

    void OnConnectionReady(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);
    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    int handleConsoleCommand_Clients(const vector<string>& argv);

    void HandleCommand(TcpConnection* conn, const Message* msg);

    private:
    TcpServerPtr server_;
    uint32_t node_id_;
    SwitchContextPtr context_;
    SwitchServicePtr service_;
    OptionsPtr options_;
};

#endif // _SWITCH_SERVER_H
