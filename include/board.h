
#ifndef BOARD_H
#define BOARD_H

#include <Arduino.h>

enum CommandType {
  SERVO_MOVE_TIME_WRITE = 1,
  SERVO_MOVE_TIME_READ = 2,
  SERVO_MOVE_TIME_WAIT_WRITE = 7,
  SERVO_VIN_LIMIT_WRITE = 22,
  SERVO_VIN_LIMIT_READ = 23,
  SERVO_TEMP_MAX_LIMIT_WRITE = 24,
  SERVO_TEMP_MAX_LIMIT_READ = 25,
  SERVO_TEMP_READ = 26,
  SERVO_VIN_READ = 27,
  SERVO_POS_READ = 28,
  SERVO_LED_ERROR_READ = 36
};

class Board {
 public:
  Board(HardwareSerial *port, long baud);

  uint8_t checkSum(uint8_t bufferLength, uint8_t *buffer);
  int sendCommand(CommandType command, uint8_t id, uint8_t numOfParams, uint8_t *params);
  int read(CommandType command, uint8_t *params, int numOfParams, uint8_t id);
  int time(uint32_t len);

 private:
  HardwareSerial *port;
  long baud;
};

#endif //BOARD_H