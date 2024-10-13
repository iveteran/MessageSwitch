#ifndef _UA_CONSOLE_COMMANDS_
#define _UA_CONSOLE_COMMANDS_

#include <string>
#include <vector>
using std::string;
using std::vector;

class SCConsole;
class UAOptions;
class UAStorer;

class UAConsoleCommands {
public:
    UAConsoleCommands(SCConsole* console, UAOptions* ua_options, UAStorer* ua_storer)
        : console_(console), ua_options_(ua_options), ua_storer_(ua_storer)
    {}

    void RegisterCommands();

protected:
    int handleConsoleCommand_Info(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);
    int handleConsoleCommand_Commands(const vector<string>& argv);
    int handleConsoleCommand_Users(const vector<string>& argv);
    int handleConsoleCommand_Sessions(const vector<string>& argv);

private:
    SCConsole* console_ = nullptr;
    UAOptions* ua_options_ = nullptr;
    UAStorer* ua_storer_ = nullptr;
};

#endif  // _UA_CONSOLE_COMMANDS_
