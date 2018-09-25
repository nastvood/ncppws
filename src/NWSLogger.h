#ifndef __NWSLogger__
#define __NWSLogger__

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cmath>

namespace nws {
  
  class NWSLogger {
    public:
      enum Level {Debug, Error, Warning, Info};
  
    private:
      Level level;
      std::string strLevel;
      std::string color;
  
    public:
      NWSLogger(Level level);
      std::ostream& operator()(int line, const char *file);
  };
  
  extern NWSLogger _debug;
  extern NWSLogger _warning;
  extern NWSLogger _error;
  extern NWSLogger _info;
}

#define debug() _debug(__LINE__, __FILE__)
#define info() _info(__LINE__, __FILE__)
#define error() _error(__LINE__, __FILE__)
#define warning() _warning(__LINE__, __FILE__)

#endif
