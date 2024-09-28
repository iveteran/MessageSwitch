#ifndef _SC_CONTEXT_H
#define _SC_CONTEXT_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include "endpoint_role.h"

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
    bool is_registered;
    uint32_t endpoint_id;
    string token;

    set<uint32_t> fwd_targets;
    set<uint32_t> subs_sources;
    set<uint32_t> rej_sources;
    set<uint8_t> subs_messages;
    set<uint8_t> rej_messages;

    SCContext(SwitchClient* server);
    string ToString() const;

    void SetForwardTargets(const vector<uint32_t>& targets);
    void RemoveForwardTargets(const vector<uint32_t>& targets);
    void SetSubscribedSources(const vector<uint32_t>& sources);
    void RemoveSubscribedSources(const vector<uint32_t>& sources);
    void SetRejectedSources(const vector<uint32_t>& sources);
    void RemoveRejectedSources(const vector<uint32_t>& sources);
    void SetSubscribedMessages(const vector<uint8_t>& messages);
    void RemoveSubscribedMessages(const vector<uint8_t>& messages);
    void SetRejectedMessages(const vector<uint8_t>& messages);
    void RemoveRejectedMessages(const vector<uint8_t>& messages);
};
typedef std::shared_ptr<SCContext> SCContextPtr;

#endif  // _SC_CONTEXT_H
