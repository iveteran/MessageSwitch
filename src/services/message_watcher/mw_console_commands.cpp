#include "mw_console_commands.h"
#include "el_console_helper.h"

void MWConsoleCommands::RegisterCommands()
{
    REGISTER_COMMAND(
            "mw_stats",
            "Show statistics of Message Watch",
            std::bind(&MWConsoleCommands::handleConsoleCommand_Stats, this, std::placeholders::_1)
            );
}

int MWConsoleCommands::handleConsoleCommand_Stats(const vector<string>& argv)
{
    PUT_LINE(argv[0], ": ", "TODO");
    return 0;
}
