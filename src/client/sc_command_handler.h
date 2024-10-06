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
    void Register(EndpointId ep_id, EEndpointRole ep_role, const string& access_code, bool with_token=false, ServiceType svc_type=0);
    void GetInfo(bool is_details, EndpointId ep_id=0);
    void ForwardTargets(const vector<EndpointId>& targets);
    void UnforwardTargets(const vector<EndpointId>& targets);
    void Subscribe(const vector<EndpointId>& sources, const vector<MessageId>& messages);
    void Unsubscribe(const vector<EndpointId>& sources, const vector<MessageId>& messages);
    void Reject(const vector<EndpointId>& sources, const vector<MessageId>& messages);
    void Unreject(const vector<EndpointId>& sources, const vector<MessageId>& messages);
    void Publish(const string& data, const vector<EndpointId> targets={}, MessageId msg_type=0);
    void RequestService(const string& data, ServiceType svc_type, MessageId svc_cmd, uint32_t sess_id=0);
    void Setup(const string& admin_code, const string& new_admin_code,
            const string& new_access_code, const string& mode);
    void Kickout(const vector<EndpointId>& targets);
    void Reload();

    void HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandlePublishData(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandleServiceRequest(TcpConnection* conn, CommandMessage* cmdMsg);

    private:
    size_t SendCommandMessage(ECommand cmd, const string& payload, const string& hdr_ext="");
    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& payload, const string& hdr_ext="");

    void HandleRegisterResult(CommandMessage* cmdMsg, const string& payload);
    void HandleGetInfoResult(CommandMessage* cmdMsg, const string& data);
    void HandleGetEndpointInfoResult(CommandMessage* cmdMsg, const string& data);
    void HandleServiceResult(CommandMessage* cmdMsg, const string& content);

    template<typename T>
    void SubUnsubRejUnrej(ECommand cmd, const vector<EndpointId>& sources, const vector<MessageId>& messages);

    private:
    SwitchClient* client_;
};

#endif // _SC_COMMAND_HANDLER_H
