#ifndef _DEMO_CONSOLE_COMMANDS_
#define _DEMO_CONSOLE_COMMANDS_

#include <string>
#include <vector>
using std::string;
using std::vector;

class SCConsole;

class DemoConsoleCommands {
public:
    DemoConsoleCommands(SCConsole* console)
        : console_(console)
    {}

    void RegisterCommands();

protected:
    int handleConsoleCommand_Info(const vector<string>& argv);

private:
    SCConsole* console_ = nullptr;
};

#endif  // _DEMO_CONSOLE_COMMANDS_
