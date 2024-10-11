#include "demo_options.h"
#include <argparse/argparse.hpp>
#include <toml.hpp>
#include <iostream>

using std::cout;
using std::endl;

string DemoOptions::ToString() const {
    std::stringstream ss;
    ss << "{";
    ss << "dummy_field: " << dummy_field << ", ";
    // Add more options here
    ss << "}";
    return ss.str();
}

int DemoOptions::ParseConfiguration(const toml::value& config)
{
    if (config.contains("demo")) {
        auto demo_config = config.at("demo");

        if (demo_config.contains("dummy_field")) {
            dummy_field = demo_config.at("dummy_field").as_integer();
            cout << "> config.demo.dummy_field: " << dummy_field << endl;
        }

        // Add more configuration items here
    }
    return 0;
}

void DemoOptions::AddArguments(argparse::ArgumentParser& program)
{
    program.add_argument("-x", "--dummy_field")
        .help("dummy field for demo")
        .default_value(0)
        .scan<'i', int>();

    // Add more arguments here
}

void DemoOptions::ReadArguments(argparse::ArgumentParser& program)
{
    dummy_field = program.get<int>("--dummy_field");
    cout << "> arguments.dummy_field: " << dummy_field << endl;

    // Read more arguments here
}
