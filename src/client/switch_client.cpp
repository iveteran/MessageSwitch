#include <stdio.h>
#include "switch_client.h"
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_console.h"
#include "sc_options.h"
#include "sc_context.h"

SwitchClient::SwitchClient(const char* host, uint16_t port, uint32_t ep_id) :
    client_(nullptr), endpoint_id_(ep_id)
{
    InitClient(host, port);
    InitComponents();
    client_->Connect();

}

SwitchClient::SwitchClient(SCOptions* options) :
    client_(nullptr), endpoint_id_(0), options_(options)
{
    if (options->endpoint_id > 0) {
        endpoint_id_ = options->endpoint_id;
    }

    InitClient(options->server_host.c_str(), options->server_port);
    InitComponents();
    client_->Connect();
}

void SwitchClient::Cleanup()
{
    console_->Destory();

    delete cmd_handler_;
    cmd_handler_ = nullptr;

    delete console_;
    console_ = nullptr;

    delete context_;
    context_ = nullptr;

    delete client_;
    client_ = nullptr;
}

void SwitchClient::Exit()
{
    Cleanup();
    EV_Singleton->StopLoop();
}

void SwitchClient::InitComponents()
{
    context_ = new SCContext(this);
    cout << "Context: " << context_->ToString() << endl;

    cmd_handler_ = new SCCommandHandler(this);
    console_ = new SCConsole(this, cmd_handler_);
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

    //client_->Connect();
}

void SwitchClient::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[OnMessageRecvd] received message, fd: %d, id: %d, size: %lu, playload size: %lu\n",
            conn->FD(), conn->ID(), msg->Size(), msg->PayloadSize());
    printf("[OnMessageRecvd] message bytes:\n");
    cout << msg->DumpHexWithChars(evt_loop::DUMP_MAX_BYTES) << endl;
    //printf("[OnMessageRecvd] message payload bytes:\n");
    //cout << DumpHexWithChars(msg->Payload(), msg->PayloadSize(), evt_loop::DUMP_MAX_BYTES) << endl;

    CommandMessage* cmdMsg = convertMessageToCommandMessage(msg,
            client_->GetMessageHeaderDescription()->is_payload_len_including_self);

    if (cmdMsg->HasResponseFlag()) {
        cmd_handler_->HandleCommandResult(conn, cmdMsg);
    } else {
        if (ECommand(cmdMsg->cmd) == ECommand::PUBLISH) {
            cmd_handler_->HandlePublishData(conn, cmdMsg);
        } else if (ECommand(cmdMsg->cmd) == ECommand::SVC) {
            cmd_handler_->HandleServiceRequest(conn, cmdMsg);
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
    auto hb_req = createHeartbeatRequest(msg_hdr_desc->is_payload_len_including_self);
    msg_hdr_desc->heartbeat_request = string((char*)&hb_req, sizeof(hb_req));
    auto hb_rsp = createHeartbeatResponse(msg_hdr_desc->is_payload_len_including_self);
    msg_hdr_desc->heartbeat_response = string((char*)&hb_rsp, sizeof(hb_rsp));
    return msg_hdr_desc;
}
