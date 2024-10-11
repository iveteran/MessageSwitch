#ifndef _MW_CONSOLE_COMMANDS_
#define _MW_CONSOLE_COMMANDS_

#include <string>
#include <vector>
using std::string;
using std::vector;

class SCConsole;

class MWConsoleCommands {
public:
    MWConsoleCommands(SCConsole* console)
        : console_(console)
    {}

    void RegisterCommands();
    
protected:
    int handleConsoleCommand_Stats(const vector<string>& argv);

private:
    SCConsole* console_ = nullptr;
};

#endif  // _MW_CONSOLE_COMMANDS_
