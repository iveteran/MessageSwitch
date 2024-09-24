#include <sstream>
#include <iomanip>  // for std::put_time
#include "sc_options.h"
#include "sc_context.h"
#include "switch_client.h"

SCContext::SCContext(SwitchClient* client) :
    switch_client(client), born_time(evt_loop::Now()), is_registered(false), endpoint_id(0)
{
    auto options = switch_client->GetOptions();
    if (! options->access_code.empty()) {
        access_code = options->access_code;
    }
    if (! options->role.empty()) {
        role = TagToEndpointRole(options->role);
    }
}

string SCContext::ToString() const
{
    auto msg_hdr_desc = switch_client->GetMessageHeaderDescription();
    std::stringstream ss;
    ss << "{";
    ss << "born_time: " << std::put_time(localtime(&born_time), "%Y-%m-%d %I:%M:%S") << ", ";
    ss << "endpoint_id: " << endpoint_id << ", ";
    ss << "is_connected: " << (switch_client->IsConnected() ? "true" : "false") << ", ";
    ss << "access_code: " << access_code << ", ";
    ss << "role: " << EndpointRoleToTag(role) << ", ";
    ss << "is_registered: " << is_registered << ", ";
    ss << "token: " << token << ", ";
    ss << "message_header_description: " << (msg_hdr_desc ? msg_hdr_desc->ToString() : "") << ", ";
    ss << "}";
    return ss.str();
}
