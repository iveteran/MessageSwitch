#include "uac_console_commands.h"
#include "uac_options.h"
#include "uac_storer.h"
#include "uac_command.h"
#include "el_console_helper.h"

void UACConsoleCommands::RegisterCommands()
{
    REGISTER_COMMAND(
            "uac_info",
            "Show self information",
            std::bind(&UACConsoleCommands::handleConsoleCommand_Info, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "uac_options",
            "Show self options",
            std::bind(&UACConsoleCommands::handleConsoleCommand_Options, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "uac_svc_cmds",
            "Show supported service commands",
            std::bind(&UACConsoleCommands::handleConsoleCommand_Commands, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "uac_user_base_info",
            "Show user base info",
            std::bind(&UACConsoleCommands::handleConsoleCommand_UserBaseInfo, this, std::placeholders::_1)
            );
}

int UACConsoleCommands::handleConsoleCommand_Info(const vector<string>& argv)
{
    PUT_LINE(argv[0], ": ", "TODO");
    return 0;
}

int UACConsoleCommands::handleConsoleCommand_Options(const vector<string>& argv)
{
    if (uac_options_) {
        PUT_LINE(uac_options_->ToString());
    }
    return 0;
}

int UACConsoleCommands::handleConsoleCommand_Commands(const vector<string>& argv)
{
    if (uac_options_) {
        PUT_LINE(listCommands());
    }
    return 0;
}

int UACConsoleCommands::handleConsoleCommand_UserBaseInfo(const vector<string>& argv)
{
    if (uac_storer_) {
        const int indent = 2;
        PUT_LINE(uac_storer_->ListAllUserBaseInfo(indent));
    }
    return 0;
}

// Add more command handler here
