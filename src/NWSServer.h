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

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "NWSClient.h"
#include "NWSLogger.h"
#include "NWSFrame.h"

namespace nws {

  class NWSServer {
  
    uint16_t port;
    struct epoll_event event;
    struct epoll_event *events = NULL;
    int sockfd = -1;
    int epfd = -1;
    unsigned int maxevents;
    std::map<int, NWSClient *> clients;

    SSL_CTX *ctx;
    string cert;
    string key;
  
    public:
      NWSServer(uint16_t port, unsigned int maxevents, const string *sslcert = nullptr, const string *sslkey = nullptr);
      ~NWSServer();
  
      int init();
      int eventLoop();

    private:
      void acceptClient();      
      void removeClient(int sockcl); 
      void readClient(int sockcl);
      bool isSSL();
      bool initSSL();
  };

}

#endif 
