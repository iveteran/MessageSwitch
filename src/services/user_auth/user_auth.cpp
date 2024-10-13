#include "switch_client.h"
#include "ua_message_handlers.h"
#include "ua_console_commands.h"
#include "ua_options.h"
#include "ua_storer.h"
#include "sc_options.h"

int main(int argc, char **argv)
{
  UAOptions ua_options;
  SCOptions options;
  options.AddSubOptions(&ua_options);

  options.ParseFromArgumentsOrConfigFile(argc, argv);

  std::cout << "> Options: " << options.ToString() << std::endl;
  std::cout << "> User Agent Options: " << ua_options.ToString() << std::endl;

  SwitchClient sc(&options);

  SignalHandler sh(SignalEvent::INT,
          [&](SignalHandler* sh, uint32_t signo) {
            printf("Shutdown\n");
            // clean and exit
            sc.Stop();
          });

  auto ua_users_file = ua_options.db_users_uri;
  auto ua_sess_file = ua_options.db_sessions_uri;
  UAStorer ua_storer(ua_users_file.c_str(), ua_sess_file.c_str());
  //ua_storer.CreateUser("abc@example.com", "my_pass");

  UAMessageHandlers msg_handlers(sc.GetCommandHandler(), &ua_storer);
  msg_handlers.SetupHandlers();

  UAConsoleCommands console_cmds(sc.GetConsole(), &ua_options, &ua_storer);
  console_cmds.RegisterCommands();

  sc.Start();

  return 0;
}
