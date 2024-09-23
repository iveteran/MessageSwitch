#ifndef _SWITCH_SERVER_H
#define _SWITCH_SERVER_H

#include "switch_options.h"
#include "switch_context.h"
#include "switch_service.h"
#include "switch_console.h"
#include "switch_command_handler.h"
#include <eventloop/el.h>

using namespace evt_loop;

class SwitchServer
{
    public:
    SwitchServer(const char* host="0.0.0.0", uint16_t port=10000);
    SwitchServer(const OptionsPtr& options);
    bool init(const char* host, uint16_t port);
    void InitComponents();
    void InitServer(const char* host, uint16_t port);
    void OnSignal(SignalHandler* sh, uint32_t signo);
    void Exit();

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
    SwitchServicePtr GetService() const { return service_; }
    size_t GetClientsTotal() const { return server_->GetConnectionNumber(); }

    private:
    HeaderDescriptionPtr CreateMessageHeaderDescription();

    void OnConnectionReady(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);
    void OnMessageRecvd(TcpConnection* conn, const Message* msg);

    private:
    TcpServerPtr server_;
    uint32_t node_id_;
    OptionsPtr options_;
    SwitchContextPtr context_;
    SwitchServicePtr service_;
    SwitchConsolePtr console_;
    CommandHandlerPtr cmd_handler_;
};

#endif // _SWITCH_SERVER_H
