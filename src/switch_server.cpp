#include <stdio.h>
#include <stddef.h>
#include "switch_server.h"
#include "switch_command_handler.h"

SwitchServer::SwitchServer(const char* host, uint16_t port) :
    server_(nullptr), node_id_(0)
{
    init(host, port);
}

SwitchServer::SwitchServer(const OptionsPtr& options) :
    server_(nullptr), node_id_(0), options_(options)
{
    if (options->node_id > 0) {
        node_id_ = options->node_id;
    }
    init(options->host.c_str(), options->port);
}

void SwitchServer::OnSignal(SignalHandler* sh, uint32_t signo)
{
    printf("SwitchServer::Shutdown\n");
    EV_Singleton->StopLoop();
}

bool SwitchServer::init(const char* host, uint16_t port)
{
    context_ = std::make_shared<SwitchContext>(this);
    auto msg_hdr_desc = CreateMessageHeaderDescription();

    server_ = std::make_shared<TcpServer>(host, port, MessageType::CUSTOM);
    server_->SetMessageHeaderDescription(msg_hdr_desc);

    TcpCallbacksPtr svr_cbs = std::shared_ptr<TcpCallbacks>(new TcpCallbacks);
    svr_cbs->on_msg_recvd_cb = std::bind(&SwitchServer::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    svr_cbs->on_conn_ready_cb = std::bind(&SwitchServer::OnConnectionReady, this, std::placeholders::_1);
    svr_cbs->on_closed_cb = std::bind(&SwitchServer::OnConnectionClosed, this, std::placeholders::_1);
    server_->SetTcpCallbacks(svr_cbs);

    printf("Context: %s\n", context_->ToString().c_str());

    return true;
}

HeaderDescriptionPtr SwitchServer::CreateMessageHeaderDescription() {
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = sizeof(CommandMessage);
    //msg_hdr_desc->payload_len_offset = 2;  // jump over the size of fields cmd and flag
    msg_hdr_desc->payload_len_offset = offsetof(struct CommandMessage, payload_len);
    msg_hdr_desc->payload_len_bytes = sizeof(CommandMessage::payload_len);
    msg_hdr_desc->is_payload_len_including_self = true;
    return msg_hdr_desc;
}

void SwitchServer::OnConnectionReady(TcpConnection* conn)
{
    printf("[SwitchServer::OnConnectionReady] fd: %d\n", conn->FD());
    context_->pending_clients.insert(std::make_pair(conn->FD(), conn));
}
void SwitchServer::OnConnectionClosed(TcpConnection* conn)
{
    printf("[SwitchServer::OnConnectionClosed] fd: %d, id: %d\n", conn->FD(), conn->ID());
    context_->admin_clients.erase(conn->ID());
    context_->endpoints.erase(conn->ID());
    context_->pending_clients.erase(conn->FD());
}
void SwitchServer::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[SwitchServer::OnMessageRecvd] fd: %d, id: %d, payload: %s, length: %lu\n",
            conn->FD(), conn->ID(), msg->Payload(), msg->PayloadSize());
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
    printf("[SwitchServer::HandleCommand] id: %d\n", conn->ID());
    printf("[SwitchServer::HandleCommand] cmd: %s(%d)\n", CommandToTag(cmd), (uint8_t)cmd);
    printf("[SwitchServer::HandleCommand] payload len: %d\n", cmdMsg->payload_len);
    auto cmdHandler = std::make_shared<CommandHandler>(context_);

    switch (cmd)
    {
        case ECommand::ECHO:
            cmdHandler->handleEcho(conn, cmdMsg, msg->Data());
            return;
        case ECommand::REG:
            cmdHandler->handleRegister(conn, cmdMsg, msg->Data());
            return;
        default:
            break;
    }

    auto iter = context_->endpoints.find(conn->ID());
    if (iter == context_->endpoints.end()) {
        fprintf(stderr, "[SwitchServer::HandleCommand] Error: the connection(id: %d) can not to match any endpoint, "
                "maybe the connection not be registered or occurred errors for endpoint manager\n", conn->ID());
        conn->Disconnect();
        return;
    }
    auto ep = iter->second;

    switch (cmd)
    {
        case ECommand::FWD:
            cmdHandler->handleForward(ep, cmdMsg, msg->Data());
            break;
        case ECommand::DATA:
            cmdHandler->handleData(ep, cmdMsg, msg->Data());
            break;
        case ECommand::INFO:
            cmdHandler->handleInfo(ep, cmdMsg, msg->Data());
            break;
        case ECommand::SETUP:
            cmdHandler->handleSetup(ep, cmdMsg, msg->Data());
            break;
        case ECommand::PROXY:
            cmdHandler->handleProxy(ep, cmdMsg, msg->Data());
            break;
        case ECommand::KICKOUT:
            cmdHandler->handleKickout(ep, cmdMsg, msg->Data());
            break;
        case ECommand::RELOAD:
            cmdHandler->handleReload(ep, cmdMsg, msg->Data());
            break;
        default:
            fprintf(stderr, "[SwitchServer::HandleCommand] Error: Unsupported command: %s(%d)\n", CommandToTag(cmd), (uint8_t)cmd);
            break;
    }
}
