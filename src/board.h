
#pragma once

#include <Arduino.h>

enum CommandType {
  SERVO_MOVE_TIME_WRITE = 1,
  SERVO_LED_ERROR_READ = 36
};

class Board {
 public:
  Board(HardwareSerial *port);

  uint8_t checkSum(uint8_t bufferLength, uint8_t *buffer);
  int sendCommand(CommandType command, uint8_t id, uint8_t numOfParams, uint8_t *params);
  int debug(String message);

 private:
  HardwareSerial *port;
};