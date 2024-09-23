#include <stdio.h>
#include "switch_client.h"
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_console.h"
#include "sc_options.h"

SwitchClient::SwitchClient(const char* host, uint16_t port, uint32_t ep_id) :
    client_(nullptr), endpoint_id_(ep_id)
{
    InitComponents();
    InitClient(host, port);
}

SwitchClient::SwitchClient(SCOptions* options) :
    client_(nullptr), endpoint_id_(0), options_(options)
{
    if (options->endpoint_id > 0) {
        endpoint_id_ = options->endpoint_id;
    }

    InitComponents();
    InitClient(options->server_host.c_str(), options->server_port);
}

void SwitchClient::Cleanup()
{
    console_->Destory();
}

void SwitchClient::InitComponents()
{
    cmd_handler_ = new SCCommandHandler(this);
    console_ = new SCConsole(endpoint_id_, cmd_handler_);
    console_->registerCommands();
}

void SwitchClient::InitClient(const char* host, uint16_t port)
{
    //client_ = std::make_shared<TcpClient>(host, port, MessageType::CUSTOM);
    client_ = new TcpClient(host, port, MessageType::CUSTOM);

    auto msg_hdr_desc = CreateMessageHeaderDescription();
    client_->SetMessageHeaderDescription(msg_hdr_desc);
    client_->SetAutoReconnect(false);

    auto client_cbs = std::make_shared<TcpCallbacks>();
    client_cbs->on_msg_recvd_cb = std::bind(&SwitchClient::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    client_cbs->on_closed_cb = std::bind(&SwitchClient::OnConnectionClosed, this, std::placeholders::_1);

    client_->SetNewClientCallback(std::bind(&SwitchClient::OnConnectionCreated, this, std::placeholders::_1));
    client_->SetTcpCallbacks(client_cbs);
    //client_->EnableKeepAlive(true);

    client_->Connect();
}

void SwitchClient::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[OnMessageRecvd] received message, fd: %d, id: %d, size: %lu\n", conn->FD(), conn->ID(), msg->Size());
    printf("[OnMessageRecvd] message bytes:\n");
    cout << msg->DumpHexWithChars(256) << endl;

    CommandMessage* cmdMsg = convertMessageToCommandMessage(msg,
            client_->GetMessageHeaderDescription()->is_payload_len_including_self);

    if (cmdMsg->HasResponseFlag()) {
        cmd_handler_->HandleCommandResult(conn, cmdMsg);
    } else {
        if (ECommand(cmdMsg->cmd) == ECommand::DATA) {
            cmd_handler_->HandleEndpointData(msg);
        }
    }
}

void SwitchClient::OnConnectionCreated(TcpConnection* conn)
{
    printf("[OnConnectionCreated] connection created, fd: %d\n", conn->FD());

    cmd_handler_->Echo("echo");
}
void SwitchClient::OnConnectionClosed(TcpConnection* conn)
{
    printf("[SwitchClient::OnConnectionClosed] fd: %d\n", conn->FD());
}

HeaderDescriptionPtr SwitchClient::CreateMessageHeaderDescription()
{
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = sizeof(CommandMessage);
    //msg_hdr_desc->payload_len_offset = 2;  // jump over the size of fields cmd and flag
    msg_hdr_desc->payload_len_offset = offsetof(struct CommandMessage, payload_len);
    msg_hdr_desc->payload_len_bytes = sizeof(CommandMessage::payload_len);
    msg_hdr_desc->is_payload_len_including_self = true;
    return msg_hdr_desc;
}
