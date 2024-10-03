#include <sstream>
#include <iomanip>  // for std::put_time
#include <iterator>
#include "sc_options.h"
#include "sc_context.h"
#include "switch_client.h"

SCContext::SCContext(SwitchClient* client) :
    switch_client(client), born_time(evt_loop::Now()), svc_type(0), is_registered(false), endpoint_id(0)
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
    ss << "svc_type: " << int(svc_type) << ", ";
    ss << "is_registered: " << is_registered << ", ";
    ss << "register_errmsg: " << register_errmsg << ", ";
    ss << "token: " << token << ", ";
    ss << "message_header_description: " << (msg_hdr_desc ? msg_hdr_desc->ToString() : "") << ", ";
    ss << "fwd_targets: [";
    std::copy(fwd_targets.begin(), fwd_targets.end(), std::ostream_iterator<uint32_t>(ss, ","));
    ss << "], ";
    ss << "subs_sources: [";
    std::copy(subs_sources.begin(), subs_sources.end(), std::ostream_iterator<uint32_t>(ss, ","));
    ss << "], ";
    ss << "rej_sources: [";
    std::copy(rej_sources.begin(), rej_sources.end(), std::ostream_iterator<uint32_t>(ss, ","));
    ss << "], ";
    ss << "subs_messages: [";
    std::copy(subs_messages.begin(), subs_messages.end(), std::ostream_iterator<int>(ss, ","));  // MUST use int to print uint8_t number
    ss << "], ";
    ss << "rej_messages: [";
    std::copy(rej_messages.begin(), rej_messages.end(), std::ostream_iterator<int>(ss, ","));  // MUST use int to print uint8_t number
    ss << "], ";
    ss << "}";
    return ss.str();
}

void SCContext::SetForwardTargets(const vector<uint32_t>& targets)
{
    fwd_targets.insert(targets.begin(), targets.end());
}
void SCContext::RemoveForwardTargets(const vector<uint32_t>& targets)
{
    for (auto elem : targets) {
        fwd_targets.erase(elem);
    }
}

void SCContext::SetSubscribedSources(const vector<uint32_t>& sources)
{
    subs_sources.insert(sources.begin(), sources.end());
}
void SCContext::RemoveSubscribedSources(const vector<uint32_t>& sources)
{
    for (auto elem : sources) {
        subs_sources.erase(elem);
    }
}

void SCContext::SetRejectedSources(const vector<uint32_t>& sources)
{
    rej_sources.insert(sources.begin(), sources.end());
}
void SCContext::RemoveRejectedSources(const vector<uint32_t>& sources)
{
    for (auto elem : sources) {
        rej_sources.erase(elem);
    }
}

void SCContext::SetSubscribedMessages(const vector<uint8_t>& messages)
{
    subs_messages.insert(messages.begin(), messages.end());
}
void SCContext::RemoveSubscribedMessages(const vector<uint8_t>& messages)
{
    for (auto elem : messages) {
        subs_messages.erase(elem);
    }
}

void SCContext::SetRejectedMessages(const vector<uint8_t>& messages)
{
    rej_messages.insert(messages.begin(), messages.end());
}
void SCContext::RemoveRejectedMessages(const vector<uint8_t>& messages)
{
    for (auto elem : messages) {
        rej_messages.erase(elem);
    }
}
