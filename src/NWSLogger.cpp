#include "NWSLogger.h"

using namespace std;

namespace nws {

  NWSLogger::NWSLogger(Level level):level(level){
    switch (level) {
      case Debug:
        this->strLevel = "DEBUG";
        this->color = "\033[1;32m";
        break;
  
      case Warning:
        this->strLevel = "WARNING";
        this->color = "\033[1;33m";
  
      case Error:
        this->strLevel = "ERROR";     
        this->color = "\033[1;31m";

      case Info:
      default:
        this->strLevel = "INFO";      
        this->color = "\033[1;37m";
    }
  };
  
  ostream& NWSLogger::operator()() {
  
    auto nowTime = std::chrono::system_clock::now();
    time_t tm = std::chrono::system_clock::to_time_t(nowTime);
    auto ms = chrono::duration_cast< chrono::milliseconds >(nowTime.time_since_epoch()).count();
    double sc = ms / 1000.;
    int tf  = floor((sc - floor(sc)) * 1000.);
  
    cout << endl 
      << this->color << this->strLevel 
      << "\033[0;34m" << "[" << put_time(std::localtime(&tm), "%F %T") << "." << tf << "]" 
      << this->color << ": " 
      << "\033[0m";
    
    return cout;
  }
  
  class NWSLogger debug(NWSLogger::Debug);
  class NWSLogger warning(NWSLogger::Debug);
  class NWSLogger error(NWSLogger::Debug);
  class NWSLogger info(NWSLogger::Info);

}
