
#include "joint.h"

Joint::Joint() {
}

int Joint::begin(int id, JointType jType, int min, int max) {
  jointType_ = jType;
  jointId_ = id;
  minAngle_ = min;
  maxAngle_ = max;
  return 1;
}

// Requests joint to move to angle in certain time either immediately or after SERVO_MOVE_START
// Returns a 1 if succesful and a 0 if not
int Joint::moveJoint(Board board, int commandedAngle, int time, bool immediate) {
  // Verify commanded angle and time is valid
  if ((commandedAngle <= maxAngle_) && (commandedAngle >= minAngle_) && (time <= 30000) && (time >= 0)) {
    // Break given angle and time parameters into byte sized chunks for command packet
    uint8_t params[] = {(uint8_t)commandedAngle, (uint8_t)(commandedAngle >> 8), (uint8_t)time,
                        (uint8_t)(time >> 8)};

    if (immediate) {
      board.sendCommand(CommandType::SERVO_MOVE_TIME_WRITE, jointId_, 4, params);
    } else {
      board.sendCommand(CommandType::SERVO_MOVE_TIME_WAIT_WRITE, jointId_, 4, params);
    }
    return 1;
  }
  return 0;
}

int Joint::setVoltageLimits(Board board, int vMin, int vMax) {
  // Verify that the parameters are within the given specifications of the servos
  if ((vMin >= 4500) && (vMax <= 12000)) {
    // Break down ints into byts to be transmitted
    uint8_t params[] = {(uint8_t)vMin, (uint8_t)(vMin >> 8), (uint8_t)vMax,
                        (uint8_t)(vMax >> 8)};
    board.sendCommand(CommandType::SERVO_VIN_LIMIT_WRITE, jointId_, 4, params);

    //Servo processing time
    delay(board.time(10));

    if (readVoltageLimits(board)) {
      if (minVoltage_ == vMin && maxVoltage_ == vMax) {
        return 1;
      }
    }
  }
  return 0;
}

int Joint::setMaxTemp(Board board, uint8_t maxTemp) {
  // Check parameter is within specifications of servo
  if (maxTemp >= 50 && maxTemp <= 100) {
    uint8_t params[1] = {maxTemp};
    board.sendCommand(CommandType::SERVO_TEMP_MAX_LIMIT_WRITE, jointId_, 1, params);

    delay(board.time(7));

    // Verify that the parameter was set correctly
    if (readMaxTemp(board)) {
      if (maxTemp_ == maxTemp) {
        return 1;
      }
    }
  }
  return 0;
}

int Joint::readPosition(Board board) {
  board.sendCommand(CommandType::SERVO_POS_READ, jointId_, 0, 0);

  uint8_t params[2]{0};

  // Give the servo time to think
  delay(board.time(8));

  if (board.read(CommandType::SERVO_POS_READ, params, 2, jointId_)) {
    lastPosition_ = (params[1] << 8) + params[0];
    return 1;
  }

  return 0;
}

int Joint::readMaxTemp(Board board) {
  board.sendCommand(CommandType::SERVO_TEMP_MAX_LIMIT_READ, jointId_, 0, 0);

  uint8_t params[1]{0};
  delay(board.time(7));

  if (board.read(CommandType::SERVO_TEMP_MAX_LIMIT_READ, params, 1, jointId_)) {
    maxTemp_ = params[0];
    return 1;
  }
  return 0;
}

int Joint::readTemp(Board board) {
  board.sendCommand(CommandType::SERVO_TEMP_READ, jointId_, 0, 0);

  uint8_t params[1];

  // Give the servo time to think
  delay(board.time(7));

  if (board.read(CommandType::SERVO_TEMP_READ, params, 1, jointId_)) {
    temp_ = params[0];
    return 1;
  }

  return 0;
}

int Joint::readVoltageLimits(Board board) {
  board.sendCommand(CommandType::SERVO_VIN_LIMIT_READ, jointId_, 0, 0);

  uint8_t params[4];

  // Give the servo time to think
  delay(board.time(10));

  if (board.read(CommandType::SERVO_VIN_LIMIT_READ, params, 4, jointId_)) {
    minVoltage_ = (params[1] << 8) + params[0];
    maxVoltage_ = (params[3] << 8) + params[2];
    return 1;
  }

  return 0;
}

int Joint::readVoltage(Board board) {
  board.sendCommand(CommandType::SERVO_VIN_READ, jointId_, 0, 0);

  uint8_t params[2];

  // Give the servo time to think
  delay(board.time(8));

  if (board.read(CommandType::SERVO_VIN_READ, params, 2, jointId_)) {
    voltageIn_ = (params[1] << 8) + params[0];
    return 1;
  }

  return 0;
}