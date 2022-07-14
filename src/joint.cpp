
#include "joint.h"

#include "board.h"

Joint::Joint() {
}

int Joint::begin(int id, JointType jType, int min, int max) {
  jointType = jType;
  jointId = id;
  minAngle = min;
  maxAngle = max;
  return 1;
}

// Requests joint to move to angle in certain time either immediately or after SERVO_MOVE_START
// Returns a 1 if succesful and a 0 if not
int Joint::moveJoint(Board board, int commandedAngle, int time, bool immediate) {
  // Verify commanded angle and time is valid
  if ((commandedAngle <= maxAngle) && (commandedAngle >= minAngle) && (time <= 30000) && (time >= 0)) {
    // Break given angle and time parameters into byte sized chunks for command packet
    uint8_t params[] = {(uint8_t)commandedAngle, (uint8_t)(commandedAngle >> 8), (uint8_t)time,
                        (uint8_t)(time >> 8)};

    if (immediate) {
      board.sendCommand(CommandType::SERVO_MOVE_TIME_WRITE, jointId, 4, params);
    } else {
      board.sendCommand(CommandType::SERVO_MOVE_TIME_WAIT_WRITE, jointId, 4, params);
    }
    delay(70000 / 115200);

    return 1;
  }
  return 0;
}

int Joint::readPosition(Board board) {
  board.sendCommand(CommandType::SERVO_POS_READ, jointId, 0, 0);

  uint8_t params[2];

  // Give the servo time to think
  delay(board.time(8) + 5);

  bool success = board.read(CommandType::SERVO_POS_READ, params, 2, jointId);

  if (success) {
    lastPosition = (params[1] << 8) + params[0];
    return lastPosition;
  }

  return 0;
}

int Joint::readTemp(Board board) {
  board.sendCommand(CommandType::SERVO_TEMP_READ, jointId, 0, 0);

  uint8_t params[1];

  // Give the servo time to think
  delay(board.time(7) + 5);

  bool success = board.read(CommandType::SERVO_TEMP_READ, params, 1, jointId);

  if (success) {
    return params[0];
  }

  return 0;
}