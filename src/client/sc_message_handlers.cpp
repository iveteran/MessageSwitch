#include "sc_message_handlers.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void SCMessageHandlers::SetupHandlers()
{
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

// for service mode
std::pair<int, string>
SCMessageHandlers::OnServiceCommand(const ServiceMessage* svc_msg, const char* body, size_t body_size)
{
    printf("SCMessageHandlers::OnServiceCommand, svc cmd: %d\n", svc_msg->svc_cmd);
    switch (svc_msg->svc_cmd) {
        case 1:
            {
                // Do something here, and respond result
                string rsp_data(R"({"svc_result": "service command: 1, TODO"})");
                return {0, rsp_data};
            }
        case 2:
            {
                // Do something here, and respond result
                string rsp_data(R"({"svc_result": "service command: 2, TODO"})");
                return {0, rsp_data};
            }
        default:
            break;
    }
    char errmsg[64];
    snprintf(errmsg, sizeof(errmsg), "Not implemented yet, service cmd: %d", svc_msg->svc_cmd);
    fprintf(stderr, "%s\n", errmsg);
    return {1, errmsg};
}
