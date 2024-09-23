#include <sstream>
#include <iomanip>  // for std::put_time
#include "sc_options.h"
#include "sc_context.h"
#include "switch_client.h"

SCContext::SCContext(SwitchClient* client) :
    switch_client(client), born_time(evt_loop::Now())
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
    std::stringstream ss;
    ss << "{";
    ss << "born_time: " << std::put_time(localtime(&born_time), "%Y-%m-%d %I:%M:%S") << ", ";
    ss << "endpoint_id: " << switch_client->ID() << ", ";
    ss << "is_connected: " << switch_client->IsConnected() << ", ";
    ss << "access_code: " << access_code << ", ";
    ss << "role: " << EndpointRoleToTag(role) << ", ";
    ss << "message_header_description: " << switch_client->GetMessageHeaderDescription()->ToString() << ", ";
    ss << "}";
    return ss.str();
}
