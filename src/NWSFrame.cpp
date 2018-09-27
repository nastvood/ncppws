#include "NWSFrame.h"

using namespace std;

namespace nws {

  NWSFrame::NWSFrame(const char *buf) {
    uint64_t offset = 0;
  
    char c = buf[offset++];  
    this->fin = c >> 7;
  
    switch (c & 0x0F) {
      case 0x0:
        this->opcode = Continuation;
        break;
  
      case 0x1:      
        this->opcode = Text;
        break;
  
      case 0x2:      
        this->opcode = Binary;
        break;
  
      case 0x8:      
        this->opcode = Close; //2 bytes data
        break;
  
      case 0x9:      
        this->opcode = Ping;
        break;
  
      case 0xa:      
        this->opcode = Pong;
        break;
  
      default:
        this->opcode = Other;      
    }
  
    c = buf[offset++];
    this->mask = c >> 7;
    unsigned char l = c & 0x7F;
  
    if (l <= 125) {
      this->len = l;
    } else if (l == 126) {
      uint16_t *s = (uint16_t *)&buf[offset];
      this->len = __builtin_bswap16(*s);
      offset += 2;
  
    } else {
      uint64_t *s = (uint64_t *)&buf[offset];
      this->len = __builtin_bswap64(*s);
      offset += 4;
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
      this->data[i] = this->mask ? c ^ this->maskKey[i % MASK_KEY_LEN] : c; 
    }
  
    if (this->opcode == Close) {
      uint16_t *s = (uint16_t *)&this->data[0];
      this->closeCode = __builtin_bswap16(*s);
    }
  };
  
  NWSFrame::NWSFrame(bool fin, Opcode opcode, bool mask, uint16_t len, char maskKey[MASK_KEY_LEN], const char *data, uint16_t closeCode = 0)
    :fin(fin), opcode(opcode), mask(mask), len(len), closeCode(closeCode) {
  
    memcpy(this->maskKey, maskKey, MASK_KEY_LEN);
  
    this->data = new char[this->len];
    memcpy(this->data, data, this->len);  
  
  }
  
  NWSFrame::~NWSFrame() {
    if (this->data) delete[] this->data;
  }
  
  NWSFrame NWSFrame::createClose(uint16_t closeCode) {
    char maskKey[MASK_KEY_LEN] = {0};
  
    uint16_t code = __builtin_bswap16(closeCode);
    NWSFrame frame = NWSFrame(true, Close, false, CLOSE_DATA_LEN, maskKey, (char *)&code, closeCode);
  
    return frame;      
  }
  
  uint64_t NWSFrame::packetSize() {
    uint64_t size = 2;
  
    if ((this->len > 125) && (this->len <= 0xFFFF)) size += 2;
  
    if (this->len > 0xFFFF) size += 4;
  
    if (this->mask) size += 4;
  
    return size + this->len; 
  }
  
  size_t NWSFrame::generatePacket(char **retPacket) {
    unsigned char *packet = new unsigned char[this->packetSize()]();
  
    unsigned char firsByte = 0;  
    if (this->fin) firsByte = firsByte | 0x80;
    firsByte += ucharOfOpcode(this->opcode);
  
    unsigned char secondByte = 0;
    if (this->mask) secondByte = secondByte | 0x80;
  
    unsigned char dataLen = 0;
    size_t offset = 0;
    if (this->len <= 125) {
      dataLen = this->len;
    } else if ((this->len > 125) && (this->len <= 0xFFFF))  {
      dataLen = 126;
      offset = 2;
      uint16_t size = __builtin_bswap16((uint16_t) this->len);
      memcpy(&packet[2], &size, offset);
    } else if (this->len > 0xFFFF) {
      dataLen = 126;
      offset = 4;
      uint64_t size = __builtin_bswap64(this->len);
      memcpy(&packet[2], &size, offset);
    } 
    secondByte += dataLen;
  
    packet[0] = firsByte;
    packet[1] = secondByte;
    offset += 2;
  
    if (this->mask) {
      for (uint64_t i = 0; i < this->len; ++i) {
        packet[offset + i] = this->data[i] ^ this->maskKey[i % MASK_KEY_LEN];
      }
    } else {
      memcpy((void *)&packet[offset], this->data, this->len); 
    }
  
    *retPacket = (char *)packet; 
  
    return this->packetSize();
  }
  
  unsigned char NWSFrame::ucharOfOpcode(Opcode opcode) {
    switch (opcode) {
      case Continuation:
        return 0x0;
  
      case Text:
        return 0x1;    
  
      case Binary:      
        return 0x2;    
  
      case Close:      
        return 0x8;    
  
      case Ping:
        return 0x9;
            
      case Pong:      
        return 0xa;          
  
      default:
        return 0xB;
    }
  }
  
  string NWSFrame::stringOfOpcode(Opcode opcode) {
    switch (opcode) {
      case Continuation:
        return "Continuation";
  
      case Text:
        return "Text";    
  
      case Binary:      
        return "Binary";    
  
      case Close:      
        return "Close";    
  
      case Ping:
        return "Ping";
            
      case Pong:      
        return "Pong";          
  
      default:
        return "Other";
    }
  }
  
  ostream &operator<<(std::ostream &out, const NWSFrame &frame) {
    out << "NWSFrame {" <<" fin: " << boolalpha << frame.fin << noboolalpha 
      << "; opcode: " << frame.stringOfOpcode(frame.opcode)
      << "; mask: "<< boolalpha << frame.mask << noboolalpha
      << "; len: "<< frame.len 
      << "; maskKey: "<< hex
      << (unsigned int)(unsigned char)frame.maskKey[0] <<" "
      << (unsigned int)(unsigned char)frame.maskKey[1] <<" "
      << (unsigned int)(unsigned char)frame.maskKey[2] <<" "
      << (unsigned int)(unsigned char)frame.maskKey[3] << dec 
      << "; closeCode: "<< frame.closeCode; 
  
    if (frame.opcode == NWSFrame::Text) {
      cout << "; data: " << string(frame.data, frame.len);
    }
      
    out << "};";
  
    return out;    
  }
}
