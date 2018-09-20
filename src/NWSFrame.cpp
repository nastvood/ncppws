#include "NWSFrame.h"

NWSFrame::NWSFrame(const char *buf) {
  uint64_t offset = 0;

  char c = buf[offset++];  
  this->fin = c >> 7;
  this->opcode = c & 0x0F;

  c = buf[offset++];
  this->mask = c >> 7;
  unsigned char l = c & 0x7F;

  if (l <= 125) {
    this->len = l;
  } else if (l == 126) {
    unsigned char h =  buf[offset++];
    unsigned char l =  buf[offset++];

    this->len = (h << 8) + l;

  } else {
    uint64_t w0 = buf[offset++];
    uint64_t w1 = buf[offset++];
    uint64_t w2 = buf[offset++];
    uint64_t w3 = buf[offset++];
    uint64_t w4 = buf[offset++];
    uint64_t w5 = buf[offset++];
    uint64_t w6 = buf[offset++];
    uint64_t w7 = buf[offset++];

    this->len = ((w0 << 56) & 0xFF00000000000000) + ((w1 << 48) & 0xFF000000000000) + ((w2 << 40) & 0xFF0000000000) + ((w3 << 32) & 0xFF00000000)
                + ((w4 << 24) & 0xFF000000) + ((w5 << 16) & 0xFF0000) + ((w6 << 8) & 0xFF00) + (w7 & 0xFF); 
  }

  if (this->mask) {
    this->maskKey[0] = buf[offset++];
    this->maskKey[1] = buf[offset++];
    this->maskKey[2] = buf[offset++];
    this->maskKey[3] = buf[offset++];
  }  

  this->data = new char[this->len];
   
  for (uint64_t i = 0; i < this->len; ++i) {
    char c = buf[offset++];
    this->data[i] = this->mask ? c ^ this->maskKey[i % 4] : c; 
  }

  printf("\n%02x\n", (unsigned int)(unsigned char)c);
};

NWSFrame::~NWSFrame() {
  if (this->data) delete[] this->data;
}

void NWSFrame::print() {
  cout<<endl;
  cout<<"fin:"<<boolalpha<<this->fin<<noboolalpha<<endl;
  cout<<"opcode:"<<hex<<(unsigned int)this->opcode<<dec<<endl;
  cout<<"mask:"<<boolalpha<<this->mask<<noboolalpha<<endl;
  cout<<"len:"<<this->len<<endl;
  
  cout<<"maskKey:"<<hex
    <<(unsigned int)(unsigned char)this->maskKey[0]<<" "
    <<(unsigned int)(unsigned char)this->maskKey[1]<<" "
    <<(unsigned int)(unsigned char)this->maskKey[2]<<" "
    <<(unsigned int)(unsigned char)this->maskKey[3]
    <<dec<<endl;

  cout<<"data:"<<this->data<<endl;
}
