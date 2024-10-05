#ifndef _SWITCH_TYPES_H
#define _SWITCH_TYPES_H

#include <string>

enum class EServingMode : uint8_t {
    Undefined,
    Normal,
    Proxy,
    RProxy,
    ClusterNode,
};
const char* ServingModeToTag(EServingMode mode);
EServingMode TagToServingMode(const std::string& mode_str);

enum class EProxyMode : uint8_t {
    Undefined,
    Random,
    RoundRobin,
    Hash,
};

using ep_id_t = uint32_t;
using EndpointId = ep_id_t;

using role_id_t = uint8_t;
using RoleId = role_id_t;

using svc_type_t = uint8_t;
using ServiceType = svc_type_t;

using msg_type_t = uint16_t;
using MessageId = msg_type_t;

#endif  // _SWITCH_TYPES_H
