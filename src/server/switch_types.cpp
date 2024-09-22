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
