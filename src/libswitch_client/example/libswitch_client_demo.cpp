#include "switch_client.h"
#include "demo_message_handlers.h"
#include "demo_console_commands.h"
#include "demo_options.h"
#include "sc_options.h"

int main(int argc, char **argv)
{
  DemoOptions demo_options;
  SCOptions options;
  options.AddSubOptions(&demo_options);

  options.ParseFromArgumentsOrConfigFile(argc, argv);

  std::cout << "> Options: " << options.ToString() << std::endl;
  std::cout << "> Demo Options: " << demo_options.ToString() << std::endl;

  SwitchClient sc(&options);

  SignalHandler sh(SignalEvent::INT,
          [&](SignalHandler* sh, uint32_t signo) {
            printf("Shutdown\n");
            // clean and exit
            sc.Stop();
          });

  DemoMessageHandlers msg_handlers(sc.GetCommandHandler());
  msg_handlers.SetupHandlers();

  DemoConsoleCommands console_cmds(sc.GetConsole(), &demo_options);
  console_cmds.RegisterCommands();

  sc.Start();

  return 0;
}
