#ifndef _SWITCH_TYPES_H
#define _SWITCH_TYPES_H

enum EServingMode {
    Switch,
    Proxy,
    RProxy,
};

enum EProxyMode {
    Random,
    RoundRobin,
    Hash,
};

#endif  // _SWITCH_TYPES_H
