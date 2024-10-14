#ifndef _UAC_OPTIONS_H
#define _UAC_OPTIONS_H

#include <string>
#include "sc_options.h"

using std::string;

struct UACOptions : public SCOptionsBase
{
    string db_driver;
    string db_uri;

    string ToString() const;
    int ParseConfiguration(const toml::value& config);

    void AddArguments(argparse::ArgumentParser& program);
    void ReadArguments(argparse::ArgumentParser& program);
};

#endif  // _UAC_OPTIONS_H
