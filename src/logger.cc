
#include "logger.h"

Logger::Logger(Serial_ *port){
  this->port = port;
}


//Log information if level is above threshold
//Returns 1 if logged, 0 if not
int Logger::log(LOGGER_LEVEL level, char *buf){
  if (level >= loggerLevel){
    port->println(buf);
    return 1;
  }
  return 0;
}

int Logger::log(LOGGER_LEVEL level, String str){
  if (level >= loggerLevel){
    port->println(str);
    return 1;
  }
  return 0;
}

int Logger::setLoggerLevel(LOGGER_LEVEL loggerLevel){
  this->loggerLevel = loggerLevel;
  return this->loggerLevel ;
}