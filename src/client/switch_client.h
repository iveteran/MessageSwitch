#include <eventloop/el.h>

using namespace evt_loop;

class SCOptions;
class SCCommandHandler;
class SCConsole;

class SwitchClient {
public:
    SwitchClient(const char* host="localhost", uint16_t port=10000, uint32_t ep_id=0);
    SwitchClient(SCOptions* options);

    void Cleanup();
    void Exit();

    uint32_t ID() const { return endpoint_id_; }

    HeaderDescriptionPtr GetMessageHeaderDescription() const {
        return client_ ? client_->GetMessageHeaderDescription() : nullptr;
    }
    TcpConnectionPtr Connection() { return client_ ? client_->Connection() : nullptr; }
    bool IsConnected() const { return client_ && client_->IsConnected(); }

    SCOptions* GetOptions() const { return options_; }

protected:
    void InitComponents();
    void InitClient(const char* host, uint16_t port);

    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    void OnConnectionCreated(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);

    HeaderDescriptionPtr CreateMessageHeaderDescription();

private:
    TcpClient* client_;
    uint32_t endpoint_id_;

    SCOptions* options_;
    SCCommandHandler* cmd_handler_;
    SCConsole* console_;
};
