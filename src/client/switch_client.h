#include <eventloop/el.h>
#include "switch_types.h"

using namespace evt_loop;

class SCOptions;
class SCContext;
class SCCommandHandler;
class SCConsole;

class SwitchClient {
public:
    SwitchClient(const char* host="localhost", uint16_t port=10000, EndpointId ep_id=0);
    SwitchClient(SCOptions* options);

    void Cleanup();
    void Exit();

    EndpointId ID() const { return endpoint_id_; }

    HeaderDescriptionPtr GetMessageHeaderDescription() const {
        return client_ ? client_->GetMessageHeaderDescription() : nullptr;
    }
    TcpConnectionPtr Connection() { return client_ ? client_->Connection() : nullptr; }
    bool IsConnected() const { return client_ && client_->IsConnected(); }
    bool Reconnect() { return client_->IsConnected() ? false : client_->Connect(); }
    void EnableHeartbeat() { client_->EnableHeartbeat(); }
    void DisableHeartbeat() { client_->DisableHeartbeat(); }
    bool IsHeartbeatEnabled() { return client_->IsHeartbeatEnabled(); }

    SCOptions* GetOptions() const { return options_; }
    SCContext* GetContext() const { return context_; }

protected:
    void InitComponents();
    void InitClient(const char* host, uint16_t port);

    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    void OnConnectionCreated(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);

    HeaderDescriptionPtr CreateMessageHeaderDescription();

private:
    TcpClient* client_;
    EndpointId endpoint_id_;

    SCOptions* options_;
    SCContext* context_;
    SCCommandHandler* cmd_handler_;
    SCConsole* console_;
};
