#include "NWSClient.h"

using namespace std;

NWSClient::NWSClient(int sfd):sockfd(sfd) {
};

NWSClient::~NWSClient() { 
  printf("Client descriptor");
};

void NWSClient::addData(char *data, int len) {
  if (this->isDoneData) {
    this->buf.clear();
  }

  buf.insert(buf.end(), &data[0], &data[len]);
  
  this->isDoneData = false;       
};

char *NWSClient::data() {
  return &buf[0];
}

void NWSClient::parseHeader() {
  if (this->state == AwaitingHandshake) {
    auto p = split(this->data(), "\r\n\r\n"); 
    auto headers = nsplit(p.first, "\r\n");

    if (headers.size() == 1) {
      //FIXME разбор Request-Line 
    } else if(headers.size() > 1) {
      //FIXME разбор Request-Line 
      for(auto h = headers.begin() + 1; h != headers.end(); ++h) {
        auto kv = split(*h, ":"); 
        string k = lower(trim(kv.first));
        string v = trim(kv.second);
        if (!(k.empty() || (k == ""))) {
    			this->header.insert({k, v});
          cout<<k<<"///"<<v<<endl;
        }
      }
    }
  } else if (this->state == Connected) {
    NWSFrame *frame = new NWSFrame(this->data());   
    frame->print();

    delete frame;
  }
}

string NWSClient::genKey() {
  string sign = this->header["sec-websocket-key"] +  "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";  

	string sha = sha1(sign);

  cout<<sign<<";sha1"<<endl;
	string tmp = sha;
	for (char c : tmp) {
		printf("%02x", (unsigned short) (unsigned char)c);
	}
  cout<<endl;
    
  return base64Encode(sha);  
}

void NWSClient::setState(State s) {
	this->state = s;	
}

void NWSClient::setIsDone(bool isDone) {
  isDoneData = isDone;

  //cout<<"---------------"<<endl<<this->data()<<"-------------------"<<endl;

  if (isDoneData) {
    this->header.clear();
    this->parseHeader();
  }
}

string NWSClient::response() {
  if (this->state == AwaitingHandshake) {
    string resp = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " 
      + this->genKey() 
      + "\r\nUpgrade: websocket\r\n\r\n";

    return resp;          
  } else {
    return "";
  }
}
