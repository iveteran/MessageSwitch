#ifndef _SWITCH_TYPES_H
#define _SWITCH_TYPES_H

#include <string>

enum class EServingMode {
    Undefined,
    Normal,
    Proxy,
    RProxy,
    ClusterNode,
};
const char* ServingModeToTag(EServingMode mode);
EServingMode TagToServingMode(const std::string& mode_str);

enum class EProxyMode {
    Undefined,
    Random,
    RoundRobin,
    Hash,
};

using EndpointId = uint32_t;
using ServiceType = uint8_t;
using MessageId = uint8_t;

#endif  // _SWITCH_TYPES_H
