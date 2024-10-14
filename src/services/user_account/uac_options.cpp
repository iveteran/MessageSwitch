#include "uac_options.h"
#include <argparse/argparse.hpp>
#include <toml.hpp>
#include <iostream>

using std::cout;
using std::endl;

string UACOptions::ToString() const {
    std::stringstream ss;
    ss << "{";
    ss << "db_driver: " << db_driver << ", ";
    ss << "db_uri: " << db_uri << ", ";
    ss << "}";
    return ss.str();
}

int UACOptions::ParseConfiguration(const toml::value& config)
{
    if (config.contains("user_account")) {
        auto ua_config = config.at("user_account");

        if (ua_config.contains("database")) {
            auto db_config = ua_config.at("database");

            if (db_config.contains("driver")) {
                db_driver = db_config.at("driver").as_string();
                cout << "> ua_config.user_account.database.driver: " << db_driver << endl;
            }

            if (db_config.contains("uri")) {
                db_uri = db_config.at("uri").as_string();
                cout << "> ua_config.user_account.database.uri: " << db_uri << endl;
            }
        }
    }
    return 0;
}

void UACOptions::AddArguments(argparse::ArgumentParser& program)
{
    program.add_argument("--db_driver")
        .help("database driver");

    program.add_argument("--db_uri")
        .help("database users uri");
}

void UACOptions::ReadArguments(argparse::ArgumentParser& program)
{
    if (program.is_used("--db_driver")) {
        db_driver = program.get<std::string>("--db_driver");
        cout << "> arguments.db_driver: " << db_driver << endl;
    }

    if (program.is_used("--db_uri")) {
        db_uri = program.get<std::string>("--db_uri");
        cout << "> arguments.db_uri: " << db_uri << endl;
    }
}
