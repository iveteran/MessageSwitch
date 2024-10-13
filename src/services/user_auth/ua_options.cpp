#include "ua_options.h"
#include <argparse/argparse.hpp>
#include <toml.hpp>
#include <iostream>

using std::cout;
using std::endl;

string UAOptions::ToString() const {
    std::stringstream ss;
    ss << "{";
    ss << "db_driver: " << db_driver << ", ";
    ss << "db_users_uri: " << db_users_uri << ", ";
    ss << "db_sessions_uri: " << db_sessions_uri << ", ";
    ss << "}";
    return ss.str();
}

int UAOptions::ParseConfiguration(const toml::value& config)
{
    if (config.contains("user_auth")) {
        auto ua_config = config.at("user_auth");

        if (ua_config.contains("database")) {
            auto db_config = ua_config.at("database");

            if (db_config.contains("driver")) {
                db_driver = db_config.at("driver").as_string();
                cout << "> ua_config.user_auth.database.driver: " << db_driver << endl;
            }

            if (db_config.contains("users_uri")) {
                db_users_uri = db_config.at("users_uri").as_string();
                cout << "> ua_config.user_auth.database.users_uri: " << db_users_uri << endl;
            }

            if (db_config.contains("sessions_uri")) {
                db_sessions_uri = db_config.at("sessions_uri").as_string();
                cout << "> ua_config.user_auth.database.sessions_uri: " << db_sessions_uri << endl;
            }
        }
    }
    return 0;
}

void UAOptions::AddArguments(argparse::ArgumentParser& program)
{
    program.add_argument("--db_driver")
        .help("database driver");

    program.add_argument("--db_users_uri")
        .help("database users uri");

    program.add_argument("--db_sessions_uri")
        .help("database users sessions uri");
}

void UAOptions::ReadArguments(argparse::ArgumentParser& program)
{
    if (program.is_used("--db_driver")) {
        db_driver = program.get<std::string>("--db_driver");
        cout << "> arguments.db_driver: " << db_driver << endl;
    }

    if (program.is_used("--db_users_uri")) {
        db_users_uri = program.get<std::string>("--db_users_uri");
        cout << "> arguments.db_users_uri: " << db_users_uri << endl;
    }

    if (program.is_used("--db_sessions_uri")) {
        db_sessions_uri = program.get<std::string>("--db_sessions_uri");
        cout << "> arguments.db_sessions_uri: " << db_sessions_uri << endl;
    }
}
