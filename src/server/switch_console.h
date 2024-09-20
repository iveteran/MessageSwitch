#ifndef _SWITCH_CONSOLE_H
#define _SWITCH_CONSOLE_H

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

class SwitchServer;

class SwitchConsole {
    public:
    SwitchConsole(SwitchServer* ss) : server_(ss) {}
    ~SwitchConsole() { Destory(); };
    void Destory();

    void registerCommands();

    private:
    int handleConsoleCommand_Clients(const vector<string>& argv);
    int handleConsoleCommand_Context(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);
    int handleConsoleCommand_Stats(const vector<string>& argv);

    private:
    SwitchServer* server_;
};
using SwitchConsolePtr = std::shared_ptr<SwitchConsole>;

#endif  // _SWITCH_CONSOLE_H
