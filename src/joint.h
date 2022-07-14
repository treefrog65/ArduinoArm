
#pragma once

#include <board.h>

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint{
 public:
  Joint();
  int jointId_;
  int lastPosition_;
  int begin(int jointId, JointType jointType, int minAngle, int maxAngle);

  int moveJoint(Board board, int commandedAngle, int time, bool immediate);

  int setVoltageLimit(Board board, int vMin, int vMax);

  int readPosition(Board board);

  int readTemp(Board board);

  int readVoltageLimits(Board board);
  int getMaxVoltage();
  int getMinVoltage();

  int readVoltage(Board board);

 private:
  JointType jointType_;
  int minAngle_;
  int maxAngle_;
  int minVoltage_;
  int maxVoltage_;
};