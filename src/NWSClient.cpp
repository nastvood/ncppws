#include "NWSClient.h"

using namespace std;

namespace nws {

  NWSClient::NWSClient(int sfd, SSL *ssl):sockfd(sfd),ssl(ssl) {
    this->state = AwaitingHandshake;
  };
  
  NWSClient::~NWSClient() { 
    if (this->lastFrame) {
      delete lastFrame;
    }

    if (this->ssl) SSL_free(this->ssl);
  
    debug()<<"Client descriptor";
  };
  
  void NWSClient::addData(char *data, int len) {
    if (this->isDoneData) {
      this->buf.clear();
    }
  
    buf.insert(buf.end(), &data[0], &data[len]);
    
    this->isDoneData = false;       
  };
  
  const char *NWSClient::data() {
    return &buf[0];
  }

  int NWSClient::write(const std::string &s) {
    if (this->ssl) {
      return  SSL_write(this->ssl, s.c_str(), s.size());
    } else {
      return ::write(this->sockfd, s.c_str(), s.size());
    }
  }

  int NWSClient::read(void *buf, int num) {
    if (this->ssl) {
      return  SSL_read(this->ssl, buf, num);
    } else {
      return ::read(this->sockfd, buf, num);
    }
  }
  
  void NWSClient::parseHeader() {
    if (this->state == AwaitingHandshake) {
      auto p = split(this->data(), "\r\n\r\n"); 
      auto headers = nsplit(p.first, "\r\n");
  
      if(headers.size() > 1) {
        auto params = nsplit(headers[0], " ");
  
        if (params.size() > 1) {
          this->requestUri = params[1];
  
          if (this->requestUri != "*") {
            auto p = split(this->requestUri, "?");
            this->host = p.first;
  
            auto parts = nsplit(p.second, "&");
            for(auto &part: parts) {
              auto p = split(part, "=");
  
      			  this->param.insert({p.first, urlDecode(p.second)});
            }
          } 
        }
  
        for(auto h = headers.begin() + 1; h != headers.end(); ++h) {
          auto kv = split(*h, ":"); 
          string k = lower(trim(kv.first));
          string v = trim(kv.second);
          if (!(k.empty() || (k == ""))) {
      			this->header.insert({k, v});
            //debug()<<"   "<<k<<"///"<<v;
          }
        }
      } else {
        //FIXME
      }
    } else if (this->state == Connected) {
      this->lastFrame = new NWSFrame(this->data());
  
      if (lastFrame->opcode == NWSFrame::Close) {
        this->setState(ClientClosed);
      }
  
      debug()<<*this->lastFrame;
    }
  }
  
  string NWSClient::genKey() {
    string sign = this->header["sec-websocket-key"] +  "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";  
  
  	string sha = sha1(sign);
  
  	string tmp = sha;
  	for (char c : tmp) {
  		printf("%02x", (unsigned short) (unsigned char)c);
  	}
      
    return base64Encode(sha);  
  }
  
  void NWSClient::setState(State s) {
  	this->state = s;	
  }
  
  void NWSClient::setIsDone(bool isDone) {
    isDoneData = isDone;
  
    if (isDoneData) {
      this->header.clear();
      this->parseHeader();
    }
  }
  
  string NWSClient::handshakeResponse() {
    if (this->state == AwaitingHandshake) {
      string resp = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " 
        + this->genKey() 
        + "\r\nUpgrade: websocket\r\n\r\n";
  
      return resp;          
    } else {
      return "";
    }
  }
  
  string NWSClient::closeResponse() {
    if ((this->state == ClientClosed) && (this->lastFrame)) {
       NWSFrame frame = NWSFrame::createClose(this->lastFrame->closeCode); 
       
       char *packet = nullptr; 
       size_t len = frame.generatePacket(&packet);
  
       NWSFrame f = NWSFrame(packet);
  
       return string(packet, len);
    } else {
      return "";
    }
  }
  
  string NWSClient::pongResponse() {
    if ((this->state == ClientPing) && (this->lastFrame)) {
       NWSFrame frame = NWSFrame::createPong(this->lastFrame->data, this->lastFrame->len); 
       
       char *packet = nullptr; 
       size_t len = frame.generatePacket(&packet);
  
       //FIXME
  
       return string(packet, len);
    } else {
      return "";
    }
  }
  
  const NWSClient::State NWSClient::getState() {
    return this->state; 
  }
  
  string NWSClient::stringOfState(State state) {
    switch (state) {
      case AwaitingHandshake:
        return "AwaitingHandshake";
  
      case HandshakeResponse:
        return "HandshakeResponse";
  
      case Connected:
        return "Connected";
  
      case ClientClosed:
        return "ClientClosed";
  
      case ClientPing:
        return "ClientPing";
  
      default:
        return "Other state";
    } 
  }
  
  ostream &operator<<(std::ostream &out, const NWSClient &client) {
    out << "Client {" 
      << "sockfd " << client.sockfd
      << "; host: " << client.host 
      <<"; requestUri: " << client.requestUri;

    out <<"; param [";
      for(const auto &p: client.param) {
        out << p.first << ":" << p.second << "; ";
      }
    out << "]";   

    out <<"; header [";
      for(const auto &p: client.header) {
        out << p.first << ":" << p.second << "; ";
      }
    out << "]";   
    
    out << " }"; 
    
    return out;
  }
}
