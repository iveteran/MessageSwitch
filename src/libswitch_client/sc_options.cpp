#include "sc_options.h"
#include <sstream>
#include <argparse/argparse.hpp>
#include <toml.hpp>
#include <iostream>

using std::cout;
using std::endl;

string SCOptions::ToString() const {
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

void SCOptions::AddSubOptions(SCOptionsBase* sub_options)
{
    sub_options_list_.push_back(sub_options);
}

int SCOptions::ParseFromArgumentsOrConfigFile(int argc, char* argv[],
        const char* progrom_version,
        const char* program_build_date,
        const char* program_copyright)
{
  int status = ParseArguments(argc, argv,
          progrom_version,
          program_build_date,
          program_copyright);
  if (status != 0) {
      return -1;
  }

  if (! config_file.empty()) {
      if (access(config_file.c_str(), F_OK) == -1) {
          fprintf(stderr, "Error: the configuration file does not exist: %s\n", config_file.c_str());
          return 1;
      }
      int status = ParseConfiguration(config_file);
      if (status != 0) {
          return -1;
      }
  }

  return 0;
}

int SCOptions::ParseArguments(int argc, char *argv[],
        const char* progrom_version,
        const char* program_build_date,
        const char* program_copyright)
{
    argparse::ArgumentParser program(argv[0], progrom_version);

    program.add_description("Github: https://github.com/iveteran/MessageSwitch");

    char version_info[256];
    snprintf(version_info, sizeof(version_info), "Version: %s, build date: %s\nCopyright: %s",
            progrom_version, program_build_date, program_copyright);
    program.add_epilog(version_info);

    program.add_argument("-H", "--server_host")
        .help("listen host")
        .default_value("127.0.0.1");
    program.add_argument("-p", "--server_port")
        .help("listen port")
        .default_value(10000)
        .scan<'i', int>();
    program.add_argument("-n", "--endpoint_id")
        .help("endpoint id for Switch")
        .default_value(0)
        .scan<'i', int>();
    program.add_argument("-r", "--role")
        .help("endpoint role, 0: undefined, 1: normal, 2: admin, 3: service")
        .default_value(1)
        .scan<'i', int>();
    program.add_argument("-t", "--svc_type")
        .help("service type, if role is service, 0: undefined")
        .default_value(0)
        .scan<'i', int>();
    program.add_argument("-c", "--enable_console")
        .help("enable console")
        .flag();
    program.add_argument("-t", "--console_sub_prompt")
        .help("sub prompt of console")
        .default_value("");
    program.add_argument("-l", "--logfile")
        .help("log file, default is STDOUT");
    program.add_argument("-a", "--access_code")
        .help("access code for endpoint");
    program.add_argument("-f", "--config")
        .help("configuration file");

    for (auto sub_options : sub_options_list_) {
        sub_options->AddArguments(program);
    }

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return -1;
    }

    server_host = program.get<std::string>("--server_host");
    cout << "> arguments.server_host: " << server_host << endl;
    server_port = program.get<int>("--server_port");
    cout << "> arguments.server_port: " << server_port << endl;
    endpoint_id = program.get<int>("--endpoint_id");
    cout << "> arguments.endpoint_id: " << endpoint_id << endl;
    role = program.get<int>("--role");
    cout << "> arguments.role: " << role << endl;
    svc_type = program.get<int>("--svc_type");
    cout << "> arguments.svc_type: " << svc_type << endl;

    if (program.is_used("--enable_console")) {
        enable_console = true;
    }
    cout << "> arguments.enable_console: " << enable_console << endl;

    console_sub_prompt = program.get<std::string>("--console_sub_prompt");
    cout << "> arguments.console_sub_prompt: " << console_sub_prompt << endl;

    if (program.is_used("--access_code")) {
        access_code = program.get<std::string>("--access_code");
    }
    cout << "> arguments.access_code: " << access_code << endl;
    if (program.is_used("--logfile")) {
        logfile = program.get<std::string>("--logfile");
    }
    cout << "> arguments.logfile: " << logfile << endl;
    if (program.is_used("--config")) {
        config_file = program.get<std::string>("--config");
    }
    cout << "> arguments.config: " << config_file << endl;

    for (auto sub_options : sub_options_list_) {
        sub_options->ReadArguments(program);
    }

    return 0;
}

int SCOptions::ParseConfiguration(const string& _config_file)
{
    const toml::value config = toml::parse(_config_file);

    if (config.contains("logfile")) {
        logfile = config.at("logfile").as_string();
        cout << "> config.logfile: " << logfile << endl;
    }

    if (config.contains("server")) {
        auto server_config = config.at("server");

        if (server_config.contains("host")) {
            server_host = server_config.at("host").as_string();
            cout << "> config.server.host: " << server_host << endl;
        }

        if (server_config.contains("port")) {
            server_port = server_config.at("port").as_integer();
            cout << "> config.server.port: " << server_port << endl;
        }
    }
    if (config.contains("client")) {
        auto client_config = config.at("client");

        if (client_config.contains("endpoint_id")) {
            endpoint_id = client_config.at("endpoint_id").as_integer();
            cout << "> config.client.endpoint_id: " << endpoint_id << endl;
        }

        if (client_config.contains("role")) {
            role = client_config.at("role").as_integer();
            cout << "> config.client.mode: " << role << endl;
        }

        if (client_config.contains("svc_type")) {
            svc_type = client_config.at("svc_type").as_integer();
            cout << "> config.client.svc_type: " << svc_type << endl;
        }

        if (client_config.contains("access_code")) {
            access_code = client_config.at("access_code").as_string();
            cout << "> config.client.access_code: " << access_code << endl;
        }

        if (client_config.contains("enable_console")) {
            enable_console = client_config.at("enable_console").as_boolean();
            cout << "> config.client.enable_console: " << enable_console << endl;
        }

        if (client_config.contains("console_sub_prompt")) {
            console_sub_prompt = client_config.at("console_sub_prompt").as_string();
            cout << "> config.client.console_sub_prompt: " << console_sub_prompt << endl;
        }
    }

    for (auto sub_options : sub_options_list_) {
        sub_options->ParseConfiguration(config);
    }

    return 0;
}
