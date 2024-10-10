#include <stdio.h>
#include "sc_peer.h"
#include "command_messages.h"
#include "switch_message.h"

SCPeer::SCPeer(const char* host, uint16_t port)
    : client_(nullptr)
{
    Init(host, port);
}
SCPeer::~SCPeer()
{
    delete client_;
}

void SCPeer::Init(const char* host, uint16_t port)
{
    //client_ = std::make_shared<TcpClient>(host, port, MessageType::CUSTOM);
    client_ = new TcpClient(host, port, MessageType::CUSTOM);

    auto msg_hdr_desc = CreateMessageHeaderDescription();
    client_->SetMessageHeaderDescription(msg_hdr_desc);
    client_->SetAutoReconnect(false);

    auto client_cbs = std::make_shared<TcpCallbacks>();
    client_cbs->on_msg_recvd_cb = std::bind(&SCPeer::OnMessageRecvd, this, std::placeholders::_1, std::placeholders::_2);
    client_cbs->on_closed_cb = std::bind(&SCPeer::OnConnectionClosed, this, std::placeholders::_1);

    client_->SetNewClientCallback(std::bind(&SCPeer::OnConnectionCreated, this, std::placeholders::_1));
    client_->SetTcpCallbacks(client_cbs);
    //client_->EnableKeepAlive(true);

    //client_->Connect();
}

void SCPeer::SetMessageCallback(const MessageCallback& cb)
{
    msg_cb_ = cb;
}
void SCPeer::SetConnectedCallback(const ConnectedCallback& cb)
{
    connected_cb_ = cb;
}
void SCPeer::SetClosedCallback(const ClosedCallback& cb)
{
    closed_cb_ = cb;
}

void SCPeer::OnMessageRecvd(TcpConnection* conn, const Message* msg)
{
    printf("[OnMessageRecvd] received message, fd: %d, id: %d, size: %lu, playload size: %lu\n",
            conn->FD(), conn->ID(), msg->Size(), msg->PayloadSize());
    printf("[OnMessageRecvd] message bytes:\n");
    cout << msg->DumpHexWithChars(evt_loop::DUMP_MAX_BYTES) << endl;
    //printf("[OnMessageRecvd] message payload bytes:\n");
    //cout << DumpHexWithChars(msg->Payload(), msg->PayloadSize(), evt_loop::DUMP_MAX_BYTES) << endl;

    auto cmdMsg = CommandMessage::FromNetworkMessage(msg,
            client_->GetMessageHeaderDescription()->is_payload_len_including_self);
    msg_cb_(conn, cmdMsg);
}

void SCPeer::OnConnectionCreated(TcpConnection* conn)
{
    printf("[OnConnectionCreated] connection created, fd: %d\n", conn->FD());
    if (connected_cb_) {
        connected_cb_();
    }
}
void SCPeer::OnConnectionClosed(TcpConnection* conn)
{
    printf("[SCPeer::OnConnectionClosed] fd: %d\n", conn->FD());
    if (closed_cb_) {
        closed_cb_();
    }
}

HeaderDescriptionPtr SCPeer::CreateMessageHeaderDescription()
{
    auto msg_hdr_desc = std::make_shared<HeaderDescription>();
    msg_hdr_desc->hdr_len = CommandMessage::HeaderSize();
    msg_hdr_desc->payload_len_offset = CommandMessage::OffsetOfPayloadLen();
    msg_hdr_desc->payload_len_bytes = CommandMessage::PayloadLenBytes();
    msg_hdr_desc->is_payload_len_including_self = true;
    auto hb_req = CommandMessage::CreateHeartbeatRequest();
    msg_hdr_desc->heartbeat_request.assign(hb_req.Data(), hb_req.Size());
    auto hb_rsp = CommandMessage::CreateHeartbeatResponse();
    msg_hdr_desc->heartbeat_response.assign(hb_rsp.Data(), hb_rsp.Size());
    return msg_hdr_desc;
}
