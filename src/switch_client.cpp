#include <stdio.h>
#include <arpa/inet.h>
#include "switch_client.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

SwitchClient::SwitchClient(const char* host, uint16_t port) :
    client_id_(0),
    client_(host, port, MessageType::CUSTOM),
    sending_timer_(TimeVal(5, 0), std::bind(&SwitchClient::OnSendingTimer, this, std::placeholders::_1))
{
    srand(time(0));
    client_id_ = rand() % 101;
    auto msg_hdr_desc = CreateMessageHeaderDescription();
    client_.SetMessageHeaderDescription(msg_hdr_desc);
    client_.SetAutoReconnect(false);

    TcpCallbacksPtr client_cbs = std::shared_ptr<TcpCallbacks>(new TcpCallbacks);
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
    msg->DumpHex();

    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    cmdMsg->payload_len = ntohl(cmdMsg->payload_len);
    if (client_.GetMessageHeaderDescription()->is_payload_len_including_self) {
        cmdMsg->payload_len -= sizeof(cmdMsg->payload_len);
    }
    if (cmdMsg->HasResponseFlag()) {
        HandleCommandResult(conn, cmdMsg);
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

    const char* content = "echo";
    SendCommandMessage(client_.Connection().get(), ECommand::ECHO, content, strlen(content));
    printf("Sent ECHO message, content: %s\n", content);

    RegisterSelf();
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

void SwitchClient::RegisterSelf()
{
    json params;
    params["id"] = client_id_;
#if 1
    params["role"] = "endpoint";
    params["access_code"] = "Hello World";
#else
    params["role"] = "admin";
    params["admin_code"] = "Foobar2000";
    //params["admin_code"] = "my_admin_code";
#endif

    auto params_data = params.dump();
    SendCommandMessage(client_.Connection().get(), ECommand::REG, params_data);
    printf("Sent REG message, content: %s\n", params_data.c_str());
}

void SwitchClient::HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg)
{
    ECommand cmd = (ECommand)cmdMsg->cmd;
    printf("Command result:\n");
    printf("cmd: %s(%d)\n", CommandToTag(cmd), uint8_t(cmd));
    printf("payload_len: %d\n", cmdMsg->payload_len);

    ResultMessage* resultMsg = (ResultMessage*)(cmdMsg->payload);
    int8_t errcode = resultMsg->errcode;
    char* content = resultMsg->data;
    printf("errcode: %d\n", errcode);
    size_t content_len = cmdMsg->payload_len - sizeof(ResultMessage);
    printf("content len: %ld\n", content_len);
    if (errcode == 0) {
        printf("content: %s\n", content);
    } else {
        printf("error message: %s\n", content);
    }

    switch (cmd)
    {
        case ECommand::REG:
            sending_timer_.Start();
            break;
        case ECommand::INFO:
            // TODO
            break;
        default:
            break;
    }
}

size_t SwitchClient::SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& data)
{
    return SendCommandMessage(conn, cmd, data.data(), data.size());
}

size_t SwitchClient::SendCommandMessage(TcpConnection* conn, ECommand cmd,
        const char* data, size_t data_len)
{
    size_t sent_bytes = 0;

    CommandMessage cmdMsg;
    cmdMsg.cmd = uint8_t(cmd);
    uint32_t payload_len = data_len;
    if (client_.GetMessageHeaderDescription()->is_payload_len_including_self) {
        payload_len += sizeof(CommandMessage::payload_len);
    }
    cmdMsg.payload_len = htonl(payload_len);

    conn->Send((char*)&cmdMsg, sizeof(cmdMsg));
    sent_bytes += sizeof(cmdMsg);
    if (data && data_len > 0) {
        conn->Send(data, data_len);
        sent_bytes += data_len;
    }

    //string msg_bytes((char*)&cmdMsg, sizeof(cmdMsg));
    //msg_bytes.append(data);
    //size_t msg_length = sizeof(cmdMsg) + strlen(data);
    //printf("hdr size: %ld\n", sizeof(cmdMsg));
    //printf("msg size: %ld\n", msg_length);
    //printf("msg bytes:\n");
    //DumpHex(msg_bytes);

    return sent_bytes;
}

void SwitchClient::OnSendingTimer(TimerEvent* timer)
{
    if (! client_.IsConnected()) {
        printf("[OnSendingTimer] The connection was disconnected! Do nothing.\n");
        return;
    }
    printf("[OnSendingTimer] send data\n");
    ECommand cmds[] = {
        //ECommand::FWD,
        ECommand::DATA,
        //ECommand::SETUP,
        //ECommand::INFO,
        //ECommand::KICKOUT,
    };
    for (auto cmd : cmds) {
        switch (cmd) {
            case ECommand::DATA:
                {
                    char content[64] = {0};
                    snprintf(content, sizeof(content), "my data #%d", client_id_);
                    SendCommandMessage(client_.Connection().get(), ECommand::DATA, content, strlen(content));
                    printf("Sent DATA message, content: %s\n", content);
                }
                break;
            case ECommand::INFO:
                {
                    const char* content = R"({"details": true})";
                    SendCommandMessage(client_.Connection().get(), ECommand::INFO, content, strlen(content));
                    printf("Sent INFO message, content: %s\n", content);
                }
                break;
            case ECommand::FWD:
                {
                    const char* content = R"({"targets": [1, 2]})";
                    SendCommandMessage(client_.Connection().get(), ECommand::FWD, content, strlen(content));
                    printf("Sent FWD message, content: %s\n", content);
                }
                break;
            case ECommand::SETUP:
                {
                    //const char* content = R"({"new_admin_code": "my_admin_code", "admin_code": "Foobar2000"})";
                    //const char* content = R"({"new_access_code": "my_access_code"})";
                    const char* content = R"({"mode": "proxy"})";
                    SendCommandMessage(client_.Connection().get(), ECommand::SETUP, content, strlen(content));
                    printf("Sent SETUP message, content: %s\n", content);
                }
                break;
            case ECommand::KICKOUT:
                {
                    const char* content = R"({"targets": [95]})";
                    SendCommandMessage(client_.Connection().get(), ECommand::KICKOUT, content, strlen(content));
                    printf("Sent KICKOUT message, content: %s\n", content);
                }
                break;
            case ECommand::RELOAD:
                SendCommandMessage(client_.Connection().get(), ECommand::RELOAD);
                printf("Sent RELOAD message\n");
                break;
            default:
                break;
        }
    }
}
