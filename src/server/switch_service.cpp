#include "switch_service.h"
#include "switch_context.h"
#include "switch_server.h"
#include "utils/crypto.h"
#include "utils/random.h"
#include <sstream>

tuple<int, string, CommandResultRegisterPtr>
SwitchService::register_endpoint(TcpConnection* conn, const CommandRegister& reg_cmd)
{
    auto context = switch_server_->GetContext();

    if (reg_cmd.role.empty() || reg_cmd.access_code.empty()) {
        return { 1, "Missing required parameter(s)", nullptr };
    }

    EEndpointRole role = TagToEndpointRole(reg_cmd.role);
    int8_t errcode = 1;
    std::stringstream ss;
    ss << "Authentication failed, role: " << reg_cmd.role;

    switch (role) {
        case EEndpointRole::Endpoint:
            if (reg_cmd.access_code.empty() || reg_cmd.access_code != context->access_code) {
                return { errcode, ss.str(), nullptr };
            }
            break;
        case EEndpointRole::Admin:
            if (reg_cmd.access_code.empty() || reg_cmd.access_code != context->admin_code) {
                return { errcode, ss.str(), nullptr };
            }
            break;
        case EEndpointRole::Service:
            if (reg_cmd.access_code.empty() || reg_cmd.access_code != context->service_access_code) {
                return { errcode, ss.str(), nullptr };
            }
            handle_service_point(conn, reg_cmd);
            break;
        default:
            {
                std::stringstream ss;
                ss << "Has invalid parameter, role: " << reg_cmd.role;
                return { errcode, ss.str(), nullptr };
            }
            break;
    }

    EndpointId ep_id = reg_cmd.id;
    if (ep_id == 0) {
        ep_id = allocate_endpoint_id();
    }

    auto regResult = std::make_shared<CommandResultRegister>();
    regResult->id = ep_id;

    auto iter = context->endpoints.find(ep_id);
    if (iter == context->endpoints.end()) {
        // new
        auto ep = std::make_shared<Endpoint>(ep_id, conn);
        ep->SetRole(role);
        auto token = generate_token(ep.get());
        ep->SetToken(token);
        context->endpoints[ep_id] = ep;
        if (role == EEndpointRole::Admin) {
            context->admin_clients[ep_id] = ep;
        }
        regResult->token = token;
    } else {
        // exists
        auto exists_ep = iter->second;
        if (exists_ep->Connection()->FD() != conn->FD()) {
            if (! reg_cmd.token.empty() && reg_cmd.token == exists_ep->GetToken()) {
                // kickout
                kickout_endpoint(exists_ep.get());
                exists_ep->SetConnection(conn);
            } else {
                int errcode = 1;
                string errmsg("You already registered on another device, is endpoint id correct? or provide the token of last registered");
                return { errcode, errmsg, nullptr };
            }
        }

        if (! reg_cmd.token.empty() && reg_cmd.token == exists_ep->GetToken()) {
            // update token
            auto token = generate_token(exists_ep.get());
            exists_ep->SetToken(token);
            regResult->token = token;
        }
        if (role != exists_ep->GetRole()) {
            // switch role
            exists_ep->SetRole(role);
            if (role == EEndpointRole::Admin) {
                context->admin_clients[ep_id] = exists_ep;
            }
        }
    }

    context->pending_clients.erase(conn->FD());

    return { 0, "", regResult };
}

int SwitchService::handle_service_point(TcpConnection* conn, const CommandRegister& reg_cmd)
{
    return 0;
}

CommandInfoPtr
SwitchService::get_stats(const CommandInfoReq& cmd_info_req)
{
    auto context = switch_server_->GetContext();

    size_t rx_bytes = 0;
    size_t tx_bytes = 0;
    for (auto [ep_id, ep] : context->endpoints) {
        rx_bytes += ep->Connection()->StatsRxBytes();
        tx_bytes += ep->Connection()->StatsTxBytes();
    }

    auto cmd_info = std::make_shared<CommandInfo>();
    cmd_info->id = switch_server_->NodeId();
    cmd_info->uptime = Now() - context->born_time;
    cmd_info->serving_mode = ServingModeToTag(context->serving_mode);
    cmd_info->access_code = context->access_code;
    cmd_info->admin_code = context->admin_code;
    cmd_info->endpoints.total = context->endpoints.size();
    cmd_info->endpoints.rx_bytes = rx_bytes;
    cmd_info->endpoints.tx_bytes = tx_bytes;
    cmd_info->admin_clients.total = context->admin_clients.size();
    cmd_info->pending_clients.total = context->pending_clients.size();

    if (cmd_info_req.is_details) {
        cmd_info->details.dummy_arr.push_back(11);
        for (auto [ep_id, _] : context->endpoints) {
            cmd_info->details.endpoints[ep_id] = 22;
        }
    }

    return cmd_info;
}

tuple<int, string>
SwitchService::forward(Endpoint* ep, const CommandForward& cmd_fwd)
{
    if (cmd_fwd.targets.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    ep->SetForwardTargets(cmd_fwd.targets);
    return { 0, "" };
}

tuple<int, string>
SwitchService::setup(const CommandSetup& cmd_setup)
{
    auto context = switch_server_->GetContext();

    if (! cmd_setup.new_admin_code.empty()) {
        if (cmd_setup.access_code.empty() || cmd_setup.access_code != context->admin_code) {
            int8_t errcode = 1;
            string errmsg("Authentication failed");
            return { errcode, errmsg };
        }

        context->admin_code = cmd_setup.new_admin_code;
    }

    if (! cmd_setup.new_access_code.empty()) {
        context->access_code = cmd_setup.new_access_code;
    }

    if (! cmd_setup.mode.empty()) {
        EServingMode mode = TagToServingMode(cmd_setup.mode);
        if (mode == EServingMode::Undefined) {
            int8_t errcode = 1;
            std::stringstream ss;
            ss << "Has invalid parameter, mode: " << cmd_setup.mode;
            return { errcode, ss.str() };
        }
        context->serving_mode = mode;
    }

    return { 0, "" };
}

tuple<int, string>
SwitchService::kickout_endpoint(const CommandKickout& cmd_kickout)
{
    auto context = switch_server_->GetContext();

    if (cmd_kickout.targets.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    for (auto ep_id : cmd_kickout.targets) {
        auto iter = context->endpoints.find(ep_id);
        if (iter == context->endpoints.end()) {
            continue;
        }
        auto target_ep = iter->second;
        kickout_endpoint(target_ep.get());
    }

    return { 0, "" };
}

EndpointId SwitchService::allocate_endpoint_id()
{
    uint32_t ep_id = generate_random_integer();
    // to find out the generated ep_id whether exists
    auto context = switch_server_->GetContext();
    while (context->endpoints.find(ep_id) != context->endpoints.end()) {
        ep_id = generate_random_integer();  // re-generate
    }
    return ep_id;
}
string SwitchService::generate_token(Endpoint* ep)
{
    stringstream ss;
    ss << ep->Id() << ":" << generate_random_integer() << ":" << ep->Connection()->FD();
    return md5(ss.str());
}
void SwitchService::kickout_endpoint(Endpoint* ep)
{
    printf("[handleKickout] kickout endpoint, id: %d, connection (id: %d, fd: %d)\n",
            ep->Id(), ep->Connection()->ID(), ep->Connection()->FD());
    auto cmd_handler = switch_server_->GetCommandHandler();
    cmd_handler->sendResultMessage(ep->Connection(), ECommand::KICKOUT, 0, "Kickout by admin or logged in at another device");
    ep->Connection()->Disconnect(); // XXX: delay 1 second to do this?
}
