#include "endpoint_role.h"

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
