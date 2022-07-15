

#pragma once

#include <Arduino.h>

enum LOGGER_LEVEL{
  DEBUG = 0,
  INFO = 1,
  WARNING = 2,
  ERROR = 3,
  FATAL = 4
};

//Defines a debug function to utilize a given serial port
class Logger{

  public:
    Logger(Serial_*);
    int log(LOGGER_LEVEL, char*);
    int setLoggerLevel(LOGGER_LEVEL);
  
  private:
    Serial_ *port;
    int loggerLevel;

};