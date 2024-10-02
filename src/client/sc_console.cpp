#include "sc_console.h"
#include <stdio.h>
#include <functional>
#include "command_messages.h"
#include "sc_command_handler.h"
#include "sc_options.h"
#include "sc_context.h"
#include "switch_client.h"
#include <eventloop/extensions/console.h>
#include <eventloop/extensions/aio_wrapper.h>
#include <argparse/argparse.hpp>
#include <signal.h>

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
}

void SCConsole::Destory()
{
    Console::Instance()->destory();
}

void SCConsole::registerCommands()
{
    Console::Instance()->registerCommand(
            "exit",
            "Exit Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Exit, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "reconnect",
            "Reconnect to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reconnect, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "heartbeat",
            "Enable or disable connection heartbeat to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Heartbeat, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "options",
            "Show options of Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Options, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "status",
            "Show status of Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Status, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_echo",
            "Send echo command to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Echo, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_register",
            "Register Switch client",
            std::bind(&SCConsole::handleConsoleCommand_Register, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_info",
            "Get information of Switch",
            std::bind(&SCConsole::handleConsoleCommand_GetInfo, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_publish",
            "Simulate to send custom data to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Publish, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_fwd",
            "Send forwarding targets command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_ForwardTargets, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_unfwd",
            "Send unforwarding targets command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_UnforwardTargets, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_sub",
            "Send subscribe command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Subscribe, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_unsub",
            "Send unsubscribe command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Unsubscribe, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_reject",
            "Send reject command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reject, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_unrej",
            "Send unreject command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Unreject, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_setup",
            "Send setup command to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Setup, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_kickout",
            "Send kickout comamnd to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Kickout, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
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
        Console::Instance()->put_line("reconnect: ", "the connection already connected, do nothing.");
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
        Console::Instance()->put_line("Status: ", (client_->IsHeartbeatEnabled() ? "enabled" : "disabled"));
    }

    return 0;
}

int SCConsole::handleConsoleCommand_Options(const vector<string>& argv)
{
    auto options = client_->GetOptions();
    Console::Instance()->put_line("options: ", options->ToString());
    return 0;
}

int SCConsole::handleConsoleCommand_Status(const vector<string>& argv)
{
    // 1. context
    // 2. connection status
    // 3. register status
    Console::Instance()->put_line("context: ", client_->GetContext()->ToString());
    Console::Instance()->put_line("is connected: ", client_->IsConnected() ? "yes" : "no");
    Console::Instance()->put_line("is registered: ", client_->GetContext()->is_registered ? "yes" : "no");
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
        .scan<'i', uint32_t>()
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

    int ep_id = cmd_ap.get<uint32_t>("--id");

    int role = cmd_ap.get<int>("--role");
    if (role < 1 || role > 3) {
        Console::Instance()->put_line("Wrong argument! the role must be given");
        return -1;
    }

    string access_code;
    if (cmd_ap.is_used("--access_code")) {
        access_code = cmd_ap.get<string>("--access_code");
    }

    if (access_code.empty() && !client_->GetOptions()->access_code.empty()) {
        access_code = client_->GetOptions()->access_code;
    }

    if (access_code.empty()) {
        Console::Instance()->put_line("Wrong argument! --access_code is required");
        return -1;
    }

    bool with_token = cmd_ap.get<bool>("--with_token");

    cmd_handler_->Register(ep_id, EEndpointRole(role), access_code, with_token);
    return 0;
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
    uint32_t ep_id = 0;
    if (cmd_ap.is_used("--endpoint")) {
        int _ep_id = cmd_ap.get<int>("--endpoint");
        if (_ep_id == -1) {
            ep_id = client_->GetContext()->endpoint_id;  // use self endpoint id
        } else {
            ep_id = _ep_id;
        }
    }
    cmd_handler_->GetInfo(is_details, ep_id);  // TODO: pass callback function
    return 0;
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
        .scan<'i', uint32_t>()
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

    auto targets = cmd_ap.get<vector<uint32_t>>("--targets");
    if (targets.empty()) {
        Console::Instance()->put_line("Wrong argument! the --targets must be more than one value");
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
        .scan<'i', uint32_t>()
        .nargs(argparse::nargs_pattern::at_least_one);

    snprintf(help, sizeof(help), "Set messages to %s", desc);
    cmd_ap.add_argument("--messages")
        .help(help)
        .scan<'i', uint8_t>()
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

    auto sources = cmd_ap.get<vector<uint32_t>>("--sources");
    auto messages = cmd_ap.get<vector<uint8_t>>("--messages");
    if (sources.empty() && messages.empty()) {
        Console::Instance()->put_line("Wrong argument! the --sources or --messages must be more than one element");
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
        .help("The data be send");
    cmd_ap.add_argument("--file")
        .help("The data reading from file be send");

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

    string data, data_file;
    if (cmd_ap.is_used("--data")) {
        data = cmd_ap.get<string>("--data");
        cmd_handler_->SendData(data);
    } else {
        if (cmd_ap.is_used("--file")) {
            data_file = cmd_ap.get<string>("--file");

            auto rd_done_cb = [&](int status, const string& data) {
                printf(">>> read done, status: %d, size: %ld\n", status, data.size());

                cmd_handler_->SendData(data);
            };

            bool success = AIO.async_read(data_file.c_str(), O_RDONLY, rd_done_cb);
            if (!success) {
                printf("error: read failed\n");
            }
        } else {
            Console::Instance()->put_line("Wrong argument! --data or --file must be given one");
            return -1;
        }
    }
    //cmd_handler_->SendData(data);

    return 0;
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
            Console::Instance()->put_line("Wrong argument! Must given admin code for setting new admin code");
            return -1;
        }
    }
    if (cmd_ap.is_used("--mode")) {
        new_mode = cmd_ap.get<string>("--mode");
    }
    if (new_admin_code.empty() && new_access_code.empty() && new_mode.empty()) {
        Console::Instance()->put_line("Wrong argument! missing required argument(s)");
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
        .scan<'i', uint32_t>()
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

    auto targets = cmd_ap.get<vector<uint32_t>>("--targets");
    if (targets.empty()) {
        Console::Instance()->put_line("Wrong argument! the --targets must be more than one value");
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
