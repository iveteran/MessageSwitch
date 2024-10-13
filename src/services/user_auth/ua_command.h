#ifndef _UA_COMMAND_H
#define _UA_COMMAND_H

#include "switch_types.h"

#include <string>
using std::string;

enum class UACommand : MessageId {
    NONE,
    LOGIN,
    LOGOUT,
    VERIFY,
    CREATE,
    DELETE,
    Count,
};

string listCommands();

#endif  // _UA_COMMAND_H
