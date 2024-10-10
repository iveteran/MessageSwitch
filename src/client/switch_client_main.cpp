#include "switch_client.h"
#include "sc_message_handlers.h"
#include "sc_options.h"
#include "version.h"

int main(int argc, char **argv) {
  SCOptions options;
  options.ParseFromArgumentsOrConfigFile(argc, argv,
          APP_VERSION, APP_BUILD_DATE, APP_COPYRIGHT);
  std::cout << "> Options: " << options.ToString() << std::endl;

  SwitchClient sc(&options);

  SignalHandler sh(SignalEvent::INT,
          [&](SignalHandler* sh, uint32_t signo) {
            printf("Shutdown\n");
            // clean and exit
            sc.Stop();
          });

  SCMessageHandlers msg_handlers(sc.GetCommandHandler());
  msg_handlers.SetupHandlers();

  sc.Start();

  return 0;
}
