#include "demo_console_commands.h"
#include "el_console_helper.h"
#include "demo_options.h"

void DemoConsoleCommands::RegisterCommands()
{
    REGISTER_COMMAND(
            "sc_demo_info",
            "Show self information",
            std::bind(&DemoConsoleCommands::handleConsoleCommand_Info, this, std::placeholders::_1)
            );

    REGISTER_COMMAND(
            "sc_demo_options",
            "Show self options",
            std::bind(&DemoConsoleCommands::handleConsoleCommand_Options, this, std::placeholders::_1)
            );

    // Add more command here
}

int DemoConsoleCommands::handleConsoleCommand_Info(const vector<string>& argv)
{
    PUT_LINE(argv[0], ": ", "TODO");
    return 0;
}

int DemoConsoleCommands::handleConsoleCommand_Options(const vector<string>& argv)
{
    if (demo_options_) {
        PUT_LINE(demo_options_->ToString());
    }
    return 0;
}

// Add more command handler here
