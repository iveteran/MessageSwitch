#include "demo_message_handlers.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void DemoMessageHandlers::Register(EndpointId ep_id, EEndpointRole ep_role, const string& access_code,
        bool with_token, ServiceType svc_type)
{
    cmd_handler_->Register(ep_id, ep_role, access_code, with_token, svc_type);
}

void DemoMessageHandlers::Publish(const string& data, const vector<EndpointId> targets, MessageId msg_type)
{
    cmd_handler_->Publish(data, targets, msg_type);
}

void DemoMessageHandlers::RequestService(const string& data, ServiceType svc_type, MessageId svc_cmd)
{
    cmd_handler_->RequestService(data, svc_type, svc_cmd);
}

void DemoMessageHandlers::SetupHandlers()
{
    // handle successful result of requests 
    cmd_handler_->SetCommandSuccessHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnCommandSuccess,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle failed result of requests 
    cmd_handler_->SetCommandFailHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnCommandFail,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of registering self
    cmd_handler_->SetRegisterResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnRegisterResult,
                this,
                std::placeholders::_1)
            );

    // handle received data from published of other endpoints
    cmd_handler_->SetPublishingDataHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnPublishingData,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of publishing data by self
    cmd_handler_->SetPublishingResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnPublishingResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of service request by self
    cmd_handler_->SetServiceRequestResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnServiceRequestResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // for service mode, handle service request (with service command)
    cmd_handler_->SetServiceRequestHandlerCallback(
            MOD_NAME,
            std::bind(
                &DemoMessageHandlers::OnServiceCommand,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );
}

void DemoMessageHandlers::OnCommandSuccess(ECommand cmd, const char* content, size_t content_len)
{
    printf("DemoMessageHandlers::OnCommandSuccess, cmd: %d\n", (uint8_t)cmd);
    // Do something here
}

void DemoMessageHandlers::OnCommandFail(ECommand cmd, const char* content, size_t content_len)
{
    printf("DemoMessageHandlers::OnCommandFail, cmd: %d\n", (uint8_t)cmd);
    // Do something here
}

void DemoMessageHandlers::OnRegisterResult(const CommandResultRegister* reg_result)
{
    printf("DemoMessageHandlers::OnRegisterResult, ep_id: %d\n", reg_result->id);
    // Do something here
}

// handle received data from published of other endpoints
void DemoMessageHandlers::OnPublishingData(const PublishingMessage* pub_msg, const char* data, size_t data_len)
{
    if (pub_msg) {
        printf("DemoMessageHandlers::OnPublishingData, Received data from endpoint: %d\n", pub_msg->source);
    }
    printf("DemoMessageHandlers::OnPublishingData, data len: %ld\n", data_len);
    // Do something here
}

void DemoMessageHandlers::OnPublishingResult(const ResultMessage* result_msg, const char* data, size_t data_len)
{
    printf("DemoMessageHandlers::OnPublishingResult, data len: %ld\n", data_len);
    // Do something here
}

void DemoMessageHandlers::OnServiceRequestResult(const ServiceMessage* svc_msg, const char* payload, size_t payload_size)
{
    printf("DemoMessageHandlers::DemoMessageHandlers, Received SVC response message:\n");
    printf("DemoMessageHandlers::DemoMessageHandlers, svc cmd: %d\n", svc_msg->svc_cmd);
    // Do something here
}

// for service mode
std::pair<int, string>
DemoMessageHandlers::OnServiceCommand(const ServiceMessage* svc_msg, const char* body, size_t body_size)
{
    printf("DemoMessageHandlers::OnServiceCommand, svc cmd: %d\n", svc_msg->svc_cmd);
    switch (svc_msg->svc_cmd) {
        case 1:
            {
                // Do something here, and respond result
                string rsp_data(R"({"svc_result": "server command: 1, TODO"})");
                return {0, rsp_data};
            }
        case 2:
            {
                // Do something here, and respond result
                string rsp_data(R"({"svc_result": "server command: 2, TODO"})");
                return {0, rsp_data};
            }
        default:
            break;
    }
    char errmsg[64];
    snprintf(errmsg, sizeof(errmsg), "Not implemented yet, cmd: %d", svc_msg->svc_cmd);
    fprintf(stderr, "%s\n", errmsg);
    return {1, errmsg};
}
