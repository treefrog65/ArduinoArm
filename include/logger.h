

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LOGGER_LEVEL {
  DEBUG = 0,
  INFO = 1,
  WARNING = 2,
  ERROR = 3,
  FATAL = 4
};

// Defines a debug function to utilize a given serial port
class Logger {
 public:
  
  void setLoggerPort(Serial_ *port) { this->port_ = port; };

  int log(LOGGER_LEVEL, char *);
  int setLoggerLevel(LOGGER_LEVEL);

 private:
    Serial_ *port_;
    int loggerLevel_;

};

#endif //LOGGER_H