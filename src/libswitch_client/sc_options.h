#ifndef _SC_OPTIONS_H_
#define _SC_OPTIONS_H_

#include <string>
#include <memory>
#include <vector>
#include <toml_fwd.hpp>

#define N_A "N/A"

using std::string;

namespace argparse {
    class ArgumentParser;
}

struct SCOptionsBase
{
    virtual string ToString() const = 0;
    virtual int ParseConfiguration(const toml::value& config) = 0;

    virtual void AddArguments(argparse::ArgumentParser& program) = 0;
    virtual void ReadArguments(argparse::ArgumentParser& program) = 0;
};

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
    string ToString() const;

    int ParseFromArgumentsOrConfigFile(int argc, char* argv[],
            const char* progrom_version = N_A,
            const char* program_build_date = N_A,
            const char* program_copyright = N_A);
    int ParseArguments(int argc, char *argv[],
            const char* progrom_version = N_A,
            const char* program_build_date = N_A,
            const char* program_copyright = N_A);
    int ParseConfiguration(const string& config_file);

    void AddSubOptions(SCOptionsBase* sub_options);

    private:
    std::vector<SCOptionsBase*> sub_options_list_;
};
using SCOptionsPtr = std::shared_ptr<SCOptions>;

#endif  // _SC_OPTIONS_H_
