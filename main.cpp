#include "src/NWSClient.h"
#include "src/NWSServer.h"

int main() {

  nws::NWSServer *server = new nws::NWSServer(8080, 64);
  server->init();
  server->eventLoop();

  return 0;
}
