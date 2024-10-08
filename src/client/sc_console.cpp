#include "sc_console.h"
#include <stdio.h>
#include <functional>
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_options.h"
#include "sc_context.h"
#include "switch_client.h"
#include "utils/time.h"
#include <eventloop/extensions/console.h>
#include <eventloop/extensions/aio_wrapper.h>
#include <argparse/argparse.hpp>
#include <signal.h>

#define PUT_LINE Console::Instance()->put_line
#define REGISTER_COMMAND Console::Instance()->registerCommand

using namespace evt_loop;

template<typename T>
void duplicate(vector<T>& v) {
    set<T> s(v.begin(), v.end());
    v.assign(s.begin(), s.end());
}

SCConsole::SCConsole(SwitchClient* client, SCCommandHandler* cmd_handler) :
    client_(client), cmd_handler_(cmd_handler)
{
    const char* prompt = "SC> ";
    Console::Initialize(prompt);

    cmd_handler_->SetCommandSuccessHandlerCallback(
            std::bind(
                &SCConsole::onCommandSuccess,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
                ));
    cmd_handler_->SetCommandFailHandlerCallback(
            std::bind(
                &SCConsole::onCommandFail,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
                ));
}

void SCConsole::Destory()
{
    Console::Instance()->destory();
}

void SCConsole::registerCommands()
{
    REGISTER_COMMAND(
            "exit",
            "Exit Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Exit, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "reconnect",
            "Reconnect to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reconnect, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "heartbeat",
            "Enable or disable connection heartbeat to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Heartbeat, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "options",
            "Show options of Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Options, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "status",
            "Show status of Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Status, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_echo",
            "Send echo command to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Echo, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_register",
            "Register Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Register, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_info",
            "Get information of Switch",
            std::bind(&SCConsole::handleConsoleCommand_GetInfo, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_publish",
            "Simulate to send custom data to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Publish, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_req_svc",
            "Request service from Switch server",
            std::bind(&SCConsole::handleConsoleCommand_RequestService, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_fwd",
            "Send forwarding targets command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_ForwardTargets, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_unfwd",
            "Send unforwarding targets command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_UnforwardTargets, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_sub",
            "Send subscribe command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Subscribe, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_unsub",
            "Send unsubscribe command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Unsubscribe, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_reject",
            "Send reject command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reject, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_unrej",
            "Send unreject command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Unreject, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_setup",
            "Send setup command to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Setup, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_kickout",
            "Send kickout comamnd to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Kickout, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "ss_reload",
            "Send reload command to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reload, this, std::placeholders::_1)
            );
}

int SCConsole::handleConsoleCommand_Exit(const vector<string>& argv)
{
    //raise(SIGINT);  // send self an INT signal
    client_->Exit();
    return 0;
}

int SCConsole::handleConsoleCommand_Reconnect(const vector<string>& argv)
{
    if (! client_->IsConnected()) {
        client_->Reconnect();
    } else {
        PUT_LINE("reconnect: ", "the connection already connected, do nothing.");
    }
    return 0;
}

int SCConsole::handleConsoleCommand_Heartbeat(const vector<string>& argv)
{
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--on")
        .help("Enable heartbeat to connection")
        .flag();
    cmd_ap.add_argument("--off")
        .help("Disable heartbeat ot connection")
        .flag();

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    if (cmd_ap.is_used("--on")) {
        client_->EnableHeartbeat();
    } else if (cmd_ap.is_used("--off")) {
        client_->DisableHeartbeat();
    } else {
        PUT_LINE("Status: ", (client_->IsHeartbeatEnabled() ? "enabled" : "disabled"));
    }

    return 0;
}

int SCConsole::handleConsoleCommand_Options(const vector<string>& argv)
{
    auto options = client_->GetOptions();
    PUT_LINE("options: ", options->ToString());
    return 0;
}

int SCConsole::handleConsoleCommand_Status(const vector<string>& argv)
{
    // 1. context
    // 2. connection status
    // 3. register status
    PUT_LINE("context: ", client_->GetContext()->ToString());
    PUT_LINE("is connected: ", client_->IsConnected() ? "yes" : "no");
    PUT_LINE("is registered: ", client_->GetContext()->is_registered ? "yes" : "no");
    return 0;
}

int SCConsole::handleConsoleCommand_Echo(const vector<string>& argv)
{
    const char* const delim = " ";
    std::ostringstream imploded;
    std::copy(++argv.begin(), argv.end(),
            std::ostream_iterator<std::string>(imploded, delim));

    cmd_handler_->Echo(imploded.str().c_str());
    return 0;
}

int SCConsole::handleConsoleCommand_Register(const vector<string>& argv)
{
    // ss_register --role <1|2|3> --access_code <ACCESS_CODE> [--with_token]

    // The default arguments can be used while disabling the default exit with these arguments.
    // This forth argument to ArgumentParser (exit_on_default_arguments) is a bool flag with a default true value.
    // The following call will retain --help and --version, but will not exit when those arguments are used.
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--id")
        .help("the endpoint id of Switch client")
        .scan<'i', EndpointId>()
        .required()
        .default_value(client_->ID());
    cmd_ap.add_argument("--role")
        .help("the role of Switch client, values: 1: normal, 2: admin, 3: service")
        .scan<'i', int>()
        .required()
        .default_value(1);
    cmd_ap.add_argument("--access_code")
        .help("the access code of Switch client");
        //.default_value("Hello World");
    cmd_ap.add_argument("--with_token")
        .help("the token last registered of Switch client")
        .flag();
    cmd_ap.add_argument("--svc_type")
        .help("the service type, only used for role is Service")
        .scan<'i', int>()
        .default_value(0);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    EndpointId ep_id = cmd_ap.get<EndpointId>("--id");

    EEndpointRole role = client_->GetContext()->role;
    if (cmd_ap.is_used("--role")) {
        role = (EEndpointRole)cmd_ap.get<int>("--role");
        if (role >= EEndpointRole::COUNT) {
            PUT_LINE("Wrong argument! the role must be given");
            return -1;
        }
    }

    string access_code;
    if (cmd_ap.is_used("--access_code")) {
        access_code = cmd_ap.get<string>("--access_code");
    }

    if (access_code.empty() && !client_->GetOptions()->access_code.empty()) {
        access_code = client_->GetOptions()->access_code;
    }

    if (access_code.empty()) {
        PUT_LINE("Wrong argument! --access_code is required");
        return -1;
    }

    bool with_token = cmd_ap.get<bool>("--with_token");
    ServiceType svc_type = 0;
    if (cmd_ap.is_used("--svc_type")) {
        svc_type = cmd_ap.get<int>("--svc_type");
    }

    cmd_handler_->SetRegisterResultHandlerCallback(
            std::bind(
                &SCConsole::onRegisterResult,
                this,
                std::placeholders::_1));
    cmd_handler_->Register(ep_id, EEndpointRole(role), access_code, with_token, svc_type);
    return 0;
}
void SCConsole::onRegisterResult(const CommandResultRegister* reg_result)
{
    PUT_LINE("* endpoint id: ", reg_result->id);
    PUT_LINE("* token: ", reg_result->token);
    PUT_LINE("* role: ", EndpointRoleToTag((EEndpointRole)reg_result->role));
}

int SCConsole::handleConsoleCommand_GetInfo(const vector<string>& argv)
{
    // ss_info [--is_details]
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--is_details")
        .help("the flag that if or not to show details information of Switch server")
        .flag();
    cmd_ap.add_argument("--endpoint")
        .help("only to get the information of specified endpoint, to get self stats if not specified")
        .scan<'i', int>()
        .default_value(-1)
        .nargs(argparse::nargs_pattern::optional);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    bool is_details = cmd_ap.get<bool>("--is_details");
    EndpointId ep_id = 0;
    if (cmd_ap.is_used("--endpoint")) {
        int _ep_id = cmd_ap.get<int>("--endpoint");
        if (_ep_id == -1) {
            ep_id = client_->GetContext()->endpoint_id;  // use self endpoint id
        } else {
            ep_id = _ep_id;
        }
    }

    cmd_handler_->SetInfoResultHandlerCallback(
            std::bind(
                &SCConsole::onGetInfoResult,
                this,
                std::placeholders::_1));
    cmd_handler_->SetEndpointInfoResultHandlerCallback(
            std::bind(
                &SCConsole::onGetEndpointInfoResult,
                this,
                std::placeholders::_1));
    cmd_handler_->GetInfo(is_details, ep_id);
    return 0;
}
void SCConsole::onGetInfoResult(const CommandInfo* cmd_info)
{
    PUT_LINE("* cmd_info.uptime: ", readable_seconds_delta(cmd_info->uptime));
    PUT_LINE("* cmd_info.endpoints.total: ", cmd_info->endpoints.total);
    PUT_LINE("* cmd_info.endpoints.rx_bytes: ", cmd_info->endpoints.rx_bytes);
    PUT_LINE("* cmd_info.admin_endpoints.total: ", cmd_info->admin_endpoints.total);
    PUT_LINE("* ...");
}
void SCConsole::onGetEndpointInfoResult(const CommandEndpointInfo* cmd_ep_info)
{
    PUT_LINE("* cmd_ep_info.uptime: ", readable_seconds_delta(cmd_ep_info->uptime));
    PUT_LINE("* cmd_ep_info.id: ", cmd_ep_info->id);
    PUT_LINE("* cmd_ep_info.role: ", EndpointRoleToTag((EEndpointRole)cmd_ep_info->role));
    PUT_LINE("* ...");
}

int SCConsole::handleConsoleCommand_ForwardTargets(const vector<string>& argv)
{
    return handleConsoleCommand_SetTargets(argv, "Set",
            std::bind(&SCCommandHandler::ForwardTargets, cmd_handler_, std::placeholders::_1));
}

int SCConsole::handleConsoleCommand_UnforwardTargets(const vector<string>& argv)
{
    return handleConsoleCommand_SetTargets(argv, "Unset",
            std::bind(&SCCommandHandler::UnforwardTargets, cmd_handler_, std::placeholders::_1));
}

int SCConsole::handleConsoleCommand_SetTargets(const vector<string>& argv, const char* desc,
        const SetTargetsCommandCallback& cmd_handler_callback)
{
    // ss_fwd --targets <ID#1 ID#2 ...>
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    char help[128];
    snprintf(help, sizeof(help), "%s target endpoints for forwarding data", desc);

    cmd_ap.add_argument("--targets")
        .help(help)
        .scan<'i', EndpointId>()
        .nargs(argparse::nargs_pattern::at_least_one);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    auto targets = cmd_ap.get<vector<EndpointId>>("--targets");
    if (targets.empty()) {
        PUT_LINE("Wrong argument! the --targets must be more than one value");
        return -1;
    }
    duplicate(targets);
    cmd_handler_callback(targets);

    if (argv[0] == "ss_fwd") {
        client_->GetContext()->SetForwardTargets(targets);
    } else if (argv[0] == "ss_unfwd") {
        client_->GetContext()->RemoveForwardTargets(targets);
    }

    return 0;
}

int SCConsole::handleConsoleCommand_Subscribe(const vector<string>& argv)
{
    return handleConsoleCommand_SubUnsubRejUnrej(argv, "Subscribe",
            std::bind(&SCCommandHandler::Subscribe, cmd_handler_, std::placeholders::_1, std::placeholders::_2));
}

int SCConsole::handleConsoleCommand_Unsubscribe(const vector<string>& argv)
{
    return handleConsoleCommand_SubUnsubRejUnrej(argv, "Unsubscribe",
            std::bind(&SCCommandHandler::Unsubscribe, cmd_handler_, std::placeholders::_1, std::placeholders::_2));
}

int SCConsole::handleConsoleCommand_Reject(const vector<string>& argv)
{
    return handleConsoleCommand_SubUnsubRejUnrej(argv, "Reject",
            std::bind(&SCCommandHandler::Reject, cmd_handler_, std::placeholders::_1, std::placeholders::_2));
}

int SCConsole::handleConsoleCommand_Unreject(const vector<string>& argv)
{
    return handleConsoleCommand_SubUnsubRejUnrej(argv, "Unreject",
            std::bind(&SCCommandHandler::Unreject, cmd_handler_, std::placeholders::_1, std::placeholders::_2));
}

int SCConsole::handleConsoleCommand_SubUnsubRejUnrej(
        const vector<string>& argv, const char* desc,
        const SubUnsubRejUnrejCommandCallback& cmd_handler_callback)
{
    // <cmd_name> --sources <ID#1 ID#2 ...>
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    char help[128];
    snprintf(help, sizeof(help), "Set sources endpoints to %s", desc);
    cmd_ap.add_argument("--sources")
        .help(help)
        .scan<'i', EndpointId>()
        .nargs(argparse::nargs_pattern::at_least_one);

    snprintf(help, sizeof(help), "Set messages to %s", desc);
    cmd_ap.add_argument("--messages")
        .help(help)
        .scan<'i', MessageId>()
        .nargs(argparse::nargs_pattern::at_least_one);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    auto sources = cmd_ap.get<vector<EndpointId>>("--sources");
    auto messages = cmd_ap.get<vector<MessageId>>("--messages");
    if (sources.empty() && messages.empty()) {
        PUT_LINE("Wrong argument! the --sources or --messages must be more than one element");
        return -1;
    }

    if (argv[0] == "ss_sub") {
        if (! sources.empty()) {
            client_->GetContext()->SetSubscribedSources(sources);
        }
        if (! messages.empty()) {
            client_->GetContext()->SetSubscribedMessages(messages);
        }
    } else if (argv[0] == "ss_unsub") {
        if (! sources.empty()) {
            client_->GetContext()->RemoveSubscribedSources(sources);
        }
        if (! messages.empty()) {
            client_->GetContext()->RemoveSubscribedMessages(messages);
        }
    } else if (argv[0] == "ss_reject") {
        if (! sources.empty()) {
            client_->GetContext()->SetRejectedSources(sources);
        }
        if (! messages.empty()) {
            client_->GetContext()->SetRejectedMessages(messages);
        }
    } else if (argv[0] == "ss_unrej") {
        if (! sources.empty()) {
            client_->GetContext()->RemoveRejectedSources(sources);
        }
        if (! messages.empty()) {
            client_->GetContext()->RemoveRejectedMessages(messages);
        }
    }

    cmd_handler_callback(sources, messages);
    return 0;
}

int SCConsole::handleConsoleCommand_Publish(const vector<string>& argv)
{
    // ss_publish <--data <DATA> | --file <FILENAME> >
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--data")
        .help("The data to send")
        .default_value(R"({"foo":"bar"})");
    cmd_ap.add_argument("--file")
        .help("The data reading from file to send");
    cmd_ap.add_argument("--targets")
        .help("The target endpoints to send")
        .scan<'i', EndpointId>()
        .nargs(argparse::nargs_pattern::at_least_one);
    cmd_ap.add_argument("--msg_type")
        .help("The message type of data")
        .scan<'i', int>()
        .nargs(1);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    vector<EndpointId> targets;
    if (cmd_ap.is_used("--targets")) {
        targets = cmd_ap.get<vector<EndpointId>>("--targets");
        if (! targets.empty()) {
            duplicate(targets);
        }
    }
    MessageId msg_type = 0;
    if (cmd_ap.is_used("--msg_type")) {
        msg_type = cmd_ap.get<int>("--msg_type");
    }

    cmd_handler_->SetPublishingResultHandlerCallback(
            std::bind(
                &SCConsole::onPublishingResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2));

    string data, data_file;
    if (cmd_ap.is_used("--data")) {
        data = cmd_ap.get<string>("--data");
        cmd_handler_->Publish(data, targets, msg_type);
    } else {
        if (cmd_ap.is_used("--file")) {
            data_file = cmd_ap.get<string>("--file");

            auto rd_done_cb = [&](int status, const string& data) {
                printf(">>> read done, status: %d, size: %ld\n", status, data.size());

                cmd_handler_->Publish(data, targets, msg_type);
            };

            bool success = AIO.async_read(data_file.c_str(), O_RDONLY, rd_done_cb);
            if (!success) {
                printf("error: read failed\n");
            }
        } else {
            PUT_LINE("Wrong argument! --data or --file must be given one");
            return -1;
        }
    }

    return 0;
}
void SCConsole::onPublishingResult(const char* data, size_t data_len)
{
    PUT_LINE("* data length: ", data_len);
    PUT_LINE("* data: ", data);  // it's string?
}

int SCConsole::handleConsoleCommand_RequestService(const vector<string>& argv)
{
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--data")
        .help("The data to send")
        .default_value(R"({"foo":"bar"})");
    cmd_ap.add_argument("--svc_type")
        .help("The service type to request")
        .scan<'i', ServiceType>()
        .nargs(1);
    cmd_ap.add_argument("--svc_cmd")
        .help("The service command to request")
        .scan<'i', MessageId>()
        .nargs(1);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    string data;
    if (cmd_ap.is_used("--data")) {
        data = cmd_ap.get<string>("--data");
    }
    ServiceType svc_type = 0;
    if (cmd_ap.is_used("--svc_type")) {
        svc_type = cmd_ap.get<ServiceType>("--svc_type");
    }
    MessageId svc_cmd = 0;
    if (cmd_ap.is_used("--svc_cmd")) {
        svc_cmd = cmd_ap.get<MessageId>("--svc_cmd");
    }

    cmd_handler_->SetServiceRequestResultHandlerCallback(
            std::bind(
                &SCConsole::onRequestServiceResult,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3));
    cmd_handler_->RequestService(data, svc_type, svc_cmd);

    return 0;
}
void SCConsole::onRequestServiceResult(const ServiceMessage* svc_msg, const char* data, size_t data_len)
{
    if (svc_msg) {
        PUT_LINE("* svc type: ", (int)svc_msg->svc_type);
        PUT_LINE("* svc cmd: ", svc_msg->svc_cmd);
        PUT_LINE("* sess_id: ", svc_msg->sess_id);
        PUT_LINE("* source: ", svc_msg->source);
    }
    PUT_LINE("* data length: ", data_len);
    PUT_LINE("* data: ", data);  // it's string?
}

int SCConsole::handleConsoleCommand_Setup(const vector<string>& argv)
{
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--new_access_code")
        .help("the new access code to set for Switch server");
    cmd_ap.add_argument("--new_admin_code")
        .help("the new admin code to set for Switch server");
    cmd_ap.add_argument("--access_code")
        .help("the old admin access code for checking permission, used for --new_admin_code");
    cmd_ap.add_argument("--mode")
        .help("the mode to set for Switch server");

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    string new_access_code, new_admin_code, access_code, new_mode;
    if (cmd_ap.is_used("--new_access_code")) {
        new_access_code = cmd_ap.get<string>("--new_access_code");
    }
    if (cmd_ap.is_used("--new_admin_code")) {
        new_admin_code = cmd_ap.get<string>("--new_admin_code");
        if (cmd_ap.is_used("--access_code")) {
            access_code = cmd_ap.get<string>("--acccess_code");
        } else {
            PUT_LINE("Wrong argument! Must given admin code for setting new admin code");
            return -1;
        }
    }
    if (cmd_ap.is_used("--mode")) {
        new_mode = cmd_ap.get<string>("--mode");
    }
    if (new_admin_code.empty() && new_access_code.empty() && new_mode.empty()) {
        PUT_LINE("Wrong argument! missing required argument(s)");
        return -1;
    }

    cmd_handler_->Setup(access_code, new_admin_code, new_access_code, new_mode);
    return 0;
}

int SCConsole::handleConsoleCommand_Kickout(const vector<string>& argv)
{
    // ss_kickout --targets <ID#1 ID#2 ...>
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--targets")
        .help("Kickout endpoints with specify targets")
        .scan<'i', EndpointId>()
        .nargs(argparse::nargs_pattern::at_least_one);

    try {
        cmd_ap.parse_args(argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << cmd_ap;
        return -1;
    }
    if (cmd_ap.is_used("--help")) {
        return 1;
    }

    auto targets = cmd_ap.get<vector<EndpointId>>("--targets");
    if (targets.empty()) {
        PUT_LINE("Wrong argument! the --targets must be more than one value");
        return -1;
    }
    duplicate(targets);
    cmd_handler_->Kickout(targets);
    return 0;
}

int SCConsole::handleConsoleCommand_Reload(const vector<string>& argv)
{
    cmd_handler_->Reload();
    return 0;
}

void SCConsole::onCommandSuccess(ECommand cmd, const char* content, size_t content_len)
{
    PUT_LINE("* ", CommandToTag(cmd), ": Successful");
}

void SCConsole::onCommandFail(ECommand cmd, const char* content, size_t content_len)
{
    PUT_LINE("* ", CommandToTag(cmd), ": Failured");
    PUT_LINE("* errmsg: ", content);
}
