#ifndef __wscrypto__
#define __wscrypto__

#include <openssl/sha.h>
#include <string>
#include <vector>

using namespace std;

string sha1(string const &s);
string base64Encode(const string &in);
string base64Decode(const string &in);

#endif
