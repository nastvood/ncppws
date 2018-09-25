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
      res.push_back(tmp);
      break;
    }

    res.push_back(tmp.substr(0, pos));
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

const string b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string urlDecode(const string &value) {
  ostringstream res;

  for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
    string::value_type c = (*i);

		if (c == '%') {
			if (distance(i, value.end()) >= 3) {
				++i;							
    		string::value_type f = (*i);
				++i;				
    		string::value_type s = (*i);

				if ((!isxdigit(f)) || (!isxdigit(s))) {
					res << c << f << s;
					continue;
				}		

				res << (char)stoul(string(&f, 1) + string(&s, 1), 0, 16);
			}
		} else {
	    res << c;
		}
  }

  return res.str();
}
