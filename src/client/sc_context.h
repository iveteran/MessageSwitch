#ifndef _SC_CONTEXT_H
#define _SC_CONTEXT_H

#include <map>
#include <string>
#include <memory>
#include "endpoint_role.h"

using std::map;
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

    SCContext(SwitchClient* server);
    string ToString() const;
};
typedef std::shared_ptr<SCContext> SCContextPtr;

#endif  // _SC_CONTEXT_H
