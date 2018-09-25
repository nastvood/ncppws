#ifndef __wshelper__
#define __wshelper__

#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

string trim(const string &s);
pair<string, string> split(const string &s, const string &delimiter);
vector<string> nsplit(const string &s, const string &delimiter);
string lower(string const &s);
string urlDecode(const string &value);
void printHexCString(char *c, size_t len);

#endif
