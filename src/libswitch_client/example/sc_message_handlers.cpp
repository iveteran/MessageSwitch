#include "sc_message_handlers.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void SCMessageHandlers::Register(EndpointId ep_id, EEndpointRole ep_role, const string& access_code,
        bool with_token, ServiceType svc_type)
{
    cmd_handler_->Register(ep_id, ep_role, access_code, with_token, svc_type);
}

void SCMessageHandlers::Publish(const string& data, const vector<EndpointId> targets, MessageId msg_type)
{
    cmd_handler_->Publish(data, targets, msg_type);
}

void SCMessageHandlers::RequestService(const string& data, ServiceType svc_type, MessageId svc_cmd)
{
    cmd_handler_->RequestService(data, svc_type, svc_cmd);
}

void SCMessageHandlers::SetupHandlers()
{
    // handle successful result of requests 
    cmd_handler_->SetCommandSuccessHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnCommandSuccess,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle failed result of requests 
    cmd_handler_->SetCommandFailHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnCommandFail,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of registering self
    cmd_handler_->SetRegisterResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnRegisterResult,
                this,
                std::placeholders::_1)
            );

    // handle received data from published of other endpoints
    cmd_handler_->SetPublishingDataHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnPublishingData,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of publishing data by self
    cmd_handler_->SetPublishingResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnPublishingResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // handle response of service request by self
    cmd_handler_->SetServiceRequestResultHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnServiceRequestResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );

    // for service mode, handle service request (with service command)
    cmd_handler_->SetServiceRequestHandlerCallback(
            MOD_NAME,
            std::bind(
                &SCMessageHandlers::OnServiceCommand,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );
}

void SCMessageHandlers::OnCommandSuccess(ECommand cmd, const char* content, size_t content_len)
{
    printf("SCMessageHandlers::OnCommandSuccess, cmd: %d\n", (uint8_t)cmd);
    // Do something here
}

void SCMessageHandlers::OnCommandFail(ECommand cmd, const char* content, size_t content_len)
{
    printf("SCMessageHandlers::OnCommandFail, cmd: %d\n", (uint8_t)cmd);
    // Do something here
}

void SCMessageHandlers::OnRegisterResult(const CommandResultRegister* reg_result)
{
    printf("SCMessageHandlers::OnRegisterResult, ep_id: %d\n", reg_result->id);
    // Do something here
}

// handle received data from published of other endpoints
void SCMessageHandlers::OnPublishingData(const PublishingMessage* pub_msg, const char* data, size_t data_len)
{
    if (pub_msg) {
        printf("SCMessageHandlers::OnPublishingData, Received data from endpoint: %d\n", pub_msg->source);
    }
    printf("SCMessageHandlers::OnPublishingData, data len: %ld\n", data_len);
    // Do something here
}

void SCMessageHandlers::OnPublishingResult(const ResultMessage* result_msg, const char* data, size_t data_len)
{
    printf("SCMessageHandlers::OnPublishingResult, data len: %ld\n", data_len);
    // Do something here
}

void SCMessageHandlers::OnServiceRequestResult(const ServiceMessage* svc_msg, const char* payload, size_t payload_size)
{
    printf("SCMessageHandlers::SCMessageHandlers, Received SVC response message:\n");
    printf("SCMessageHandlers::SCMessageHandlers, svc cmd: %d\n", svc_msg->svc_cmd);
    // Do something here
}

// for service mode
std::pair<int, string>
SCMessageHandlers::OnServiceCommand(const ServiceMessage* svc_msg, const char* body, size_t body_size)
{
    printf("SCMessageHandlers::OnServiceCommand, svc cmd: %d\n", svc_msg->svc_cmd);
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
