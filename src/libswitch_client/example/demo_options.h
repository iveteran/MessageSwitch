#ifndef _DEMO_OPTIONS_H
#define _DEMO_OPTIONS_H

#include <string>
#include "sc_options.h"

using std::string;

struct DemoOptions : public SCOptionsBase
{
    int dummy_field;

    // Add more fields here

    string ToString() const;
    int ParseConfiguration(const toml::value& config);

    void AddArguments(argparse::ArgumentParser& program);
    void ReadArguments(argparse::ArgumentParser& program);
};

#endif  // _DEMO_OPTIONS_H
