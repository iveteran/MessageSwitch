#ifndef _ENDPOINT_ROLE_H
#define _ENDPOINT_ROLE_H

#include <string>

enum class EEndpointRole : uint8_t {
    Undefined,
    Normal,
    Admin,
    Service,
    COUNT,
};
const char* EndpointRoleToTag(EEndpointRole role);
EEndpointRole TagToEndpointRole(const std::string& role_str);

#endif  // _ENDPOINT_ROLE_H
