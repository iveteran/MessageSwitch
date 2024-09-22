#ifndef _SC_CONSOLE
#define _SC_CONSOLE

#include "switch_message.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

class SCCommandHandler;

class SCConsole {
    public:
    SCConsole(uint32_t client_id, SCCommandHandler* cmd_handler) :
        client_id_(client_id), cmd_handler_(cmd_handler)
    {}
    void Destory();

    void registerCommands();

    private:
    int handleConsoleCommand_Exit(const vector<string>& argv);
    int handleConsoleCommand_Reconnect(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);
    int handleConsoleCommand_Status(const vector<string>& argv);
    int handleConsoleCommand_Echo(const vector<string>& argv);
    int handleConsoleCommand_Register(const vector<string>& argv);
    int handleConsoleCommand_GetInfo(const vector<string>& argv);
    int handleConsoleCommand_SendData(const vector<string>& argv);
    int handleConsoleCommand_ForwardTargets(const vector<string>& argv);
    int handleConsoleCommand_Setup(const vector<string>& argv);
    int handleConsoleCommand_Kickout(const vector<string>& argv);
    int handleConsoleCommand_Reload(const vector<string>& argv);

    private:
    uint32_t client_id_;
    SCCommandHandler* cmd_handler_;
};
using SCConsolePtr = std::shared_ptr<SCConsole>;

#endif  // _SC_CONSOLE
