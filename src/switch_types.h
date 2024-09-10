#ifndef _SWITCH_TYPES_H
#define _SWITCH_TYPES_H

enum class EServingMode {
    Undefined,
    Normal,
    Proxy,
    RProxy,
    ClusterNode,
};
const char* ServingModeToTag(EServingMode mode);
EServingMode TagToServingMode(const std::string& mode_str);

enum class EEndpointRole {
    Undefined,
    Endpoint,
    Admin,
};
const char* EndpointRoleToTag(EEndpointRole role);
EEndpointRole TagToEndpointRole(const std::string& role_str);

enum class EProxyMode {
    Undefined,
    Random,
    RoundRobin,
    Hash,
};

#endif  // _SWITCH_TYPES_H
