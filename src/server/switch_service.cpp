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

    if (reg_cmd.role == 0 || reg_cmd.role >= (uint8_t)EEndpointRole::COUNT || reg_cmd.access_code.empty()) {
        return { 1, "Missing required parameter(s)", nullptr };
    }

    EEndpointRole role = (EEndpointRole)reg_cmd.role;
    int8_t errcode = 1;
    std::stringstream ss;
    ss << "Authentication failed, role: " << reg_cmd.role;

    switch (role) {
        case EEndpointRole::Normal:
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

    auto& any_endpoints = context->endpoints;
    auto iter = any_endpoints.find(ep_id);
    if (iter == any_endpoints.end()) {
        // new
        auto ep = std::make_shared<Endpoint>(ep_id, conn);
        ep->SetRole(role);
        auto token = generate_token(ep.get());
        ep->SetToken(token);
        any_endpoints[ep_id] = ep;
        switch (role) {
            case EEndpointRole::Normal:
                context->normal_endpoints[ep_id] = ep;
                break;
            case EEndpointRole::Admin:
                context->admin_endpoints[ep_id] = ep;
                break;
            case EEndpointRole::Service:
                //handle_service_point(ep.get(), reg_cmd);
                ep->SetServiceType(reg_cmd.svc_type);
                context->service_endpoints[reg_cmd.svc_type].insert(ep);
                break;
            default:
                printf("[Register] Unsupported endpoint role: %d\n", int(role));
                break;
        }
        regResult->token = token;
        conn->SetID(ep_id);   // assign endpoint id to connection
    } else {
        // exists
        auto& exists_ep = iter->second;
        auto exists_svc_type = exists_ep->GetServiceType();
        if (exists_ep->Connection()->FD() != conn->FD()) {
            if (! reg_cmd.token.empty() && reg_cmd.token == exists_ep->GetToken()) {
                // in difference connection, kickout older
                kickout_endpoint(exists_ep.get());
                exists_ep->SetConnection(conn);
            } else {
                int errcode = 1;
                string errmsg("You already registered on another device, is endpoint id correct? or provide the token of last registered");
                return { errcode, errmsg, nullptr };
            }
        }

        if ((! reg_cmd.token.empty() && reg_cmd.token == exists_ep->GetToken())
                || role != exists_ep->GetRole()) {
            // update token
            auto token = generate_token(exists_ep.get());
            exists_ep->SetToken(token);
            regResult->token = token;
        }
        if (role != exists_ep->GetRole()) {
            // switch role
            switch (exists_ep->GetRole()) {
                case EEndpointRole::Normal:
                    context->normal_endpoints.erase(ep_id);
                    break;
                case EEndpointRole::Admin:
                    context->admin_endpoints.erase(ep_id);
                    break;
                case EEndpointRole::Service:
                    context->service_endpoints[exists_svc_type].erase(exists_ep);
                    if (context->service_endpoints[exists_svc_type].empty()) {
                        context->service_endpoints.erase(exists_svc_type);
                    }
                    break;
                default:
                    printf("[Register] Unsupported endpoint role: %d\n", int(exists_ep->GetRole()));
                    break;
            }
            switch (role) {
                case EEndpointRole::Normal:
                    context->normal_endpoints[ep_id] = exists_ep;
                    break;
                case EEndpointRole::Admin:
                    context->admin_endpoints[ep_id] = exists_ep;
                    break;
                case EEndpointRole::Service:
                    context->service_endpoints[reg_cmd.svc_type].insert(exists_ep);
                    break;
                default:
                    printf("[Register] Unsupported endpoint role: %d\n", int(exists_ep->GetRole()));
                    break;
            }
            exists_ep->SetRole(role);
            regResult->role = (uint8_t)role;
        }
        if (role == EEndpointRole::Service &&
                reg_cmd.svc_type != exists_svc_type) {
            // switch service type
            context->service_endpoints[exists_svc_type].erase(exists_ep);
            if (context->service_endpoints[exists_svc_type].empty()) {
                context->service_endpoints.erase(exists_svc_type);
            }
            context->service_endpoints[reg_cmd.svc_type].insert(exists_ep);
            exists_ep->SetServiceType(reg_cmd.svc_type);
        }
    }

    context->pending_clients.erase(conn->FD());

    return { 0, "", regResult };
}

int SwitchService::handle_service_point(const Endpoint* ep, const CommandRegister& reg_cmd)
{
    /*
    auto context = switch_server_->GetContext();
    context->service_endpoints[reg_cmd.svc_type].insert(ep);
    ep->SetServiceType(reg_cmd.svc_type);
    */
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
    cmd_info->admin_endpoints.total = context->admin_endpoints.size();
    cmd_info->normal_endpoints.total = context->normal_endpoints.size();
    cmd_info->service_endpoints.svc_type_total = context->service_endpoints.size();
    for (auto [_, svc_eps] : context->service_endpoints) {
        cmd_info->service_endpoints.svc_ep_total += svc_eps.size();
    }

    cmd_info->message_subscribers.msg_type_total = context->message_subscribers.size();
    set<EndpointId> msg_eps_set;
    for (auto [_, msg_eps] : context->message_subscribers) {
        msg_eps_set.insert(msg_eps.begin(), msg_eps.end());
    }
    cmd_info->message_subscribers.msg_ep_total = msg_eps_set.size();

    cmd_info->pending_clients.total = context->pending_clients.size();

    if (cmd_info_req.is_details) {
        for (auto [ep_id, ep] : context->endpoints) {
            cmd_info->endpoints.eps[ep_id]["uptime"] = Now() - ep->GetBornTime();
        }
        for (auto [ep_id, _] : context->normal_endpoints) {
            cmd_info->normal_endpoints.eps.push_back(ep_id);
        }
        for (auto [ep_id, _] : context->admin_endpoints) {
            cmd_info->admin_endpoints.eps.push_back(ep_id);
        }
        for (auto [svc_type, ep_set] : context->service_endpoints) {
            for (auto ep : ep_set) {
                cmd_info->service_endpoints.eps[svc_type].push_back(ep->Id());
            }
        }
        for (auto [msg_type, ep_id_set] : context->message_subscribers) {
            for (auto ep_id : ep_id_set) {
                cmd_info->message_subscribers.eps[msg_type].push_back(ep_id);
            }
        }
    }

    return cmd_info;
}

tuple<int, string, CommandEndpointInfoPtr>
SwitchService::get_endpoint_stats(const CommandInfoReq& cmd_info_req)
{
    if (cmd_info_req.endpoint_id == 0) {
        return { 1, "Must specify an endpoint id", nullptr };
    }
    auto context = switch_server_->GetContext();
    auto iter = context->endpoints.find(cmd_info_req.endpoint_id);
    if (iter == context->endpoints.end()) {
        char errmsg[64];
        snprintf(errmsg, sizeof(errmsg), "Can not find out the endpoint id: %d", cmd_info_req.endpoint_id);
        return { 1, errmsg, nullptr };
    }
    auto ep = iter->second;
    auto cmd_ep_info = std::make_shared<CommandEndpointInfo>();
    cmd_ep_info->id = ep->Id();
    cmd_ep_info->role = (uint8_t)(ep->GetRole());
    cmd_ep_info->uptime = Now() - ep->GetBornTime();
    cmd_ep_info->svc_type = ep->GetServiceType();
    std::copy(ep->GetForwardTargets().begin(), ep->GetForwardTargets().end(),
          std::back_inserter(cmd_ep_info->fwd_targets));

    std::copy(ep->GetSubscriedSources().begin(), ep->GetSubscriedSources().end(),
          std::back_inserter(cmd_ep_info->subs_sources));
    std::copy(ep->GetRejectedSources().begin(), ep->GetRejectedSources().end(),
          std::back_inserter(cmd_ep_info->rej_sources));
    std::copy(ep->GetSubscriedMessages().begin(), ep->GetSubscriedMessages().end(),
          std::back_inserter(cmd_ep_info->subs_messages));
    std::copy(ep->GetRejectedMessages().begin(), ep->GetRejectedMessages().end(),
          std::back_inserter(cmd_ep_info->rej_messages));

    cmd_ep_info->rx_bytes += ep->Connection()->StatsRxBytes();
    cmd_ep_info->tx_bytes += ep->Connection()->StatsTxBytes();
    return { 0, "", cmd_ep_info };
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
SwitchService::unforward(Endpoint* ep, const CommandUnforward& cmd_unfwd)
{
    if (cmd_unfwd.targets.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    ep->UnsetForwardTargets(cmd_unfwd.targets);
    return { 0, "" };
}

tuple<int, string>
SwitchService::subscribe(Endpoint* ep, const CommandSubscribe& cmd_sub)
{
    if (cmd_sub.sources.empty() && cmd_sub.messages.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    if (!cmd_sub.sources.empty()) {
        ep->SubscribeSources(cmd_sub.sources);
    }
    if (!cmd_sub.messages.empty()) {
        ep->SubscribeMessages(cmd_sub.messages);
    }

    for (auto msg_type : cmd_sub.messages) {
        auto context = switch_server_->GetContext();
        auto iter = context->message_subscribers.find(msg_type);
        if (iter != context->message_subscribers.end()) {
            auto& ep_set = iter->second;
            ep_set.insert(ep->Id());
        } else {
            set<EndpointId> ep_set;
            ep_set.insert(ep->Id());
            context->message_subscribers[msg_type] = ep_set;
        }
        //context->message_subscribers[msg_type].insert(ep->Id());
    }

    return { 0, "" };
}

tuple<int, string>
SwitchService::unsubscribe(Endpoint* ep, const CommandUnsubscribe& cmd_unsub)
{
    if (cmd_unsub.sources.empty() && cmd_unsub.messages.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    if (!cmd_unsub.sources.empty()) {
        ep->UnsubscribeSources(cmd_unsub.sources);
    }
    if (!cmd_unsub.messages.empty()) {
        ep->UnsubscribeMessages(cmd_unsub.messages);
    }

    for (auto msg_type : cmd_unsub.messages) {
        auto context = switch_server_->GetContext();
        auto iter = context->message_subscribers.find(msg_type);
        if (iter != context->message_subscribers.end()) {
            auto& ep_set = iter->second;
            ep_set.erase(ep->Id());
            if (ep_set.empty()) {
                context->message_subscribers.erase(iter);
            }
        }
    }

    return { 0, "" };
}

tuple<int, string>
SwitchService::reject(Endpoint* ep, const CommandReject& cmd_rej)
{
    if (cmd_rej.sources.empty() && cmd_rej.messages.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    if (!cmd_rej.sources.empty()) {
        ep->RejectSources(cmd_rej.sources);
    }
    if (!cmd_rej.messages.empty()) {
        ep->RejectMessages(cmd_rej.messages);
    }
    return { 0, "" };
}

tuple<int, string>
SwitchService::unreject(Endpoint* ep, const CommandUnreject& cmd_unrej)
{
    if (cmd_unrej.sources.empty() && cmd_unrej.messages.empty()) {
        int8_t errcode = 1;
        string errmsg("Missing required parameter or the parameter is invalid");
        return { errcode, errmsg };
    }

    if (!cmd_unrej.sources.empty()) {
        ep->UnrejectSources(cmd_unrej.sources);
    }
    if (!cmd_unrej.messages.empty()) {
        ep->UnrejectMessages(cmd_unrej.messages);
    }
    return { 0, "" };
}

bool SwitchService::is_forwarding_allowed(const Endpoint* source_ep, const Endpoint* target_ep, MessageId msg_type)
{
    if (source_ep->Id() == target_ep->Id()) {
        return false;
    }

    bool is_allowed = true;
    if (target_ep->IsRejectedSource(source_ep->Id())) {
        // check source blacklist
        printf("[handlePublishData] the endpoint in blacklist of target,"
                " be rejected, source ep id: %d, target ep id: %d\n",
                source_ep->Id(), target_ep->Id());
        is_allowed = false;
    } else if (! target_ep->IsSubscribedSource(source_ep->Id())) {
        // check source whitelist
        printf("[handlePublishData] the endpoint not in whitelist of target,"
                " be rejected, source ep id: %d, target ep id: %d\n",
                source_ep->Id(), target_ep->Id());
        is_allowed = false;
    } else if (msg_type > 0) {
        if (target_ep->IsRejectedMessage(msg_type)) {
            // check message type blacklist
            printf("[handlePublishData] the message type(%d) in blacklist of target,"
                    " be rejected, source ep id: %d, target ep id: %d\n",
                    msg_type, source_ep->Id(), target_ep->Id());
            is_allowed = false;
        } else if (! target_ep->IsSubscribedMessage(msg_type)) {
            // check message type whitelist
            printf("[handlePublishData] the message type(%d) not in whitelist of target,"
                    " be rejected, source ep id: %d, target ep id: %d\n",
                    msg_type, source_ep->Id(), target_ep->Id());
            is_allowed = false;
        }
    }
    return is_allowed;
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
    EndpointId ep_id = generate_random_integer();
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
    // XXX: clear endpoints here? or clear them in SwitchServer::OnConnectionClosed?
    ep->Connection()->Disconnect(); // XXX: delay 1 second to do this?
}
