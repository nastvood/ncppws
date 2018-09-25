#ifndef __wscrypto__
#define __wscrypto__

#include <openssl/sha.h>
#include <openssl/opensslv.h>
#include <openssl/ssl.h>
#include <string>
#include <vector>


using namespace std;

string sha1(string const &s);
string base64Encode(const string &in);
string base64Decode(const string &in);
void clientHello(char *c, size_t len);

#endif
