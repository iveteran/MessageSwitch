#include "switch_message.h"

const char* CommandToTag(ECommand cmd) {
    const char* cmd_tag = "UNDEFINED";
    switch (cmd) {
        case ECommand::ECHO:
            cmd_tag = "ECHO";
            break;
        case ECommand::REG:
            cmd_tag = "REG";
            break;
        case ECommand::FWD:
            cmd_tag = "FWD";
            break;
        case ECommand::DATA:
            cmd_tag = "DATA";
            break;
        case ECommand::INFO:
            cmd_tag = "INFO";
            break;
        case ECommand::SETUP:
            cmd_tag = "SETUP";
            break;
        case ECommand::PROXY:
            cmd_tag = "PROXY";
            break;
        case ECommand::KICKOUT:
            cmd_tag = "KICKOUT";
            break;
        case ECommand::EXIT:
            cmd_tag = "EXIT";
            break;
        case ECommand::RELOAD:
            cmd_tag = "RELOAD";
            break;
        case ECommand::RESULT:
            cmd_tag = "RESULT";
            break;
        default:
            cmd_tag = "UNDEFINED";
            break;
    }
    return cmd_tag;
}
