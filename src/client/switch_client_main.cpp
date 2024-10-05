#include "switch_client.h"
#include "argparse/argparse.hpp"
#include "toml.hpp"
#include "sc_options.h"
#include "version.h"

int parse_arguments(int argc, char **argv, SCOptionsPtr& options) {
    argparse::ArgumentParser program("switch_client", APP_VERSION);

    program.add_description("Github: https://github.com/iveteran/MessageSwitch");

    char version_info[256];
    snprintf(version_info, sizeof(version_info), "Version: %s, build date: %s\nCopyright: %s",
            APP_VERSION, APP_BUILD_DATE, APP_COPYRIGHT);
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
        .scan<'i', EndpointId>();
    program.add_argument("-r", "--role")
        .help("endpoint role")
        .default_value("normal");
    program.add_argument("-l", "--logfile")
        .help("log file, default is STDOUT");
    program.add_argument("-a", "--access_code")
        .help("access code for endpoint");
    program.add_argument("-f", "--config")
        .help("configuration file");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return -1;
    }

    options->server_host = program.get<std::string>("--server_host");
    cout << "> arguments.server_host: " << options->server_host << endl;
    options->server_port = program.get<int>("--server_port");
    cout << "> arguments.server_port: " << options->server_port << endl;
    options->endpoint_id = program.get<EndpointId>("--endpoint_id");
    cout << "> arguments.endpoint_id: " << options->endpoint_id << endl;
    options->role = program.get<std::string>("--role");
    cout << "> arguments.role: " << options->role << endl;

    if (program.is_used("--access_code")) {
        options->access_code = program.get<std::string>("--access_code");
        cout << "> arguments.access_code: " << options->access_code << endl;
    }
    if (program.is_used("--logfile")) {
        options->logfile = program.get<std::string>("--logfile");
        cout << "> arguments.logfile: " << options->logfile << endl;
    }
    if (program.is_used("--config")) {
        options->config_file = program.get<std::string>("--config");
        cout << "> arguments.config: " << options->config_file << endl;
    }

    return 0;
}

int parse_configuration(const string& config_file, SCOptionsPtr& options) {
    const toml::value config = toml::parse(config_file);

    if (config.contains("logfile")) {
        auto logfile = config.at("logfile").as_string();
        cout << "> config.logfile: " << logfile << endl;
        if (! logfile.empty()) {
            options->logfile = logfile;
        }
    }

    if (config.contains("server")) {
        auto server_config = config.at("server");

        if (server_config.contains("host")) {
            auto host = server_config.at("host").as_string();
            cout << "> config.server.host: " << host << endl;
            options->server_host = host;
        }

        if (server_config.contains("port")) {
            auto port = server_config.at("port").as_integer();
            cout << "> config.server.port: " << port << endl;
            options->server_port = port;
        }
    }
    if (config.contains("client")) {
        auto client_config = config.at("client");

        if (client_config.contains("endpoint_id")) {
            auto endpoint_id = client_config.at("endpoint_id").as_integer();
            cout << "> config.client.endpoint_id: " << endpoint_id << endl;
            options->endpoint_id = endpoint_id;
        }

        if (client_config.contains("role")) {
            auto role = client_config.at("role").as_string();
            cout << "> config.client.mode: " << role << endl;
            options->role = role;
        }

        if (client_config.contains("access_code")) {
            auto access_code = client_config.at("access_code").as_string();
            cout << "> config.client.access_code: " << access_code << endl;
            options->access_code = access_code;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
  auto options = std::make_shared<SCOptions>();

  int status = parse_arguments(argc, argv, options);
  if (status != 0) {
      return 1;
  }

  if (! options->config_file.empty()) {
      if (access(options->config_file.c_str(), F_OK) == -1) {
          fprintf(stderr, "Error: the configuration file does not exist: %s\n", options->config_file.c_str());
          return 1;
      }
      int status = parse_configuration(options->config_file, options);
      if (status != 0) {
          return 1;
      }
  }

  std::cout << "> Options: " << options->ToString() << std::endl;

  SwitchClient switch_client(options.get());

  SignalHandler sh(SignalEvent::INT,
          [&](SignalHandler* sh, uint32_t signo) {
            printf("Shutdown\n");
            // clean and exit
            switch_client.Exit();
          });

  EV_Singleton->StartLoop();

  return 0;
}
