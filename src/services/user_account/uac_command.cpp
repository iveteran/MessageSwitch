#include "uac_command.h"
#include <sstream>

string listCommands()
{
    std::stringstream ss;
    ss << "NONE: " << (MessageId)UACCommand::NONE << "\n";
    ss << "BASE_INFO: " << (MessageId)UACCommand::BASE_INFO;
    return ss.str();
}
