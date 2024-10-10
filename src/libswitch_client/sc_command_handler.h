#ifndef _SC_COMMAND_HANDLER_H
#define _SC_COMMAND_HANDLER_H

#include "switch_message.h"
#include "endpoint_role.h"

#include <string>
#include <vector>
#include <functional>
using std::string;
using std::vector;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

class CommandMessage;
class CommandInfo;
class CommandEndpointInfo;
class CommandResultRegister;
class PublishingMessage;
class ServiceMessage;

class SwitchClient;

using CommandSuccessHandlerCallback = std::function<void (ECommand, const char*, size_t)>;
using CommandFailHandlerCallback = std::function<void (ECommand, const char*, size_t)>;

using RegisterResultHandlerCallback = std::function<void (const CommandResultRegister*)>;
using InfoResultHandlerCallback = std::function<void (const CommandInfo*)>;
using EndpointInfoResultHandlerCallback = std::function<void (const CommandEndpointInfo*)>;

using PublishingDataHandlerCallback = std::function<void (const PublishingMessage*, const char*, size_t)>;
using PublishingResultHandlerCallback = std::function<void (const ResultMessage*, const char*, size_t)>;

using ServiceRequestHandlerCallback = std::function<std::pair<int, string> (const ServiceMessage*, const char*, size_t)>;
using ServiceRequestResultHandlerCallback = std::function<void (const ServiceMessage*, const char*, size_t)>;

class SCCommandHandler {
    public:
    SCCommandHandler(SwitchClient* client, bool is_payload_len_including_self)
        : client_(client), is_payload_len_including_self_(is_payload_len_including_self)
    {}

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

    void HandleCommandMessage(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandleCommandResult(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandlePublishData(TcpConnection* conn, CommandMessage* cmdMsg);
    void HandleServiceRequest(TcpConnection* conn, CommandMessage* cmdMsg);

    void SetCommandSuccessHandlerCallback(const CommandSuccessHandlerCallback& cb) {
        cmd_success_handler_cb_ = cb;
    }
    void SetCommandFailHandlerCallback(const CommandFailHandlerCallback& cb) {
        cmd_fail_handler_cb_ = cb;
    }
    void SetRegisterResultHandlerCallback(const RegisterResultHandlerCallback& cb) {
        reg_result_handler_cb_ = cb;
    }
    void SetInfoResultHandlerCallback(const InfoResultHandlerCallback& cb) {
        info_result_handler_cb_ = cb;
    }
    void SetEndpointInfoResultHandlerCallback(const EndpointInfoResultHandlerCallback& cb) {
        ep_info_result_handler_cb_ = cb;
    }
    void SetPublishingDataHandlerCallback(const PublishingDataHandlerCallback& cb) {
        pub_data_handler_cb_ = cb;
    }
    void SetPublishingResultHandlerCallback(const PublishingResultHandlerCallback& cb) {
        pub_result_handler_cb_ = cb;
    }
    void SetServiceRequestHandlerCallback(const ServiceRequestHandlerCallback& cb) {
        svc_req_handler_cb_ = cb;
    }
    void SetServiceRequestResultHandlerCallback(const ServiceRequestResultHandlerCallback& cb) {
        svc_req_result_handler_cb_ = cb;
    }

    private:
    size_t SendCommandMessage(ECommand cmd, const string& payload, const string& hdr_ext="");
    size_t SendCommandMessage(TcpConnection* conn, ECommand cmd, const string& payload, const string& hdr_ext="");

    void HandleRegisterResult(CommandMessage* cmdMsg, const string& payload);
    void HandleGetInfoResult(CommandMessage* cmdMsg, const string& data);
    void HandleGetEndpointInfoResult(CommandMessage* cmdMsg, const string& data);

    void HandlePublishingResult(CommandMessage* cmdMsg);
    void HandleServiceResult(CommandMessage* cmdMsg);

    template<typename T>
    void SubUnsubRejUnrej(ECommand cmd, const vector<EndpointId>& sources, const vector<MessageId>& messages);

    private:
    SwitchClient* client_;
    bool is_payload_len_including_self_;

    CommandSuccessHandlerCallback       cmd_success_handler_cb_;
    CommandFailHandlerCallback          cmd_fail_handler_cb_;

    RegisterResultHandlerCallback       reg_result_handler_cb_;
    InfoResultHandlerCallback           info_result_handler_cb_;
    EndpointInfoResultHandlerCallback   ep_info_result_handler_cb_;

    PublishingDataHandlerCallback       pub_data_handler_cb_;
    PublishingResultHandlerCallback     pub_result_handler_cb_;

    ServiceRequestHandlerCallback       svc_req_handler_cb_;
    ServiceRequestResultHandlerCallback svc_req_result_handler_cb_;
};

#endif // _SC_COMMAND_HANDLER_H
