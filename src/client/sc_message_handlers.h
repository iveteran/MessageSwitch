#ifndef _SC_MESSAGE_HANDLERS_H
#define _SC_MESSAGE_HANDLERS_H

#include <string>

using std::string;

class SCCommandHandler;
class ResultMessage;
class PublishingMessage;
class ServiceMessage;

class SCMessageHandlers {
public:
    SCMessageHandlers(SCCommandHandler* cmd_handler)
        : cmd_handler_(cmd_handler)
    {}

    void SetupHandlers();

protected:
    void OnPublishingData(const PublishingMessage* pub_msg, const char* data, size_t data_len);
    void OnPublishingResult(const ResultMessage* result_msg, const char* data, size_t data_len);
    std::pair<int, string> OnServiceCommand(const ServiceMessage* svc_msg, const char* data, size_t data_size);

private:
    SCCommandHandler* cmd_handler_;
};

#endif  // _SC_MESSAGE_HANDLERS_H
