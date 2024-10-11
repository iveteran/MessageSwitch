#ifndef _DEMO_CONSOLE_COMMANDS_
#define _DEMO_CONSOLE_COMMANDS_

#include <string>
#include <vector>
using std::string;
using std::vector;

class SCConsole;
class DemoOptions;

class DemoConsoleCommands {
public:
    DemoConsoleCommands(SCConsole* console, DemoOptions* demo_options)
        : console_(console), demo_options_(demo_options)
    {}

    void RegisterCommands();

protected:
    int handleConsoleCommand_Info(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);

private:
    SCConsole* console_ = nullptr;
    DemoOptions* demo_options_ = nullptr;
};

#endif  // _DEMO_CONSOLE_COMMANDS_
