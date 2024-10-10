#include <eventloop/el.h>
#include "switch_types.h"

using namespace evt_loop;

class CommandMessage;

using MessageCallback = std::function<void (TcpConnection*, CommandMessage*)>;
using ConnectedCallback = std::function<void ()>;
using ClosedCallback = std::function<void ()>;

class SCPeer {
public:
    SCPeer(const char* host="localhost", uint16_t port=10000);
    ~SCPeer();

    HeaderDescriptionPtr GetMessageHeaderDescription() const {
        return client_ ? client_->GetMessageHeaderDescription() : nullptr;
    }
    TcpConnectionPtr Connection() { return client_ ? client_->Connection() : nullptr; }
    bool IsConnected() const { return client_ && client_->IsConnected(); }
    bool Connect() { return Reconnect(); }
    bool Reconnect() { return client_->IsConnected() ? false : client_->Connect(); }
    void EnableHeartbeat() { client_->EnableHeartbeat(); }
    void DisableHeartbeat() { client_->DisableHeartbeat(); }
    bool IsHeartbeatEnabled() { return client_->IsHeartbeatEnabled(); }

    void SetConnectedCallback(const ConnectedCallback& cb);
    void SetClosedCallback(const ClosedCallback& cb);
    void SetMessageCallback(const MessageCallback& cb);

protected:
    void Init(const char* host, uint16_t port);

    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    void OnConnectionCreated(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);

    HeaderDescriptionPtr CreateMessageHeaderDescription();

private:
    TcpClient* client_;
    MessageCallback msg_cb_;
    ConnectedCallback connected_cb_;
    ClosedCallback closed_cb_;
};
