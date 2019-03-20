#include "wscrypto.h"
#include <vector>

using namespace std;

string sha1(string const &s) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  char *c = const_cast<char *>(s.c_str());

  SHA1(reinterpret_cast<unsigned char*>(c), s.size(), hash); 

	string res;

  return string(reinterpret_cast<char *>(hash), SHA_DIGEST_LENGTH);
}

//https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
typedef unsigned char uchar;
const string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
string base64Encode(const string &in) {
   string out;

    int val = 0, valb = -6;
    for (uchar c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(b[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(b[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');

    return out;
}

string base64Decode(const string &in) {
  string out;

  vector<int> T(256, -1);
  for (int i = 0; i < 64; i++) T[b[i]] = i;

  int val = 0, valb = -8;
  for (uchar c : in) {
	  if (T[c] == -1) break;
    val = (val << 6) + T[c];
    valb += 6;
  	if (valb >= 0) {
    	out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }

  return out;
}
