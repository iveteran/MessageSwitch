#include <stdio.h>
#include "switch_client.h"
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_console.h"
#include "sc_options.h"
#include "sc_context.h"
#include "sc_peer.h"

SwitchClient::SwitchClient(const char* host, uint16_t port, EndpointId ep_id,
        bool enable_console, const char* console_sub_prompt)
    : endpoint_id_(ep_id), enable_console_(enable_console),
    console_sub_prompt_(console_sub_prompt)
{
    peer_ = new SCPeer(host, port);
    CreateComponents();
    peer_->Connect();
}

SwitchClient::SwitchClient(SCOptions* options)
    : peer_(nullptr), endpoint_id_(0), options_(options)
{
    if (options->endpoint_id > 0) {
        endpoint_id_ = options->endpoint_id;
    }
    enable_console_ = options->enable_console; 
    console_sub_prompt_ = options->console_sub_prompt;

    peer_ = new SCPeer(options->server_host.c_str(), options->server_port);
    peer_->SetClosedCallback(std::bind(&SwitchClient::OnPeerClosed, this));
    peer_->SetConnectedCallback(std::bind(&SwitchClient::OnPeerConnected, this));

    CreateComponents();

    peer_->SetMessageCallback(
            std::bind(
                &SCCommandHandler::HandleCommandMessage,
                cmd_handler_,
                std::placeholders::_1,
                std::placeholders::_2)
            );
    peer_->Connect();
}

void SwitchClient::Cleanup()
{
    if (console_) {
        console_->Destory();
        delete console_;
        console_ = nullptr;
    }

    delete cmd_handler_;
    cmd_handler_ = nullptr;

    delete context_;
    context_ = nullptr;

    delete peer_;
    peer_ = nullptr;
}

void SwitchClient::Stop()
{
    Cleanup();
    EV_Singleton->StopLoop();
}

void SwitchClient::Start()
{
    EV_Singleton->StartLoop();
}

void SwitchClient::CreateComponents()
{
    context_ = new SCContext(this);
    cout << "Context: " << context_->ToString() << endl;

    cmd_handler_ = new SCCommandHandler(this, peer_->GetMessageHeaderDescription()->is_payload_len_including_self);
    if (enable_console_) {
        console_ = new SCConsole(this, cmd_handler_, console_sub_prompt_.c_str());
        console_->registerCommands();
    }
}

void SwitchClient::OnPeerConnected()
{
    RegisterSelf();
}

void SwitchClient::OnPeerClosed()
{
}

TcpConnectionPtr SwitchClient::Connection()
{
    return peer_->Connection();
}
bool SwitchClient::IsConnected() const
{
    return  peer_->IsConnected();
}
bool SwitchClient::Reconnect()
{
    return peer_->Reconnect();
}
void SwitchClient::EnableHeartbeat()
{
    peer_->EnableHeartbeat();
}
void SwitchClient::DisableHeartbeat()
{
    peer_->DisableHeartbeat();
}
bool SwitchClient::IsHeartbeatEnabled()
{
    return peer_->IsHeartbeatEnabled();
}

void SwitchClient::RegisterSelf()
{
    uint32_t ep_id = context_->endpoint_id;
    EEndpointRole role = context_->role;
    ServiceType svc_type = context_->svc_type;
    string access_code(context_->access_code);
    bool with_token = false;
    cmd_handler_->Register(ep_id, role, access_code, with_token, svc_type);
}
