
#include "logger.h"

// Log information if level is above threshold
// Returns 1 if logged, 0 if not
int Logger::log(LOGGER_LEVEL level, char* buf) {
  if (level >= loggerLevel_) {
    port_->println(buf);
    return 1;
  }
  return 0;
}

int Logger::setLoggerLevel(LOGGER_LEVEL loggerLevel) {
  this->loggerLevel_ = loggerLevel;
  return this->loggerLevel_;
}