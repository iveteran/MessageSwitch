#ifndef _SC_OPTIONS_H_
#define _SC_OPTIONS_H_

#include <string>
#include <memory>
#include <sstream>

#define N_A "N/A"

using std::string;

struct SCOptions
{
    string      server_host;
    uint16_t    server_port;
    uint32_t    endpoint_id;
    string      access_code;
    uint8_t     role = 0;               // EEndpointRole::{Normal, Service, Admin};
    uint16_t    svc_type;               // if role is Service, 0: serve all service
    bool        enable_console = false;
    string      console_sub_prompt;
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
        ss << "svc_type: " << svc_type << ", ";
        ss << "enable_console: " << enable_console << ", ";
        ss << "console_sub_prompt: " << console_sub_prompt << ", ";
        ss << "logfile: " << logfile << ", ";
        ss << "config_file: " << config_file << ", ";
        ss << "}";
        return ss.str();
    }

    int ParseFromArgumentsOrConfigFile(int argc, char* argv[],
            const char* progrom_version = N_A,
            const char* program_build_date = N_A,
            const char* program_copyright = N_A);
    int ParseArguments(int argc, char *argv[],
            const char* progrom_version = N_A,
            const char* program_build_date = N_A,
            const char* program_copyright = N_A);
    int ParseConfiguration(const string& config_file);
};
using SCOptionsPtr = std::shared_ptr<SCOptions>;

#endif  // _SC_OPTIONS_H_
