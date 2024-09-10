#include "switch_message.h"
#include "el.h"

using namespace evt_loop;

class SwitchClient {
public:
    SwitchClient(const char* host="localhost", uint16_t port=10000);

protected:
    void OnMessageRecvd(TcpConnection* conn, const Message* msg);
    void OnConnectionCreated(TcpConnection* conn);
    void OnConnectionClosed(TcpConnection* conn);

    HeaderDescriptionPtr CreateMessageHeaderDescription();

    void RegisterSelf();
    void HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg);

    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& data);
    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd,
            const char* data = NULL, size_t data_len = 0);
    void OnSendingTimer(TimerEvent* timer);

private:
    uint32_t client_id_;
    TcpClient client_;
    PeriodicTimer sending_timer_;
};
