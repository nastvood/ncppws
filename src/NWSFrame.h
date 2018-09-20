#ifndef __NWSFrame__
#define __NWSFrame__

#include <string>
#include <iostream>

using namespace std;

class NWSFrame {
  public:
    bool fin;
    unsigned char opcode;
    bool mask;
    uint64_t len;
    char maskKey[4] = {0};
  
  public:
    NWSFrame(const char *buf);

    void print();
};

#endif

