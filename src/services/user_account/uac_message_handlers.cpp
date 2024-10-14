#include "uac_message_handlers.h"
#include "uac_command.h"
#include "uac_messages.h"
#include "uac_storer.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void UACMessageHandlers::SetupHandlers()
{
    // for service mode, handle service request (with service command)
    cmd_handler_->SetServiceRequestHandlerCallback(
            MOD_NAME,
            std::bind(
                &UACMessageHandlers::OnServiceCommand,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );
}

// for service mode
std::pair<int, string>
UACMessageHandlers::OnServiceCommand(const ServiceMessage* svc_msg, const char* body, size_t body_size)
{
    int errcode = -1;
    string rsp_data;
    printf("UACMessageHandlers::OnServiceCommand, svc cmd: %d\n", svc_msg->svc_cmd);
    switch ((UACCommand)svc_msg->svc_cmd) {
        case UACCommand::BASE_INFO:
            {
                UserBaseInfoReq req;
                req.DecodeFromJSON(body, body_size);
                auto user_bi = uac_storer_->GetUserBaseInfo(req.uid);
                if (user_bi) {
                    errcode = 0;
                    rsp_data = user_bi->EncodeToJSON();
                } else {
                    rsp_data = "Get user base info, the user does not exist";
                }
            }
            break;
        default:
            {
                char errmsg[64];
                snprintf(errmsg, sizeof(errmsg), "Unknown command: %d", svc_msg->svc_cmd);
                rsp_data = errmsg;
            }
            break;
    }
    if (errcode != 0) {
        fprintf(stderr, "[UACMessageHandlers::OnServiceCommand] error: %s\n", rsp_data.c_str());
    }
    return {errcode, rsp_data};
}
