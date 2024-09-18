#include "argparse/argparse.hpp"
#include "toml.hpp"
#include "switch_server.h"
#include "switch_options.h"

int parse_arguments(int argc, char **argv, OptionsPtr& options) {
    argparse::ArgumentParser program("switch");

    program.add_argument("-H", "--host")
        .help("listen host")
        .default_value("127.0.0.1");
    program.add_argument("-p", "--port")
        .help("listen port")
        .default_value(10000)
        .scan<'i', int>();
    program.add_argument("-n", "--node_id")
        .help("node id for cluster")
        .default_value(1)
        .scan<'i', int>();
    program.add_argument("-m", "--mode")
        .help("serving mode")
        .default_value("normal");
    program.add_argument("-l", "--logfile")
        .help("log file, default is STDOUT");
    program.add_argument("-a", "--access_code")
        .help("access code for endpoint");
    program.add_argument("-A", "--admin_code")
        .help("access code for administrator");
    program.add_argument("-f", "--config")
        .help("configuration file");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return -1;
    }

    options->host = program.get<std::string>("--host");
    cout << "> arguments.host: " << options->host << endl;
    options->port = program.get<int>("--port");
    cout << "> arguments.port: " << options->port << endl;
    options->node_id = program.get<int>("--node_id");
    cout << "> arguments.node_id: " << options->node_id << endl;
    options->serving_mode = program.get<std::string>("--mode");
    cout << "> arguments.mode: " << options->serving_mode << endl;
    if (program.is_used("--logfile")) {
        options->logfile = program.get<std::string>("--logfile");
        cout << "> arguments.logfile: " << options->logfile << endl;
    }
    if (program.is_used("--access_code")) {
        options->access_code = program.get<std::string>("--access_code");
        cout << "> arguments.access_code: " << options->access_code << endl;
    }
    if (program.is_used("--admin_code")) {
        options->admin_code = program.get<std::string>("--admin_code");
        cout << "> arguments.admin_code: " << options->admin_code << endl;
    }
    if (program.is_used("--config")) {
        options->config_file = program.get<std::string>("--config");
        cout << "> arguments.config: " << options->config_file << endl;
    }

    return 0;
}

int parse_configuration(const string& config_file, OptionsPtr& options) {
    const toml::value config = toml::parse(config_file);

    if (config.contains("server")) {
        auto server_config = config.at("server");

        if (server_config.contains("host")) {
            auto host = server_config.at("host").as_string();
            cout << "> config.server.host: " << host << endl;
            options->host = host;
        }

        if (server_config.contains("port")) {
            auto port = server_config.at("port").as_integer();
            cout << "> config.server.port: " << port << endl;
            options->port = port;
        }

        if (server_config.contains("node_id")) {
            auto node_id = server_config.at("node_id").as_integer();
            cout << "> config.server.node_id: " << node_id << endl;
            options->node_id = node_id;
        }

        if (server_config.contains("mode")) {
            auto serving_mode = server_config.at("mode").as_string();
            cout << "> config.server.mode: " << serving_mode << endl;
            options->serving_mode = serving_mode;
        }

        if (server_config.contains("logfile")) {
            auto logfile = server_config.at("logfile").as_string();
            cout << "> config.server.logfile: " << logfile << endl;
            if (! logfile.empty()) {
                options->logfile = logfile;
            }
        }
    }
    if (config.contains("auth")) {
        auto auth_config = config.at("auth");

        if (auth_config.contains("access_code")) {
            auto access_code = auth_config.at("access_code").as_string();
            cout << "> config.auth.access_code: " << access_code << endl;
            options->access_code = access_code;
        }

        if (auth_config.contains("admin_code")) {
            auto admin_code = auth_config.at("admin_code").as_string();
            cout << "> config.auth.admin_code: " << admin_code << endl;
            options->admin_code = admin_code;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
  auto options = std::make_shared<Options>();

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

  SwitchServer switch_server(options);
  SignalHandler sh(SignalEvent::INT, std::bind(&SwitchServer::OnSignal, &switch_server, std::placeholders::_1, std::placeholders::_2));

  EV_Singleton->StartLoop();

  return 0;
}
