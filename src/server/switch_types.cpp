#include <cstddef>
#include <string>
#include "switch_types.h"

const char* ServingModeToTag(EServingMode mode) {
    const char* mode_str = "undefined";
    switch (mode) {
        case EServingMode::Normal:
            mode_str = "normal";
            break;
        case EServingMode::Proxy:
            mode_str = "proxy";
            break;
        case EServingMode::RProxy:
            mode_str = "rproxy";
            break;
        default:
            break;
    }
    return mode_str;
}

EServingMode TagToServingMode(const std::string& mode_str) {
    EServingMode mode = EServingMode::Undefined;
    if (mode_str == "normal") {
        mode = EServingMode::Normal;
    } else if (mode_str == "proxy") {
        mode = EServingMode::Proxy;
    } else if (mode_str == "rproxy") {
        mode = EServingMode::RProxy;
    }
    return mode;
}

const char* EndpointRoleToTag(EEndpointRole role) {
    const char* role_str = NULL;
    switch (role) {
        case EEndpointRole::Endpoint:
            role_str = "endpoint";
            break;
        case EEndpointRole::Admin:
            role_str = "admin";
            break;
        case EEndpointRole::Service:
            role_str = "service";
            break;
        default:
            role_str = "undefined";
            break;
    }
    return role_str;
}

EEndpointRole TagToEndpointRole(const std::string& role_str) {
    EEndpointRole role = EEndpointRole::Undefined;
    if (role_str == "endpoint") {
        role = EEndpointRole::Endpoint;
    } else if (role_str == "admin") {
        role = EEndpointRole::Admin;
    } else if (role_str == "service") {
        role = EEndpointRole::Service;
    }
    return role;
}
