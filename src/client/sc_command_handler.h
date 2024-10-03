#ifndef _SC_COMMAND_HANDLER_H
#define _SC_COMMAND_HANDLER_H

#include "switch_message.h"
#include "endpoint_role.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

class CommandMessage;
class SwitchClient;

class SCCommandHandler {
    public:
    SCCommandHandler(SwitchClient* client) : client_(client) {}

    void Echo(const char* content);
    void Register(uint32_t ep_id, EEndpointRole ep_role, const string& access_code, bool with_token=false, uint8_t svc_type=0);
    void GetInfo(bool is_details, uint32_t ep_id=0);
    void ForwardTargets(const vector<uint32_t>& targets);
    void UnforwardTargets(const vector<uint32_t>& targets);
    void Subscribe(const vector<uint32_t>& sources, const vector<uint8_t>& messages);
    void Unsubscribe(const vector<uint32_t>& sources, const vector<uint8_t>& messages);
    void Reject(const vector<uint32_t>& sources, const vector<uint8_t>& messages);
    void Unreject(const vector<uint32_t>& sources, const vector<uint8_t>& messages);
    void Publish(const string& data);
    void RequestService(const string& data, uint8_t svc_type);
    void Setup(const string& admin_code, const string& new_admin_code,
            const string& new_access_code, const string& mode);
    void Kickout(const vector<uint32_t>& targets);
    void Reload();

    void HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandlePublishData(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandleServiceRequest(TcpConnection* conn, CommandMessage* cmdMsg);

    private:
    size_t SendCommandMessage(ECommand cmd, const string& payload, uint8_t svc_type=0);
    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& payload, uint8_t svc_type=0);

    void HandleRegisterResult(CommandMessage* cmdMsg, const string& payload);
    void HandleGetInfoResult(CommandMessage* cmdMsg, const string& data);
    void HandleGetEndpointInfoResult(CommandMessage* cmdMsg, const string& data);

    template<typename T>
    void SubUnsubRejUnrej(ECommand cmd, const vector<uint32_t>& sources, const vector<uint8_t>& messages);

    private:
    SwitchClient* client_;
};

#endif // _SC_COMMAND_HANDLER_H
