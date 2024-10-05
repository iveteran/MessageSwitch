#include "switch_endpoint.h"
#include "eventloop/eventloop.h"

Endpoint::Endpoint(EndpointId id, TcpConnection* conn)
    : role_(EEndpointRole::Undefined), conn_(conn), born_time_(evt_loop::Now()), svc_type_(0)
{
    conn_->SetID(id);
}

void Endpoint::SetForwardTargets(const vector<EndpointId>& targets)
{
    fwd_targets_.insert(targets.begin(), targets.end());
}
void Endpoint::UnsetForwardTargets(const vector<EndpointId>& targets)
{
    for (auto elem : targets) {
        fwd_targets_.erase(elem);
    }
}

void Endpoint::SubscribeSources(const vector<EndpointId>& sources)
{
    subs_sources_.insert(sources.begin(), sources.end());
}
void Endpoint::UnsubscribeSources(const vector<EndpointId>& sources)
{
    for (auto elem : sources) {
        subs_sources_.erase(elem);
    }
}
void Endpoint::RejectSources(const vector<EndpointId>& sources)
{
    rej_sources_.insert(sources.begin(), sources.end());
}
void Endpoint::UnrejectSources(const vector<EndpointId>& sources)
{
    for (auto elem : sources) {
        rej_sources_.erase(elem);
    }
}

bool Endpoint::IsSubscribedSource(EndpointId src_ep_id) const
{
    if (! subs_sources_.empty()) {
        // 如果有白名单，不在白名单的拒绝
        auto iter = subs_sources_.find(src_ep_id);
        if (iter == subs_sources_.end()) {
            return false;
        }
    }
    return true;
}
bool Endpoint::IsRejectedSource(EndpointId src_ep_id) const
{
    if (! rej_sources_.empty()) {
        // 如果有黑名单，在黑名单的拒绝
        auto iter = rej_sources_.find(src_ep_id);
        if (iter != rej_sources_.end()) {
            return true;
        }
    }
    return false;
}

void Endpoint::SubscribeMessages(const vector<MessageId>& messages)
{
    subs_messages_.insert(messages.begin(), messages.end());
}
void Endpoint::UnsubscribeMessages(const vector<MessageId>& messages)
{
    for (auto elem : messages) {
        subs_messages_.erase(elem);
    }
}
void Endpoint::RejectMessages(const vector<MessageId>& messages)
{
    rej_messages_.insert(messages.begin(), messages.end());
}
void Endpoint::UnrejectMessages(const vector<MessageId>& messages)
{
    for (auto elem : messages) {
        rej_messages_.erase(elem);
    }
}
bool Endpoint::IsSubscribedMessage(MessageId msg_id) const
{
    if (! subs_messages_.empty()) {
        // 如果有白名单，不在白名单的拒绝
        auto iter = subs_messages_.find(msg_id);
        if (iter == subs_messages_.end()) {
            return false;
        }
    }
    return true;
}
bool Endpoint::IsRejectedMessage(MessageId msg_id) const
{
    if (! rej_messages_.empty()) {
        // 如果有黑名单，在黑名单的拒绝
        auto iter = rej_messages_.find(msg_id);
        if (iter != rej_messages_.end()) {
            return true;
        }
    }
    return false;
}
