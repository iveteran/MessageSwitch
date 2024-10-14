#ifndef _UAC_CONSOLE_COMMANDS_
#define _UAC_CONSOLE_COMMANDS_

#include <string>
#include <vector>
using std::string;
using std::vector;

class SCConsole;
class UACOptions;
class UACStorer;

class UACConsoleCommands {
public:
    UACConsoleCommands(SCConsole* console, UACOptions* uac_options, UACStorer* uac_storer)
        : console_(console), uac_options_(uac_options), uac_storer_(uac_storer)
    {}

    void RegisterCommands();

protected:
    int handleConsoleCommand_Info(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);
    int handleConsoleCommand_Commands(const vector<string>& argv);
    int handleConsoleCommand_UserBaseInfo(const vector<string>& argv);

private:
    SCConsole* console_ = nullptr;
    UACOptions* uac_options_ = nullptr;
    UACStorer* uac_storer_ = nullptr;
};

#endif  // _UAC_CONSOLE_COMMANDS_
