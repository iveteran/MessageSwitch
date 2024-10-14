#ifndef _UAC_COMMAND_H
#define _UAC_COMMAND_H

#include "switch_types.h"

#include <string>
using std::string;

enum class UACCommand : MessageId {
    NONE,
    BASE_INFO,
    Count,
};

string listCommands();

#endif  // _UAC_COMMAND_H
