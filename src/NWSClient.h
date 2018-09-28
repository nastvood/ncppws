#ifndef __NWSClient__
#define __NWSClient__

#include <vector>
#include <map>
#include <iostream>

#include "wshelper.h" 
#include "wscrypto.h" 
#include "NWSFrame.h"
#include "NWSLogger.h"

namespace nws {

  class NWSClient {
    public:
    	enum State {AwaitingHandshake, HandshakeResponse, Connected, ClientClosed, ClientPing};
  
  	private:
  	  int sockfd = -1;
    	std::vector<char> buf;
  	  bool isDoneData = false;
    	std::map<string, string> header;
  
      std::map<string, string> param;
      std::string requestUri;
      std::string host;  
  	  State state;
      NWSFrame *lastFrame = nullptr;
  
    public:
      NWSClient(int sfd);
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
  
    protected:    
      void parseHeader();
  
  };

}

#endif
