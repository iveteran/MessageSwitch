#ifndef _DEMO_MESSAGE_HANDLERS_H
#define _DEMO_MESSAGE_HANDLERS_H

#include "switch_message.h"
#include "endpoint_role.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

class SCCommandHandler;
class CommandMessage;
class CommandResultRegister;
class PublishingMessage;
class ServiceMessage;

class DemoMessageHandlers {
public:
    DemoMessageHandlers(SCCommandHandler* cmd_handler)
        : cmd_handler_(cmd_handler)
    {}

    void Register(EndpointId ep_id, EEndpointRole ep_role, const string& access_code,
            bool with_token=false, ServiceType svc_type=0);
    void Publish(const string& data, const vector<EndpointId> targets={}, MessageId msg_type=0);
    void RequestService(const string& data, ServiceType svc_type, MessageId svc_cmd);

    void SetupHandlers();

protected:
    //void RegisterSelf();
    void OnCommandSuccess(ECommand cmd, const char* content, size_t content_len);
    void OnCommandFail(ECommand cmd, const char* content, size_t content_len);
    void OnRegisterResult(const CommandResultRegister* reg_result);
    void OnPublishingData(const PublishingMessage* pub_msg, const char* data, size_t data_len);
    void OnPublishingResult(const ResultMessage* result_msg, const char* data, size_t data_len);
    void OnServiceRequestResult(const ServiceMessage* svc_msg, const char* payload, size_t payload_size);
    std::pair<int, string> OnServiceCommand(const ServiceMessage* svc_msg, const char* data, size_t data_size);

private:
    SCCommandHandler* cmd_handler_;
};

#endif  // _DEMO_MESSAGE_HANDLERS_H
