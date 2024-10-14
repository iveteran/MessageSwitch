#ifndef _UAC_MESSAGE_HANDLERS_H
#define _UAC_MESSAGE_HANDLERS_H

#include <string>

using std::string;

class SCCommandHandler;
class ServiceMessage;
class UACStorer;

class UACMessageHandlers {
public:
    UACMessageHandlers(SCCommandHandler* cmd_handler, UACStorer* uac_storer)
        : cmd_handler_(cmd_handler), uac_storer_(uac_storer)
    {}

    void SetupHandlers();

protected:
    std::pair<int, string> OnServiceCommand(const ServiceMessage* svc_msg, const char* data, size_t data_size);

private:
    SCCommandHandler* cmd_handler_;
    UACStorer* uac_storer_;
};

#endif  // _UAC_MESSAGE_HANDLERS_H
