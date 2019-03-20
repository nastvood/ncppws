#ifndef __NWSClient__
#define __NWSClient__

#include <vector>
#include <map>
#include <iostream>

#include <unistd.h>
#include <sys/socket.h> 
#include <openssl/ssl.h>

#include "wshelper.h" 
#include "wscrypto.h" 
#include "NWSFrame.h"
#include "NWSLogger.h"

namespace nws {

  class NWSClient {
    public:
    	enum State {AwaitingHandshake, HandshakeResponse, Connected, ClientClosed, ClientPing};
  
  	private:
    	std::vector<char> buf;
  	  bool isDoneData = false;
  	  State state;
      NWSFrame *lastFrame = nullptr;

    public:
  	  int sockfd = -1;
      SSL *ssl = nullptr;
      std::string requestUri;
      std::string host;           
    	std::map<string, string> header;  
      std::map<string, string> param;
  
    public:
      NWSClient(int sfd, SSL *ssl = nullptr);
      ~NWSClient();
  
      void addData(char *data, int len);
      void setIsDone(bool isDone);
  		void setState(State s);
      std::string genKey();
  
      std::string handshakeResponse();
      std::string closeResponse();
      std::string pongResponse();
  
      const State getState();
      const char *data(); 

      static std::string stringOfState(State state);

      friend std::ostream &operator<<(std::ostream &os, const NWSClient &client);
      int write(const std::string &s);
      int read(void *buf, int num);

    protected:    
      void parseHeader();
  
  };

}

#endif
