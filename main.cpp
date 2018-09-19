#include "src/NWSClient.h"
#include "src/NWSServer.h"

int main() {

  NWSServer *server = new NWSServer(8080, 64);
  server->init();
  server->eventLoop();

  return 0;
}
