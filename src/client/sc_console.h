#ifndef _SC_CONSOLE
#define _SC_CONSOLE

#include "switch_message.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

using std::string;
using std::vector;

class SwitchClient;
class SCCommandHandler;

class SCConsole {
    public:
    SCConsole(SwitchClient* client, SCCommandHandler* cmd_handler);
    void Destory();

    void registerCommands();

    private:
    int handleConsoleCommand_Exit(const vector<string>& argv);
    int handleConsoleCommand_Reconnect(const vector<string>& argv);
    int handleConsoleCommand_Heartbeat(const vector<string>& argv);
    int handleConsoleCommand_Options(const vector<string>& argv);
    int handleConsoleCommand_Status(const vector<string>& argv);
    int handleConsoleCommand_Echo(const vector<string>& argv);
    int handleConsoleCommand_Register(const vector<string>& argv);
    int handleConsoleCommand_GetInfo(const vector<string>& argv);
    int handleConsoleCommand_Publish(const vector<string>& argv);
    int handleConsoleCommand_RequestService(const vector<string>& argv);
    int handleConsoleCommand_ForwardTargets(const vector<string>& argv);
    int handleConsoleCommand_UnforwardTargets(const vector<string>& argv);
    int handleConsoleCommand_Subscribe(const vector<string>& argv);
    int handleConsoleCommand_Unsubscribe(const vector<string>& argv);
    int handleConsoleCommand_Reject(const vector<string>& argv);
    int handleConsoleCommand_Unreject(const vector<string>& argv);
    int handleConsoleCommand_Setup(const vector<string>& argv);
    int handleConsoleCommand_Kickout(const vector<string>& argv);
    int handleConsoleCommand_Reload(const vector<string>& argv);

    using SetTargetsCommandCallback = std::function<void (const vector<uint32_t>&)>;
    int handleConsoleCommand_SetTargets(const vector<string>& argv, const char* desc,
            const SetTargetsCommandCallback& cmd_handler_callback);

    using SubUnsubRejUnrejCommandCallback = std::function<void (const vector<uint32_t>&, const vector<uint8_t>&)>;
    int handleConsoleCommand_SubUnsubRejUnrej(const vector<string>& argv, const char* desc,
            const SubUnsubRejUnrejCommandCallback& cmd_handler_callback);

    private:
    SwitchClient* client_;
    SCCommandHandler* cmd_handler_;
};
using SCConsolePtr = std::shared_ptr<SCConsole>;

#endif  // _SC_CONSOLE
