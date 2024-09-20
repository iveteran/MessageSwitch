#include "sc_console.h"
#include <stdio.h>
#include <functional>
#include "command_messages.h"
#include "sc_command_handler.h"
#include <eventloop/extensions/console.h>

using namespace evt_loop;

void SCConsole::Destory()
{
    Console::Instance()->destory();
}

void SCConsole::registerCommands()
{
    Console::Instance()->registerCommand(
            "reconnect",
            "Reconnect to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_Reconnect, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "options",
            "Show options of program",
            std::bind(&SCConsole::handleConsoleCommand_Options, this, std::placeholders::_1)
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
            "ss_data",
            "Simulate to send custom data to Switch server",
            std::bind(&SCConsole::handleConsoleCommand_SendData, this, std::placeholders::_1)
            );
    Console::Instance()->registerCommand(
            "ss_fwd_data",
            "Send forwarding data command by Switch server",
            std::bind(&SCConsole::handleConsoleCommand_ForwardData, this, std::placeholders::_1)
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

int SCConsole::handleConsoleCommand_Reconnect(const vector<string>& argv) 
{
    Console::Instance()->put_line("reconnect: ", "TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_Options(const vector<string>& argv)
{
    Console::Instance()->put_line("options: ", "TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_Register(const vector<string>& argv)
{
    cmd_handler_->Register();
    return 0;
}

int SCConsole::handleConsoleCommand_GetInfo(const vector<string>& argv)
{
    cmd_handler_->GetInfo(/* bind callback here */);
    return 0;
}

int SCConsole::handleConsoleCommand_SendData(const vector<string>& argv)
{
    Console::Instance()->put_line("TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_ForwardData(const vector<string>& argv)
{
    Console::Instance()->put_line("TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_Setup(const vector<string>& argv)
{
    Console::Instance()->put_line("TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_Kickout(const vector<string>& argv)
{
    Console::Instance()->put_line("TODO");
    return 0;
}

int SCConsole::handleConsoleCommand_Reload(const vector<string>& argv)
{
    Console::Instance()->put_line("TODO");
    return 0;
}
