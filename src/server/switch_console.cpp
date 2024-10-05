#include "switch_console.h"
#include "command_messages.h"
#include "switch_service.h"
#include "switch_server.h"
#include "utils/time.h"
#include <eventloop/extensions/console.h>
#include <argparse/argparse.hpp>

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
    Console::Instance()->registerCommand(
            "clients",
            "Show number of connected clients",
            std::bind(&SwitchConsole::handleConsoleCommand_Clients, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "options",
            "Show options of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Options, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "context",
            "Show runtime context of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Context, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "stats",
            "Show runtime statistics of server",
            std::bind(&SwitchConsole::handleConsoleCommand_Stats, this, std::placeholders::_1)
            );
}

int SwitchConsole::handleConsoleCommand_Clients(const vector<string>& argv)
{
    Console::Instance()->put_line("clients: ", server_->GetClientsTotal());
    return 0;
}
int SwitchConsole::handleConsoleCommand_Options(const vector<string>& argv)
{
    auto options = server_->GetOptions();
    Console::Instance()->put_line("options: ", options->ToString());
    return 0;
}
int SwitchConsole::handleConsoleCommand_Context(const vector<string>& argv)
{
    auto context = server_->GetContext();
    Console::Instance()->put_line("context: ", context->ToString());
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
            Console::Instance()->put_line(errmsg);
        } else {
            string cmd_ep_info_json = cmd_ep_info->encodeToJSON();
            Console::Instance()->put_line("stats: ", cmd_ep_info_json);
            Console::Instance()->put_line("* uptime: ", readable_seconds_delta(cmd_ep_info->uptime));
        }
    } else {
        auto cmd_info = service->get_stats(cmd_info_req);
        string cmd_info_json = cmd_info->encodeToJSON();
        Console::Instance()->put_line("stats: ", cmd_info_json);
        Console::Instance()->put_line("* uptime: ", readable_seconds_delta(cmd_info->uptime));
    }
    return 0;
}
