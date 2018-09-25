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
      std::ostream& operator()();
  };
  
  extern NWSLogger debug;
  extern NWSLogger warning;
  extern NWSLogger error;
  extern NWSLogger info;
}

#endif
