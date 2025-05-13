
#ifndef JOINT_H
#define JOINT_H

#include "BusLinker.h"

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint {
 public:
  Joint(BusLinker* busLinker, int jointId, JointType jointType, uint16_t minPosition_, uint16_t maxPosition_);
  int jointId_;

  int moveJoint(uint16_t commandedAngle, uint16_t time, bool immediate);

  int setVoltageLimits(uint16_t vMin, uint16_t vMax);
  int setPositionLimits(uint16_t posMin, uint16_t posMax);
  int setMaxTemp(uint8_t maxTemp);

  int readPosition();
  int readTemp();
  int readVoltageLimits();
  int readVoltage();
  int readMaxTemp();

  uint16_t getMinVoltage() { return minVoltage_; };
  uint16_t getMaxVoltage() { return maxVoltage_; };
  uint16_t getVoltage() { return voltageIn_; }
  int16_t getLastPosition() { return lastPosition_; };
  uint8_t getTemp() { return temp_; };
  uint8_t getMaxTemp() { return maxTemp_; };

 private:
  BusLinker* bus;
  JointType jointType_;

  uint8_t temp_;
  uint16_t voltageIn_;
  int16_t lastPosition_;

  uint8_t maxTemp_;

  uint16_t minPosition_;
  uint16_t maxPosition_;

  uint16_t minVoltage_;
  uint16_t maxVoltage_;
};

#endif //JOINT_H