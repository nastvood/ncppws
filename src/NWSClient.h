#ifndef __NWSClient__
#define __NWSClient__

#include <vector>
#include <map>
#include <iostream>

#include "wshelper.h" 
#include "wscrypto.h" 

class NWSClient {
  public:
  	enum State {AwaitingHandshake, HandshakeResponse, Connected};

	private:
	  int sockfd = -1;
  	std::vector<char> buf;
	  bool isDoneData = false;
  	std::map<std::string, std::string> header;
	  State state;

  public:
    NWSClient(int sfd);
    ~NWSClient();

    void addData(char *data, int len);
    void setIsDone(bool isDone);
		void setState(State s);
    string genKey();
    char *data(); 

    string response();

  protected:    
    void parseHeader();

};

#endif

