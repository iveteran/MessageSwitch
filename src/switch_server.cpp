#include <stdio.h>
#include "switch_server.h"
#include "switch_command_handler.h"

SwitchServer::SwitchServer(const char* host, uint16_t port) :
  node_id_(0), server_(host, port, MessageType::CUSTOM)
{
    context_ = std::make_shared<SwitchContext>(this);
    auto msg_hdr_desc = CreateMessageHeaderDescription();
    server_.SetMessageHeaderDescription(msg_hdr_desc);

    TcpCallbacksPtr svr_cbs = std::shared_ptr<TcpCallbacks>(new TcpCallbacks);
    svr_cbs->on_msg_recvd_cb = std::bind(&SwitchServer::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    svr_cbs->on_conn_ready_cb = std::bind(&SwitchServer::OnConnectionReady, this, std::placeholders::_1);
    svr_cbs->on_closed_cb = std::bind(&SwitchServer::OnConnectionClosed, this, std::placeholders::_1);
    server_.SetTcpCallbacks(svr_cbs);
}

HeaderDescriptionPtr SwitchServer::CreateMessageHeaderDescription() {
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = sizeof(CommandMessage);
    msg_hdr_desc->payload_len_offset = 1;  // jump over the size of cmd field
    msg_hdr_desc->payload_len_bytes = sizeof(CommandMessage::payload_len);
    msg_hdr_desc->is_payload_len_including_self = true;
    return msg_hdr_desc;
}

void SwitchServer::OnConnectionReady(TcpConnection* conn)
{
    printf("[SwitchServer::OnConnectionReady] fd: %d\n", conn->FD());
    auto ep = std::make_shared<Endpoint>(conn->FD(), conn);
    context_->endpoints.insert(std::make_pair(conn->FD(), ep));
}
void SwitchServer::OnConnectionClosed(TcpConnection* conn)
{
    printf("[SwitchServer::OnConnectionClosed] fd: %d\n", conn->FD());
    context_->endpoints.erase(conn->FD());
}
void SwitchServer::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[SwitchServer::OnMessageRecvd] fd: %d, payload: %s, length: %lu\n", conn->FD(), msg->Payload(), msg->PayloadSize());
    printf("[SwitchServer::OnMessageRecvd] message size: %lu\n", msg->Size());
    printf("[SwitchServer::OnMessageRecvd] message bytes:\n");
    msg->DumpHex();

    HandleCommand(conn, msg);
}

void SwitchServer::HandleCommand(TcpConnection* conn, const Message* msg)
{
    CommandMessage* cmdMsg = (CommandMessage*)(msg->Data().data());
    cmdMsg->payload_len = ntohl(cmdMsg->payload_len);
    if (IsMessagePayloadLengthIncludingSelf()) {
        cmdMsg->payload_len -= sizeof(cmdMsg->payload_len);
    }
    ECommand cmd = (ECommand)cmdMsg->cmd;

    auto iter = context_->endpoints.find(conn->FD());
    if (iter == context_->endpoints.end()) {
        fprintf(stderr, "Error: the connection can not to match any endpoint, maybe occurred errors for endpoint managing");
        return;
    }
    auto ep = iter->second;

    auto cmdHandler = std::make_shared<CommandHandler>(context_);
    switch (cmd)
    {
        case ECommand::ECHO:
            cmdHandler->handleEcho(ep, cmdMsg, msg->Data());
            break;
        case ECommand::REG:
            cmdHandler->handleRegister(ep, cmdMsg, msg->Data());
            break;
        case ECommand::FWD:
            cmdHandler->handleForward(ep, cmdMsg, msg->Data());
            break;
        case ECommand::DATA:
            cmdHandler->handleData(ep, cmdMsg, msg->Data());
            break;
        default:
            break;
    }
}
