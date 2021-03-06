#ifndef __NWSFrame__
#define __NWSFrame__

#include <string>
#include <iostream>
#include <cstring>

#define MASK_KEY_LEN 4
#define CLOSE_DATA_LEN 2

namespace nws {

  class NWSFrame {
    public:
    	enum Opcode {Continuation, Text, Binary, Close, Ping, Pong, Other};
  
      bool fin = true;
      Opcode opcode;
      bool mask;
      uint64_t len;
      char maskKey[MASK_KEY_LEN] = {0};
      char *data = nullptr;
      uint16_t closeCode = 0;
  
    protected:
      unsigned char ucharOfOpcode(Opcode opcode);
      uint64_t packetSize();
    
    public:
      NWSFrame(const char *buf);
      NWSFrame(bool fin, Opcode opcode, bool mask, uint16_t len, char maskKey[4], const char *data, uint16_t closeCode);
      ~NWSFrame();
  
      size_t generatePacket(char **packet);
      static NWSFrame createClose(uint16_t closeCode);
      static NWSFrame createPong(char * data, uint16_t len);
      static std::string stringOfOpcode(Opcode opcode);
  
      friend std::ostream &operator<<(std::ostream &os, const NWSFrame &frame);
  };

}

#endif

