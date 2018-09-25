#ifndef __NWSServer__
#define __NWSServer__

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <map>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>

#include "NWSClient.h"
#include "NWSLogger.h"

namespace nws {

  class NWSServer {
  
    uint16_t port;
    struct epoll_event event;
    struct epoll_event *events = NULL;
    int sockfd = -1;
    int epfd = -1;
    unsigned int maxevents;
    std::map<int, NWSClient *> clients;
  
    public:
      NWSServer(uint16_t port, unsigned int maxevents);
      ~NWSServer();
  
      int init();
      int eventLoop();
  };

}

#endif 
