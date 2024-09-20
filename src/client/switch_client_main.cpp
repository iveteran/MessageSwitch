#include "switch_client.h"

int main(int argc, char **argv) {
  SwitchClient switch_client;

  SignalHandler sh(SignalEvent::INT, [&](SignalHandler* sh, uint32_t signo) {
          printf("Shutdown\n");
          switch_client.Cleanup();
          EV_Singleton->StopLoop();
          });

  EV_Singleton->StartLoop();

  return 0;
}
