#include "switch_console.h"
#include "command_messages.h"
#include "switch_service.h"
#include "switch_server.h"
#include "utils/time.h"
#include <eventloop/extensions/console.h>
#include <argparse/argparse.hpp>

#define PUT_LINE Console::Instance()->put_line
#define REGISTER_COMMAND Console::Instance()->registerCommand

using namespace evt_loop;


SwitchConsole::SwitchConsole(SwitchServer* ss) : server_(ss)
{
    const char* prompt = "Switch> ";
    Console::Initialize(prompt);
}

void SwitchConsole::Destory()
{
    // XXX: MUST call destory of Console manually, otherwise the terminal will be silently always
    Console::Instance()->destory();
}

void SwitchConsole::registerCommands()
{
    REGISTER_COMMAND(
            "clients",
            "Show number of connected clients",
            std::bind(&SwitchConsole::handleConsoleCommand_Clients, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "options",
            "Show options of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Options, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "context",
            "Show runtime context of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Context, this, std::placeholders::_1)
            );
    REGISTER_COMMAND(
            "stats",
            "Show runtime statistics of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Stats, this, std::placeholders::_1)
            );
}

int SwitchConsole::handleConsoleCommand_Clients(const vector<string>& argv)
{
    PUT_LINE("* clients: ", server_->GetClientsTotal());
    return 0;
}
int SwitchConsole::handleConsoleCommand_Options(const vector<string>& argv)
{
    auto options = server_->GetOptions();
    PUT_LINE("* options: ", options->ToString());
    return 0;
}
int SwitchConsole::handleConsoleCommand_Context(const vector<string>& argv)
{
    auto context = server_->GetContext();
    PUT_LINE("* context: ", context->ToString());
    return 0;
}
int SwitchConsole::handleConsoleCommand_Stats(const vector<string>& argv)
{
    // TODO: add sub commands: [--endpoint EP_ID]
    argparse::ArgumentParser cmd_ap(argv[0], "1.0", argparse::default_arguments::help, false);

    cmd_ap.add_argument("--is_details")
        .help("The flag that whether to show details")
        .flag();
    cmd_ap.add_argument("--endpoint")
        .help("Get stats of specified endpoint")
        .scan<'i', EndpointId>()
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
    auto service = server_->GetService();

    CommandInfoReq cmd_info_req;
    cmd_info_req.is_details = cmd_ap.get<bool>("--is_details");
    if (cmd_ap.is_used("--endpoint")) {
        EndpointId ep_id = cmd_ap.get<EndpointId>("--endpoint");
        if (ep_id > 0) {
            cmd_info_req.endpoint_id = ep_id;
        }
    }

    if (cmd_info_req.endpoint_id > 0) {
        auto [status, errmsg, cmd_ep_info] = service->get_endpoint_stats(cmd_info_req);
        if (! errmsg.empty()) {
            PUT_LINE("* ", errmsg);
        } else {
            string cmd_ep_info_json = cmd_ep_info->encodeToJSON();
            PUT_LINE("* stats: ", cmd_ep_info_json);
            PUT_LINE("* uptime: ", readable_seconds_delta(cmd_ep_info->uptime));
        }
    } else {
        auto cmd_info = service->get_stats(cmd_info_req);
        string cmd_info_json = cmd_info->encodeToJSON();
        PUT_LINE("* stats: ", cmd_info_json);
        PUT_LINE("* uptime: ", readable_seconds_delta(cmd_info->uptime));
    }
    return 0;
}
