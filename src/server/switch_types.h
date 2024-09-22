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

#endif  // _SWITCH_TYPES_H
