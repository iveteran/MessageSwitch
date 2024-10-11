#include "mw_message_handlers.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void MWMessageHandlers::SetupHandlers()
{
    // handle received data from published of other endpoints
    cmd_handler_->SetPublishingDataHandlerCallback(
            MOD_NAME,
            std::bind(
                &MWMessageHandlers::OnPublishingData,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );
}

// handle received data from published of other endpoints
void MWMessageHandlers::OnPublishingData(const PublishingMessage* pub_msg, const char* data, size_t data_len)
{
    if (pub_msg) {
        printf("MWMessageHandlers::OnPublishingData, Received data from endpoint: %d\n", pub_msg->source);
    }
    printf("MWMessageHandlers::OnPublishingData, data len: %ld\n", data_len);
    // Do something here
}
