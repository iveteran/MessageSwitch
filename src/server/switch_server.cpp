#include <stdio.h>
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
    Exit();
}

void SwitchServer::Exit()
{
    printf("SwitchServer::Shutdown\n");
    // XXX: MUST call destory of Console manually, otherwise the terminal will be silently always
    console_->Destory();
    EV_Singleton->StopLoop();
}

bool SwitchServer::init(const char* host, uint16_t port)
{
    InitServer(host, port);
    InitComponents();

    return true;
}

void SwitchServer::InitComponents()
{
    context_ = std::make_shared<SwitchContext>(this);

    printf("Context: %s\n", context_->ToString().c_str());

    service_ = std::make_shared<SwitchService>(this);
    cmd_handler_ = std::make_shared<CommandHandler>(context_, service_);

    console_ = std::make_shared<SwitchConsole>(this);
    console_->registerCommands();
}

void SwitchServer::InitServer(const char* host, uint16_t port)
{
    auto msg_hdr_desc = CreateMessageHeaderDescription();

    server_ = std::make_shared<TcpServer>(host, port, MessageType::CUSTOM);
    server_->SetMessageHeaderDescription(msg_hdr_desc);

    auto svr_cbs = std::make_shared<TcpCallbacks>();
    svr_cbs->on_msg_recvd_cb = std::bind(&SwitchServer::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    svr_cbs->on_conn_ready_cb = std::bind(&SwitchServer::OnConnectionReady, this, std::placeholders::_1);
    svr_cbs->on_closed_cb = std::bind(&SwitchServer::OnConnectionClosed, this, std::placeholders::_1);
    server_->SetTcpCallbacks(svr_cbs);
}

HeaderDescriptionPtr SwitchServer::CreateMessageHeaderDescription() {
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = CommandMessage::HeaderSize();
    //msg_hdr_desc->payload_len_offset = 2;  // jump over the size of fields cmd and flag
    msg_hdr_desc->payload_len_offset = CommandMessage::OffsetOfPayloadLen();
    msg_hdr_desc->payload_len_bytes = CommandMessage::PayloadLenBytes();
    msg_hdr_desc->is_payload_len_including_self = true;
    auto hb_req = CommandMessage::CreateHeartbeatRequest();
    msg_hdr_desc->heartbeat_request.assign(hb_req.Data(), hb_req.Size());
    auto hb_rsp = CommandMessage::CreateHeartbeatResponse();
    msg_hdr_desc->heartbeat_response.assign(hb_rsp.Data(), hb_rsp.Size());
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
    // clear endpoint
    context_->RemoveEndpoint(conn->ID());
    context_->pending_clients.erase(conn->FD());
}
void SwitchServer::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[SwitchServer::OnMessageRecvd] fd: %d, id: %d, size: %lu\n", conn->FD(), conn->ID(), msg->Size());
    printf("[SwitchServer::OnMessageRecvd] message bytes(%lu):\n", msg->Size());
    cout << msg->DumpHexWithChars(evt_loop::DUMP_MAX_BYTES) << endl;

    cmd_handler_->handleCommand(conn, msg);
}
