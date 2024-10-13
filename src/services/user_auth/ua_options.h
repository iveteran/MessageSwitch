#ifndef _UA_OPTIONS_H
#define _UA_OPTIONS_H

#include <string>
#include "sc_options.h"

using std::string;

struct UAOptions : public SCOptionsBase
{
    string db_driver;
    string db_users_uri;
    string db_sessions_uri;

    string ToString() const;
    int ParseConfiguration(const toml::value& config);

    void AddArguments(argparse::ArgumentParser& program);
    void ReadArguments(argparse::ArgumentParser& program);
};

#endif  // _UA_OPTIONS_H
