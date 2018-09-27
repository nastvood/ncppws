#include "NWSServer.h"

using namespace nws;

NWSServer::NWSServer(uint16_t port, unsigned int maxevents) {
  this->port = htons(port);
  this->maxevents = maxevents;
}

NWSServer::~NWSServer() {
  if (this->epfd > -1) close(this->epfd);
  if (this->sockfd > -1) close(this->sockfd);  
  if (this->events != NULL) free(this->events);
}

int NWSServer::init() {  
  info()<<"init start";

  this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sockfd == -1) {
    error()<<"server socket";
    return errno;
  }

  int enable = 1;
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
    error()<<"setsockopt";
    return errno;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = this -> port;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(this->sockfd,  (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    error()<<"bind";
    return errno;
  }

  this->epfd = epoll_create1(0);
  if (this->epfd == -1) {
    error()<<"epoll_create";
    return errno;
  }

  listen(this->sockfd, 128);

  event.data.fd  = this->sockfd;
  event.events = EPOLLIN | EPOLLOUT | EPOLLET;
  if(epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->sockfd, &event) == -1) {
    error()<<"epoll_ctl";
    return errno;
  }

  events = (epoll_event *)calloc (this -> maxevents, sizeof event);

  info()<<"init finish";

  return 0;
}

int NWSServer::eventLoop() {
  if ((this->sockfd < 0) || (this->epfd < 0)) return -1;
  
  while(1) {
    info()<<"event loop";

    int n = epoll_wait(this->epfd, this->events, this->maxevents, -1);
    debug()<<"epoll_wait events count: "<<n;

    if (n == 0) continue;
    if (n == -1) return errno;

    for (int i = 0; i < n; ++i) {
      if ((this->events[i].events & EPOLLERR) || (this->events[i].events & EPOLLHUP) || !(this->events[i].events & EPOLLIN)){
        continue;
      } else if (this->sockfd == this->events[i].data.fd) {
        info()<<"server sock accept start";

        struct sockaddr inAddr;
        socklen_t inLen = sizeof inAddr;
        int sockcl = accept(this->sockfd, &inAddr, &inLen);
        if (sockcl == -1) {
          continue;
        }

        int flags = fcntl (sockcl, F_GETFL, 0);
        if (flags == -1) {
          perror("fnctl get");
          continue;
        }

        flags |= O_NONBLOCK;
        if (fcntl (sockcl, F_SETFL, flags) < 0) {
          perror("fnctl set");
          continue;
        }        

        char hbuf[NI_MAXHOST];
        char sbuf[NI_MAXSERV];  
        if (getnameinfo(&inAddr, inLen, hbuf, NI_MAXHOST, sbuf, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
          continue;
        }

        debug()<<"accept connection "<<sockcl<<" host "<<hbuf<<" port "<<sbuf;
        
        event.data.fd  = sockcl;
        event.events = EPOLLIN | EPOLLET;
        if(epoll_ctl(this->epfd, EPOLL_CTL_ADD, sockcl, &event) == -1) {
          perror("epoll_ctl client");
          return errno;
        }

        this->clients.insert({sockcl, new NWSClient(sockcl)});
        info()<<"server sock accept end";
      } else {
        info()<<"server sock data start";
        NWSClient *client = this->clients[this->events[i].data.fd];

        while(1) {
          char buf[512] = {0};

          ssize_t count = read(this->events[i].data.fd, buf, sizeof buf);
          if (count > 0) {
            client -> addData(buf, count);
          }

          if (((count == -1) && (errno == EAGAIN)) || (count == 0)) {
            client->setIsDone(true);

            if (client->getState() == NWSClient::AwaitingHandshake) {
              string resp = client->handshakeResponse();

						  debug()<<resp;

              ssize_t writeLen = write(this->events[i].data.fd, resp.c_str(), resp.size());
	  					if (writeLen > -1) {
		  					client->setState(NWSClient::Connected);
			  			}
            }

            break;
          }

          if (count == -1) {
            debug()<<"read "<<errno;
            break;
          }
        }
      }
    }
  }

  return 0;
}
