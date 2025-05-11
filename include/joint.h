
#ifndef JOINT_H
#define JOINT_H

#include "board.h"

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint {
 public:
  int jointId_;
  void begin(int jointId, JointType jointType, uint16_t minPosition_, uint16_t maxPosition_);

  int moveJoint(Board board, uint16_t commandedAngle, uint16_t time, bool immediate);

  int setVoltageLimits(Board board, int vMin, int vMax);
  int setMaxTemp(Board board, uint8_t maxTemp);

  int readPosition(Board board);
  int readTemp(Board board);
  int readVoltageLimits(Board board);
  int readVoltage(Board board);
  int readMaxTemp(Board board);

  int getMaxVoltage() { return maxVoltage_; };
  int getMinVoltage() { return minVoltage_; };
  int getVoltage() { return voltageIn_; }
  int getLastPosition() { return lastPosition_; };
  int getTemp() { return temp_; };
  int getMaxTemp() { return maxTemp_; };

 private:
  JointType jointType_;

  int temp_;
  int voltageIn_;
  int lastPosition_;

  int maxTemp_;
 public:
  uint16_t minPosition_;
  uint16_t maxPosition_;

  int minVoltage_;
  int maxVoltage_;
};

#endif //JOINT_H