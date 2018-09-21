#ifndef __NWSFrame__
#define __NWSFrame__

#include <string>
#include <iostream>
#include <cstring>

using namespace std;

#define MASK_KEY_LEN 4
#define CLOSE_DATA_LEN 2

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
    void print();

    static NWSFrame createClose(uint16_t closeCode);
    static string stringOfOpcode(Opcode opcode);

};


#endif

