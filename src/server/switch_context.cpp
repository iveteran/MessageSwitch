#include <sstream>
#include <iomanip>  // for std::put_time
#include "switch_context.h"
#include "switch_server.h"

SwitchContext::SwitchContext(SwitchServer* server) :
    switch_server(server), born_time(evt_loop::Now())
{
    auto options = switch_server->GetOptions();
    if (! options->access_code.empty()) {
        access_code = options->access_code;
    }
    if (! options->admin_code.empty()) {
        admin_code = options->admin_code;
    }
    if (! options->service_access_code.empty()) {
        service_access_code = options->service_access_code;
    }
    if (! options->serving_mode.empty()) {
        serving_mode = TagToServingMode(options->serving_mode);
    }
}

string SwitchContext::ToString() const
{
    std::stringstream ss;
    ss << "{";
    ss << "born_time: " << std::put_time(localtime(&born_time), "%Y-%m-%d %I:%M:%S") << ", ";
    ss << "node_id: " << switch_server->NodeId() << ", ";
    ss << "access_code: " << access_code << ", ";
    ss << "admin_code: " << admin_code << ", ";
    ss << "service_access_code: " << service_access_code << ", ";
    ss << "serving_mode: " << ServingModeToTag(serving_mode) << ", ";
    ss << "message_header_description: " << switch_server->GetMessageHeaderDescription()->ToString() << ", ";
    ss << "}";
    return ss.str();
}

void SwitchContext::RemoveEndpoint(EndpointId ep_id)
{
    auto iter = endpoints.find(ep_id);
    if (iter == endpoints.end()) {
        return;
    }
    auto ep = iter->second;
    switch (ep->GetRole()) {
        case EEndpointRole::Normal:
            normal_endpoints.erase(ep->Id());
            break;
        case EEndpointRole::Admin:
            admin_endpoints.erase(ep->Id());
            break;
        case EEndpointRole::Service:
            {
                auto iter = service_endpoints.find(ep->GetServiceType());
                if (iter != service_endpoints.end()) {
                    iter->second.erase(ep);
                }
            }
            break;
        default:
            break;
    }
    endpoints.erase(iter);
}
