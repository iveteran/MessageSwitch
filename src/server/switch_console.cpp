#include "switch_console.h"
#include "command_messages.h"
#include "switch_service.h"
#include "switch_server.h"
#include <eventloop/extensions/console.h>

using namespace evt_loop;

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
    auto service = server_->GetService();
    CommandInfoReq cmd_info_req;
    cmd_info_req.is_details = true;
    auto cmd_info = service->get_stats(cmd_info_req);
    string cmd_info_json = cmd_info->encodeToJSON();
    Console::Instance()->put_line("stats: ", cmd_info_json);
    return 0;
}
