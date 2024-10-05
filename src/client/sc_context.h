#ifndef _SC_CONTEXT_H
#define _SC_CONTEXT_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include "endpoint_role.h"
#include "switch_types.h"

using std::map;
using std::set;
using std::vector;
using std::string;

class SwitchClient;

struct SCContext
{
    SwitchClient*   switch_client;

    time_t born_time;
    string access_code;
    EEndpointRole role;
    ServiceType svc_type;
    bool is_registered;
    string register_errmsg;
    EndpointId endpoint_id;
    string token;

    set<EndpointId> fwd_targets;
    set<EndpointId> subs_sources;
    set<EndpointId> rej_sources;
    set<MessageId> subs_messages;
    set<MessageId> rej_messages;

    SCContext(SwitchClient* server);
    string ToString() const;

    void SetForwardTargets(const vector<EndpointId>& targets);
    void RemoveForwardTargets(const vector<EndpointId>& targets);
    void SetSubscribedSources(const vector<EndpointId>& sources);
    void RemoveSubscribedSources(const vector<EndpointId>& sources);
    void SetRejectedSources(const vector<EndpointId>& sources);
    void RemoveRejectedSources(const vector<EndpointId>& sources);
    void SetSubscribedMessages(const vector<MessageId>& messages);
    void RemoveSubscribedMessages(const vector<MessageId>& messages);
    void SetRejectedMessages(const vector<MessageId>& messages);
    void RemoveRejectedMessages(const vector<MessageId>& messages);
};
typedef std::shared_ptr<SCContext> SCContextPtr;

#endif  // _SC_CONTEXT_H
