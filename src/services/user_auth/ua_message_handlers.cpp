#include "ua_message_handlers.h"
#include "ua_command.h"
#include "ua_messages.h"
#include "ua_storer.h"
#include "command_messages.h"
#include "sc_command_handler.h"

static const char* MOD_NAME = "msg_handler";

void UAMessageHandlers::SetupHandlers()
{
    // for service mode, handle service request (with service command)
    cmd_handler_->SetServiceRequestHandlerCallback(
            MOD_NAME,
            std::bind(
                &UAMessageHandlers::OnServiceCommand,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3)
            );
}

// for service mode
std::pair<int, string>
UAMessageHandlers::OnServiceCommand(const ServiceMessage* svc_msg, const char* body, size_t body_size)
{
    int errcode = -1;
    string rsp_data;
    printf("UAMessageHandlers::OnServiceCommand, svc cmd: %d\n", svc_msg->svc_cmd);
    switch ((UACommand)svc_msg->svc_cmd) {
        case UACommand::LOGIN:
            {
                Login login;
                login.DecodeFromJSON(body, body_size);
                bool success = ua_storer_->VerifyUser(login.username, login.password);
                if (success) {
                    auto user_info = ua_storer_->GetUser(login.username);
                    auto sess = ua_storer_->CreateSession(user_info->uid);
                    errcode = 0;
                    LoginResult login_res;
                    login_res.uid = user_info->uid;
                    login_res.session_id = sess->session_id;
                    rsp_data = login_res.EncodeToJSON();
                } else {
                    rsp_data = "Login, authentication failed";
                }
            }
            break;
        case UACommand::LOGOUT:
            {
                Logout logout;
                logout.DecodeFromJSON(body, body_size);
                bool success = ua_storer_->VerifySession(logout.uid, logout.session_id);
                if (success) {
                    errcode = 0;
                    ua_storer_->RemoveSession(logout.uid);
                } else {
                    rsp_data = "Logout, Aathentication failed";
                }
            }
            break;
        case UACommand::VERIFY:
            {
                VerifyUser verify_user;
                verify_user.DecodeFromJSON(body, body_size);
                bool success = ua_storer_->VerifySession(verify_user.uid, verify_user.session_id);
                if (success) {
                    errcode = 0;
                } else {
                    rsp_data = "Verifying session, authentication failed";
                }
            }
            break;
        case UACommand::CREATE:
            {
                CreateUser user;
                user.DecodeFromJSON(body, body_size);
                bool exists = ua_storer_->IsUserExists(user.username);
                if (! exists) {
                    auto user_info = ua_storer_->CreateUser(user.username, user.password);
                    auto sess = ua_storer_->CreateSession(user_info->uid);
                    errcode = 0;
                    LoginResult login_res;
                    login_res.uid = user_info->uid;
                    login_res.session_id = sess->session_id;
                    rsp_data = login_res.EncodeToJSON();
                } else {
                    rsp_data = "Creating user, the username already exists";
                }
            }
            break;
        case UACommand::DELETE:
            {
                RemoveUser rm_user;
                rm_user.DecodeFromJSON(body, body_size);
                bool success = ua_storer_->VerifySession(rm_user.uid, rm_user.session_id);
                if (success) {
                    ua_storer_->RemoveUser(rm_user.uid);
                    errcode = 0;
                } else {
                    rsp_data = "Deleting user, authentication failed";
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
        fprintf(stderr, "[UAMessageHandlers::OnServiceCommand] error: %s\n", rsp_data.c_str());
    }
    return {errcode, rsp_data};
}
