#include "wshelper.h"

using namespace std;

pair<string, string> split(const string &s, const string &delimiter) {
  size_t pos = s.find(delimiter);

  if (pos == string::npos) {
    return make_pair(s, "");
  }

  string fst = s.substr(0, pos);
  string snd = s.substr(pos + delimiter.length());

  return make_pair(fst, snd);
} 

vector<string> nsplit(const string &s, const string &delimiter) {
  vector<string> res;
  string tmp = s;

  while(1) {
    size_t pos = tmp.find(delimiter);

    if (pos == string::npos) {
      cout<<"add---"<<tmp<<endl;
      res.push_back(tmp);
      break;
    }

    res.push_back(tmp.substr(0, pos));
    cout<<"add---"<<tmp.substr(0, pos)<<endl;
    tmp = tmp.substr(pos + delimiter.length());
  }    

  return res;
} 

string trim(const string &s) {
  string tmp = s;
  tmp.erase(0, tmp.find_first_not_of(" \n\r\t"));
  tmp.erase(tmp.find_last_not_of(" \n\r\t") + 1);

  return tmp;
}

string lower(string const &s) {
  string tmp = s;
  transform(tmp.begin(), tmp.end(), tmp.begin(), (int (*)(int))std::tolower);

  return tmp;
}
