
#pragma once

#include <Arduino.h>

enum CommandType {
  SERVO_MOVE_TIME_WRITE = 1,
  SERVO_MOVE_TIME_WAIT_WRITE = 7,
  SERVO_POS_READ = 28,
  SERVO_LED_ERROR_READ = 36
};

class Board {
 public:
  Board(HardwareSerial *port, long baud);

  uint8_t checkSum(uint8_t bufferLength, uint8_t *buffer);
  int sendCommand(CommandType command, uint8_t id, uint8_t numOfParams, uint8_t *params);
  int read(CommandType command, uint8_t *params, int numOfParams, uint8_t id);
  int logError(String message);
  int logError(int);
  int logError(char *buf);
  void debugOn();
  void debugOff();

  bool debug = false;

 private:
  HardwareSerial *port;
  long baud;
};