#include "switch_server.h"

int main(int argc, char **argv) {
  SwitchServer switch_server;
  SignalHandler sh(SignalEvent::INT, std::bind(&SwitchServer::OnSignal, &switch_server, std::placeholders::_1, std::placeholders::_2));

  EV_Singleton->StartLoop();

  return 0;
}
