#include <stdio.h>
#include <arpa/inet.h>
#include "switch_client.h"

SwitchClient::SwitchClient(const char* host, uint16_t port) :
    client_id_(0),
    client_(host, port, MessageType::CUSTOM),
    sending_timer_(TimeVal(5, 0), std::bind(&SwitchClient::OnSendingTimer, this, std::placeholders::_1))
{
    srand(time(0));
    client_id_ = rand() % 101;
    auto msg_hdr_desc = CreateMessageHeaderDescription();
    client_.SetMessageHeaderDescription(msg_hdr_desc);

    TcpCallbacksPtr client_cbs = std::shared_ptr<TcpCallbacks>(new TcpCallbacks);
    client_cbs->on_msg_recvd_cb = std::bind(&SwitchClient::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);

    client_.SetNewClientCallback(std::bind(&SwitchClient::OnConnectionCreated, this, std::placeholders::_1));
    client_.SetTcpCallbacks(client_cbs);
    //client_.EnableKeepAlive(true);

    client_.Connect();
}

void SwitchClient::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[OnMessageRecvd] received message, fd: %d, payload: %s, length: %lu\n", conn->FD(), msg->Payload(), msg->PayloadSize());
    printf("[OnMessageRecvd] message size: %lu\n", msg->Size());
    printf("[OnMessageRecvd] message bytes:\n");
    msg->DumpHex();

    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    cmdMsg->payload_len = ntohl(cmdMsg->payload_len);
    if (client_.GetMessageHeaderDescription()->is_payload_len_including_self) {
        cmdMsg->payload_len -= sizeof(cmdMsg->payload_len);
    }
    if (cmdMsg->cmd == ECommand::DATA) {
        printf("Received forwarding DATA message:\n");
        DumpHex(string(msg->Payload(), msg->PayloadSize()));
    } else {
        HandleCommandResult(conn, cmdMsg);
    }
}

void SwitchClient::OnConnectionCreated(TcpConnection* conn)
{
    printf("[OnConnectionCreated] connection created, fd: %d\n", conn->FD());
    printf("[OnConnectionCreated] echo\n");

    const char* content = "echo";
    SendCommandMessage(client_.Connection().get(), ECommand::ECHO, content, strlen(content));
    printf("Sent ECHO message, content: %s\n", content);

    sending_timer_.Start();
}

HeaderDescriptionPtr SwitchClient::CreateMessageHeaderDescription()
{
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = sizeof(CommandMessage);
    msg_hdr_desc->payload_len_offset = 1;
    msg_hdr_desc->payload_len_bytes = sizeof(CommandMessage::payload_len);
    msg_hdr_desc->is_payload_len_including_self = true;
    return msg_hdr_desc;
}

void SwitchClient::HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg)
{
    ECommand cmd = (ECommand)cmdMsg->cmd;
    printf("Command result:\n");
    printf("cmd: %d\n", cmd);
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
}

size_t SwitchClient::SendCommandMessage(TcpConnection* conn, ECommand cmd,
        const char* data, size_t data_len)
{
    size_t sent_bytes = 0;

    CommandMessage cmdMsg;
    cmdMsg.cmd = cmd;
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
    printf("[OnSendingTimer] send data\n");
    //const char* content = "my data";
    char content[64] = {0};
    snprintf(content, sizeof(content), "my data #%d", client_id_);
    SendCommandMessage(client_.Connection().get(), ECommand::DATA, content, strlen(content));
    printf("Sent DATA message, content: %s\n", content);
}
