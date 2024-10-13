#include "ua_command.h"
#include <sstream>

string listCommands()
{
    std::stringstream ss;
    ss << "NONE: " << (MessageId)UACommand::NONE << "\n";
    ss << "LOGIN: " << (MessageId)UACommand::LOGIN << "\n";
    ss << "LOGOUT: " << (MessageId)UACommand::LOGOUT << "\n";
    ss << "VERIFY: " << (MessageId)UACommand::VERIFY << "\n";
    ss << "CREATE: " << (MessageId)UACommand::CREATE << "\n";
    ss << "DELETE: " << (MessageId)UACommand::DELETE;
    return ss.str();
}
