#include <stdio.h>
#include "switch_client.h"
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_console.h"

SwitchClient::SwitchClient(const char* host, uint16_t port, uint32_t client_id) :
    client_id_(client_id),
    client_(host, port, MessageType::CUSTOM)
{
    srand(time(0));
    client_id_ = rand() % 101;

    InitComponents();
    InitClient();
}

void SwitchClient::Cleanup()
{
    console_->Destory();
}

void SwitchClient::InitComponents()
{
    cmd_handler_ = new SCCommandHandler(this);
    console_ = new SCConsole(client_id_, cmd_handler_);
    console_->registerCommands();
}

void SwitchClient::InitClient()
{
    auto msg_hdr_desc = CreateMessageHeaderDescription();
    client_.SetMessageHeaderDescription(msg_hdr_desc);
    client_.SetAutoReconnect(false);

    auto client_cbs = std::make_shared<TcpCallbacks>();
    client_cbs->on_msg_recvd_cb = std::bind(&SwitchClient::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    client_cbs->on_closed_cb = std::bind(&SwitchClient::OnConnectionClosed, this, std::placeholders::_1);

    client_.SetNewClientCallback(std::bind(&SwitchClient::OnConnectionCreated, this, std::placeholders::_1));
    client_.SetTcpCallbacks(client_cbs);
    //client_.EnableKeepAlive(true);

    client_.Connect();
}

void SwitchClient::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[OnMessageRecvd] received message, fd: %d, payload: %s, length: %lu\n",
            conn->FD(), msg->Payload(), msg->PayloadSize());
    printf("[OnMessageRecvd] message size: %lu\n", msg->Size());
    printf("[OnMessageRecvd] message bytes:\n");
    cout << msg->DumpHex() << endl;

    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    cmdMsg->payload_len = ntohl(cmdMsg->payload_len);
    if (client_.GetMessageHeaderDescription()->is_payload_len_including_self) {
        cmdMsg->payload_len -= sizeof(cmdMsg->payload_len);
    }
    if (cmdMsg->HasResponseFlag()) {
        cmd_handler_->HandleCommandResult(conn, cmdMsg);
    } else {
        if (ECommand(cmdMsg->cmd) == ECommand::DATA) {
            printf("Received forwarding DATA message:\n");
            DumpHex(string(msg->Payload(), msg->PayloadSize()));
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
