#include "demo_console_commands.h"
#include "el_console_helper.h"

void DemoConsoleCommands::RegisterCommands()
{
    REGISTER_COMMAND(
            "sc_demo_info",
            "Show self information",
            std::bind(&DemoConsoleCommands::handleConsoleCommand_Info, this, std::placeholders::_1)
            );

    // Add more command here
}

int DemoConsoleCommands::handleConsoleCommand_Info(const vector<string>& argv)
{
    PUT_LINE(argv[0], ": ", "TODO");
    return 0;
}

// Add more command handler here
