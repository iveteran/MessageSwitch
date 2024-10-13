#include "ua_console_commands.h"
#include "ua_options.h"
#include "ua_storer.h"
#include "ua_command.h"
#include "el_console_helper.h"

void UAConsoleCommands::RegisterCommands()
{
    REGISTER_COMMAND(
            "ua_info",
            "Show self information",
            std::bind(&UAConsoleCommands::handleConsoleCommand_Info, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "ua_options",
            "Show self options",
            std::bind(&UAConsoleCommands::handleConsoleCommand_Options, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "ua_svc_cmds",
            "Show supported service commands",
            std::bind(&UAConsoleCommands::handleConsoleCommand_Commands, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "ua_users",
            "Show users",
            std::bind(&UAConsoleCommands::handleConsoleCommand_Users, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "ua_sessions",
            "Show users session",
            std::bind(&UAConsoleCommands::handleConsoleCommand_Sessions, this, std::placeholders::_1)
            );
}

int UAConsoleCommands::handleConsoleCommand_Info(const vector<string>& argv)
{
    PUT_LINE(argv[0], ": ", "TODO");
    return 0;
}

int UAConsoleCommands::handleConsoleCommand_Options(const vector<string>& argv)
{
    if (ua_options_) {
        PUT_LINE(ua_options_->ToString());
    }
    return 0;
}

int UAConsoleCommands::handleConsoleCommand_Commands(const vector<string>& argv)
{
    if (ua_options_) {
        PUT_LINE(listCommands());
    }
    return 0;
}

int UAConsoleCommands::handleConsoleCommand_Users(const vector<string>& argv)
{
    if (ua_storer_) {
        const int indent = 2;
        PUT_LINE(ua_storer_->GetUsers(indent));
    }
    return 0;
}

int UAConsoleCommands::handleConsoleCommand_Sessions(const vector<string>& argv)
{
    if (ua_storer_) {
        const int indent = 2;
        PUT_LINE(ua_storer_->GetSessions(indent));
    }
    return 0;
}

// Add more command handler here
