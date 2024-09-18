#ifndef _SWITCH_CONTEXT_H
#define _SWITCH_CONTEXT_H

#include <map>
#include <string>
#include "switch_endpoint.h"
#include "switch_types.h"

#define DEFAULT_ACCESS_TOKEN "Hello World"
#define DEFAULT_ADMIN_TOKEN "Foobar2000"

using std::map;
using std::string;

class SwitchServer;

struct SwitchContext
{
    SwitchServer*                   switch_server;
    map<int, TcpConnection*>        pending_clients;
    map<EndpointId, EndpointPtr>    endpoints;
    map<EndpointId, EndpointPtr>    admin_clients;
    map<EndpointId, EndpointPtr>    proxy_endpoints;
    map<EndpointId, EndpointPtr>    rproxy_endpoints;

    time_t born_time;
    string access_code = DEFAULT_ACCESS_TOKEN;
    string admin_code = DEFAULT_ADMIN_TOKEN;
    EServingMode serving_mode;

    SwitchContext(SwitchServer* server);
    string ToString() const;
};
typedef std::shared_ptr<SwitchContext> SwitchContextPtr;

#endif  // _SWITCH_CONTEXT_H
