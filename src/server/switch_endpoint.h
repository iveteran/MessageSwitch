#ifndef _SWITCH_ENDPOINT_H
#define _SWITCH_ENDPOINT_H

#include <vector>
#include <set>
#include <memory>
#include <eventloop/tcp_connection.h>
#include "switch_message.h"
#include "endpoint_role.h"
#include "switch_types.h"

using std::vector;
using std::set;
using std::shared_ptr;

namespace evt_loop {
    class TcpConnection;
}
using evt_loop::TcpConnection;

class Endpoint {
public:
    Endpoint(EndpointId id, TcpConnection* conn);

    EndpointId Id() const { return conn_->ID(); }
    void SetId(EndpointId id) { conn_->SetID(id); }

    EEndpointRole GetRole() const { return role_; }
    void SetRole(EEndpointRole mode) { role_ = mode; }
    string GetToken() const { return token_; }
    void SetToken(const string& token) { token_ = token; }

    TcpConnection* Connection() { return conn_; }
    void SetConnection(TcpConnection* conn) { conn_ = conn; }
    time_t GetBornTime() const { return born_time_; }
    void SetServiceType(uint8_t svc_type) { svc_type_ = svc_type; }
    uint8_t GetServiceType() const { return svc_type_; }

    const set<EndpointId>& GetForwardTargets() const { return fwd_targets_; }
    void SetForwardTargets(const vector<EndpointId>& targets);
    void UnsetForwardTargets(const vector<EndpointId>& targets);

    void SubscribeSources(const vector<EndpointId>& sources);
    void UnsubscribeSources(const vector<EndpointId>& sources);
    void RejectSources(const vector<EndpointId>& sources);
    void UnrejectSources(const vector<EndpointId>& sources);
    const set<EndpointId>& GetSubscriedSources() const { return subs_sources_; }
    const set<EndpointId>& GetRejectedSources() const { return rej_sources_; }
    bool IsSubscribedSource(EndpointId src_ep_id) const;
    bool IsRejectedSource(EndpointId src_ep_id) const;

    void SubscribeMessages(const vector<MessageId>& messages);
    void UnsubscribeMessages(const vector<MessageId>& messages);
    void RejectMessages(const vector<MessageId>& messages);
    void UnrejectMessages(const vector<MessageId>& messages);
    const set<MessageId>& GetSubscriedMessages() const { return subs_messages_; }
    const set<MessageId>& GetRejectedMessages() const { return rej_messages_; }
    bool IsSubscribedMessage(MessageId msg_id) const;
    bool IsRejectedMessage(MessageId msg_id) const;

private:
    EEndpointRole       role_;
    string              token_;
    TcpConnection*      conn_;
    time_t              born_time_;
    ServiceType         svc_type_;           // service type, if role is Service

    set<EndpointId>     fwd_targets_;

    set<EndpointId>     subs_sources_;      // subscribed sources
    set<EndpointId>     rej_sources_;       // rejected sources

    set<MessageId>      subs_messages_;     // subscribed messages
    set<MessageId>      rej_messages_;      // rejected messages

    //map<SessionID, EndpointId> sess_sources_;
};
typedef std::shared_ptr<Endpoint> EndpointPtr;

#endif // _SWITCH_ENDPOINT_H
