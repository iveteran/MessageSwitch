#include <eventloop/el.h>

using namespace evt_loop;

class SCCommandHandler;
class SCConsole;

class SwitchClient {
public:
    SwitchClient(const char* host="localhost", uint16_t port=10000, uint32_t client_id=0);
    void Cleanup();
    uint32_t ID() const { return client_id_; }

    HeaderDescriptionPtr GetMessageHeaderDescription() const {
        return client_.GetMessageHeaderDescription();
    }
    TcpConnectionPtr Connection() { return client_.Connection(); }
    bool IsConnected() const { return client_.IsConnected(); }

protected:
    void InitComponents();
    void InitClient();

    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    void OnConnectionCreated(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);

    HeaderDescriptionPtr CreateMessageHeaderDescription();

private:
    uint32_t client_id_;
    TcpClient client_;

    SCCommandHandler* cmd_handler_;
    SCConsole* console_;
};
