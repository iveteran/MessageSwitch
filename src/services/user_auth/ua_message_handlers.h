#ifndef _UA_MESSAGE_HANDLERS_H
#define _UA_MESSAGE_HANDLERS_H

#include <string>

using std::string;

class SCCommandHandler;
class ServiceMessage;
class UAStorer;

class UAMessageHandlers {
public:
    UAMessageHandlers(SCCommandHandler* cmd_handler, UAStorer* ua_storer)
        : cmd_handler_(cmd_handler), ua_storer_(ua_storer)
    {}

    void SetupHandlers();

protected:
    std::pair<int, string> OnServiceCommand(const ServiceMessage* svc_msg, const char* data, size_t data_size);

private:
    SCCommandHandler* cmd_handler_;
    UAStorer* ua_storer_;
};

#endif  // _UA_MESSAGE_HANDLERS_H
