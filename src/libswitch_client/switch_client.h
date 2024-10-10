#include <eventloop/el.h>
#include "switch_types.h"

using namespace evt_loop;

class SCPeer;
class SCOptions;
class SCContext;
class SCCommandHandler;
class SCConsole;

class SwitchClient {
public:
    SwitchClient(const char* host="localhost", uint16_t port=10000,
            EndpointId ep_id=0, bool enable_console=false,
            const char* console_sub_prompt=nullptr);
    SwitchClient(SCOptions* options);

    void Cleanup();
    void Stop();
    void Start();

    EndpointId ID() const { return endpoint_id_; }

    TcpConnectionPtr Connection();
    bool IsConnected() const;
    bool Reconnect();
    void EnableHeartbeat();
    void DisableHeartbeat();
    bool IsHeartbeatEnabled();

    SCOptions* GetOptions() const { return options_; }
    SCContext* GetContext() const { return context_; }
    SCPeer* GetPeer() const { return peer_; }
    SCCommandHandler* GetCommandHandler() const { return cmd_handler_; }

protected:
    void CreateComponents();
    void RegisterSelf();
    void OnPeerConnected();
    void OnPeerClosed();

private:
    SCPeer* peer_ = nullptr;
    EndpointId endpoint_id_;
    bool enable_console_;

    SCOptions* options_ = nullptr;
    SCContext* context_ = nullptr;
    SCCommandHandler* cmd_handler_ = nullptr;
    SCConsole* console_ = nullptr;
    string console_sub_prompt_;
};
