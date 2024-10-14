#include "switch_client.h"
#include "uac_message_handlers.h"
#include "uac_console_commands.h"
#include "uac_options.h"
#include "uac_storer.h"
#include "sc_options.h"

int main(int argc, char **argv)
{
  UACOptions uac_options;
  SCOptions options;
  options.AddSubOptions(&uac_options);

  options.ParseFromArgumentsOrConfigFile(argc, argv);

  std::cout << "> Options: " << options.ToString() << std::endl;
  std::cout << "> User Account Options: " << uac_options.ToString() << std::endl;

  SwitchClient sc(&options);

  SignalHandler sh(SignalEvent::INT,
          [&](SignalHandler* sh, uint32_t signo) {
            printf("Shutdown\n");
            // clean and exit
            sc.Stop();
          });

  auto uac_db_uri = uac_options.db_uri;
  UACStorer ua_storer(uac_db_uri.c_str());

  UACMessageHandlers msg_handlers(sc.GetCommandHandler(), &ua_storer);
  msg_handlers.SetupHandlers();

  UACConsoleCommands console_cmds(sc.GetConsole(), &uac_options, &ua_storer);
  console_cmds.RegisterCommands();

  sc.Start();

  return 0;
}
