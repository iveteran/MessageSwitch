#ifndef _SWITCH_CONTEXT_H
#define _SWITCH_CONTEXT_H

#include <map>
#include <string>
#include "switch_endpoint.h"

#define DEFAULT_ACCESS_TOKEN "Hello World"
#define DEFAULT_ADMIN_TOKEN "Foobar2000"

using std::map;
using std::string;

class SwitchServer;

struct SwitchContext
{
    SwitchContext(SwitchServer* server) : switch_server(server) {}

    SwitchServer*                   switch_server;
    map<int, TcpConnection*>        register_pending_client;
    map<EndpointId, EndpointPtr>    endpoints;
    map<EndpointId, EndpointPtr>    proxy_endpoints;
    map<EndpointId, EndpointPtr>    rproxy_endpoints;

    string  access_token = DEFAULT_ACCESS_TOKEN;
    string  admin_token = DEFAULT_ADMIN_TOKEN;
};
typedef std::shared_ptr<SwitchContext> SwitchContextPtr;

#endif  // _SWITCH_CONTEXT_H
