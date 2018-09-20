#ifndef __NWSFrame__
#define __NWSFrame__

#include <string>
#include <iostream>

using namespace std;

class NWSFrame {
  public:
  	enum Opcode {Continuation, Text, Binary, Close, Ping, Pong, Other};

    bool fin;
    unsigned char opcode;
    bool mask;
    uint64_t len;
    char maskKey[4] = {0};
    char *data = nullptr;
  
  public:
    NWSFrame(const char *buf);
    ~NWSFrame();

    void print();
};

#endif

