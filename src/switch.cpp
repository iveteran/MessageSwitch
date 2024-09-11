#include "argparse/argparse.hpp"
#include "switch_server.h"
#include "switch_options.h"

OptionsPtr parse_arguments(int argc, char **argv) {
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
    program.add_argument("-a", "--access_code")
        .help("access code for endpoint");
    program.add_argument("-A", "--admin_code")
        .help("access code for administrator");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return nullptr;
    }

    auto options = std::make_shared<Options>();
    options->host = program.get<std::string>("--host");
    options->port = program.get<int>("--port");
    options->node_id = program.get<int>("--node_id");
    options->serving_mode = program.get<std::string>("--mode");
    if (program.is_used("--access_code")) {
        options->access_code = program.get<std::string>("--access_code");
    }
    if (program.is_used("--admin_code")) {
        options->admin_code = program.get<std::string>("--admin_code");
    }

    return options;
}

int main(int argc, char **argv) {
  auto options = parse_arguments(argc, argv);
  if (! options) {
      return 1;
  }
  std::cout << "Options: " << options->ToString() << std::endl;

  SwitchServer switch_server(options);
  SignalHandler sh(SignalEvent::INT, std::bind(&SwitchServer::OnSignal, &switch_server, std::placeholders::_1, std::placeholders::_2));

  EV_Singleton->StartLoop();

  return 0;
}
