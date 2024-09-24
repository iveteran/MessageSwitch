#ifndef _SWITCH_SERVICE_H
#define _SWITCH_SERVICE_H

#include "command_messages.h"
#include <tuple>

using std::tuple;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

class CommandInfoReq;
class SwitchServer;
class Endpoint;

class SwitchService {

public:
    SwitchService(const SwitchServer* switch_server) :
        switch_server_(switch_server)
    {}

    tuple<int, string> register_endpoint(TcpConnection* conn, const CommandRegister& cmd_reg);
    int handle_service_point(TcpConnection* conn, const CommandRegister& reg_cmd);
    CommandInfoPtr get_stats(const CommandInfoReq& cmd_info_req);
    tuple<int, string> forward(Endpoint* ep, const CommandForward& cmd_fwd);
    tuple<int, string> setup(const CommandSetup& cmd_setup);
    tuple<int, string> kickout_endpoint(const CommandKickout& cmd_kickout);

private:
    void kickout_endpoint(Endpoint* ep);

private:
    const SwitchServer* switch_server_;
};
using SwitchServicePtr = std::shared_ptr<SwitchService>;

#endif // _SWITCH_SERVICE_H
