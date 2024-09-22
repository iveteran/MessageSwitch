#ifndef _SWITCH_ENDPOINT_H
#define _SWITCH_ENDPOINT_H

#include <vector>
#include <set>
#include <memory>
#include <eventloop/tcp_connection.h>
#include "switch_message.h"
#include "endpoint_role.h"

using std::vector;
using std::set;
using std::shared_ptr;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

typedef uint32_t EndpointId;

class Endpoint {
public:
    Endpoint(EndpointId id, TcpConnection* conn) : conn_(conn)
    {
        conn_->SetID(id);
    }

    EndpointId Id() const { return conn_->ID(); }
    void SetId(EndpointId id) { conn_->SetID(id); }

    EEndpointRole GetRole() const { return role_; }
    void SetRole(EEndpointRole mode) { role_ = mode; }

    TcpConnection* Connection() { return conn_; }

    const vector<EndpointId>& GetForwardTargets() const
    {
        return fwd_targets_;
    }
    void SetForwardTargets(const vector<EndpointId>& targets)
    {
        fwd_targets_ = targets;
    }

private:
    EEndpointRole       role_;
    TcpConnection*      conn_;
    vector<EndpointId>  fwd_targets_;

    set<EndpointId>     subs_sources_;      // subscribed sources
    set<EndpointId>     rej_sources_;       // rejected sources
    set<ECommand>       subs_messages_;     // subscribed messages
    set<ECommand>       rej_messages_;      // rejected messages

    //map<SessionID, EndpointId> sess_sources_;
};
typedef std::shared_ptr<Endpoint> EndpointPtr;

#endif // _SWITCH_ENDPOINT_H
