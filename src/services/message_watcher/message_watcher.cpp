#include "switch_client.h"
#include "mw_message_handlers.h"
#include "mw_console_commands.h"
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

  MWMessageHandlers msg_handlers(sc.GetCommandHandler());
  msg_handlers.SetupHandlers();

  MWConsoleCommands console_cmds(sc.GetConsole());
  console_cmds.RegisterCommands();

  sc.Start();

  return 0;
}
