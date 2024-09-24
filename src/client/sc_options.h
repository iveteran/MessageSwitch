#ifndef _SC_OPTIONS_H_
#define _SC_OPTIONS_H_

#include <string>
#include <memory>
#include <sstream>

#define DEFAULT_ACCESS_TOKEN "Hello World"
#define DEFAULT_ROLE "endpoint"

using std::string;

struct SCOptions
{
    string      server_host;
    uint16_t    server_port;
    uint32_t    endpoint_id;
    string      access_code = DEFAULT_ACCESS_TOKEN;
    string      role = DEFAULT_ROLE;
    string      logfile;
    string      config_file;

    SCOptions() : server_port(0), endpoint_id(0) {}
    string ToString() const {
        std::stringstream ss;
        ss << "{";
        ss << "server_host: " << server_host << ", ";
        ss << "server_port: " << server_port << ", ";
        ss << "endpoint_id: " << endpoint_id << ", ";
        ss << "access_code: " << access_code << ", ";
        ss << "role: " << role << ", ";
        ss << "logfile: " << logfile << ", ";
        ss << "config_file: " << config_file << ", ";
        ss << "}";
        return ss.str();
    }
};
using SCOptionsPtr = std::shared_ptr<SCOptions>;

#endif  // _SC_OPTIONS_H_
