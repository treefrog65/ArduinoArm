
#ifndef JOINT_H
#define JOINT_H

#include "board.h"

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint {
 public:
  Joint();
  int jointId_;
  int begin(int jointId, JointType jointType, int minAngle, int maxAngle);

  int moveJoint(Board board, int commandedAngle, int time, bool immediate);

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

  int minAngle_;
  int maxAngle_;

  int minVoltage_;
  int maxVoltage_;
};

#endif //JOINT_H