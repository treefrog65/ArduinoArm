
#include "Joint.h"

Joint::Joint(BusLinker* busLinker, int id, JointType jType, uint16_t min, uint16_t max) {
  bus = busLinker;
  jointType_ = jType;
  jointId_ = id;
  minPosition_ = min;
  maxPosition_ = max;
}

// Requests joint to move to angle in certain time either immediately or after SERVO_MOVE_START
int Joint::moveJoint(uint16_t commandedPosition, uint16_t time, bool immediate) {
  // Verify commanded angle and time is valid
  if ((commandedPosition > maxPosition_) || (commandedPosition < minPosition_) || (time > 30000) || (time < 0)) {
    return 0;
  }

  // Break given angle and time parameters into byte sized chunks for command packet
  uint8_t params[] = {(uint8_t)commandedPosition, (uint8_t)(commandedPosition >> 8), (uint8_t)time,
                      (uint8_t)(time >> 8)};

  if (immediate) {
    bus->sendCommand(CommandType::SERVO_MOVE_TIME_WRITE, jointId_, 4, params);
  } else {
    bus->sendCommand(CommandType::SERVO_MOVE_TIME_WAIT_WRITE, jointId_, 4, params);
  }
  return 1;
}

int Joint::setVoltageLimits(uint16_t vMin, uint16_t vMax) {
  // Verify that the parameters are within the given specifications of the servos
  if ((vMin < 4500) || (vMax > 12000)) {
    return 0;
  }

  // Break down ints into byts to be transmitted
  uint8_t params[] = {(uint8_t)vMin, (uint8_t)(vMin >> 8), (uint8_t)vMax,
                      (uint8_t)(vMax >> 8)};
  bus->sendCommand(CommandType::SERVO_VIN_LIMIT_WRITE, jointId_, 4, params);

  // Servo processing time
  delay(bus->time(10));

  if (readVoltageLimits()) {
    if (minVoltage_ == vMin && maxVoltage_ == vMax) {
      return 1;
    }
  }
  return 0;
}

int Joint::setMaxTemp(uint8_t maxTemp) {
  // Check parameter is within specifications of servo
  if (maxTemp >= 50 && maxTemp <= 100) {
    uint8_t params[1] = {maxTemp};
    bus->sendCommand(CommandType::SERVO_TEMP_MAX_LIMIT_WRITE, jointId_, 1, params);

    delay(bus->time(7));

    // Verify that the parameter was set correctly
    if (readMaxTemp()) {
      if (maxTemp_ == maxTemp) {
        return 1;
      }
    }
  }
  return 0;
}

int Joint::readPosition() {
  bus->sendCommand(CommandType::SERVO_POS_READ, jointId_, 0, 0);

  uint8_t params[2]{0};

  // Give the servo time to think
  delay(bus->time(8));

  if (bus->read(CommandType::SERVO_POS_READ, params, 2, jointId_)) {
    lastPosition_ = (params[1] << 8) + params[0];
    return 1;
  }

  return 0;
}

int Joint::readMaxTemp() {
  bus->sendCommand(CommandType::SERVO_TEMP_MAX_LIMIT_READ, jointId_, 0, 0);

  uint8_t params[1]{0};
  delay(bus->time(7));

  if (bus->read(CommandType::SERVO_TEMP_MAX_LIMIT_READ, params, 1, jointId_)) {
    maxTemp_ = params[0];
    return 1;
  }
  return 0;
}

int Joint::readTemp() {
  bus->sendCommand(CommandType::SERVO_TEMP_READ, jointId_, 0, 0);

  uint8_t params[1];

  // Give the servo time to think
  delay(bus->time(7));

  if (bus->read(CommandType::SERVO_TEMP_READ, params, 1, jointId_)) {
    temp_ = params[0];
    return 1;
  }

  return 0;
}

int Joint::readVoltageLimits() {
  bus->sendCommand(CommandType::SERVO_VIN_LIMIT_READ, jointId_, 0, 0);

  uint8_t params[4];

  // Give the servo time to think
  delay(bus->time(10));

  if (bus->read(CommandType::SERVO_VIN_LIMIT_READ, params, 4, jointId_)) {
    minVoltage_ = (params[1] << 8) + params[0];
    maxVoltage_ = (params[3] << 8) + params[2];
    return 1;
  }

  return 0;
}

int Joint::readVoltage() {
  bus->sendCommand(CommandType::SERVO_VIN_READ, jointId_, 0, 0);

  uint8_t params[2];

  // Give the servo time to think
  delay(bus->time(8));

  if (bus->read(CommandType::SERVO_VIN_READ, params, 2, jointId_)) {
    voltageIn_ = (params[1] << 8) + params[0];
    return 1;
  }

  return 0;
}