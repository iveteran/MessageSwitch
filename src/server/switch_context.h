#ifndef _SWITCH_CONTEXT_H
#define _SWITCH_CONTEXT_H

#include <map>
#include <string>
#include "switch_endpoint.h"
#include "switch_types.h"

#define DEFAULT_ACCESS_TOKEN "Hello World"
#define DEFAULT_ADMIN_TOKEN "Foobar2000"
#define DEFAULT_SERVICE_ACCESS_TOKEN "GOE works"

using std::map;
using std::string;

class SwitchServer;

struct SwitchContext
{
    SwitchServer*                   switch_server;
    map<int, TcpConnection*>        pending_clients;
    map<EndpointId, EndpointPtr>    endpoints;
    map<EndpointId, EndpointPtr>    normal_endpoints;
    map<EndpointId, EndpointPtr>    admin_endpoints;
    map<ServiceType, set<EndpointPtr>>  service_endpoints;
    //map<EndpointId, EndpointPtr>    proxy_endpoints;
    //map<EndpointId, EndpointPtr>    rproxy_endpoints;

    map<MessageId, set<EndpointId>>    message_subscribers;

    time_t born_time;
    string access_code = DEFAULT_ACCESS_TOKEN;
    string admin_code = DEFAULT_ADMIN_TOKEN;
    string service_access_code = DEFAULT_SERVICE_ACCESS_TOKEN;
    EServingMode serving_mode;

    SwitchContext(SwitchServer* server);
    string ToString() const;

    void RemoveEndpoint(EndpointId ep_id);
};
typedef std::shared_ptr<SwitchContext> SwitchContextPtr;

#endif  // _SWITCH_CONTEXT_H
