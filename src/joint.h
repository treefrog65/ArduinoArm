
#pragma once

#include <board.h>

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint{
 public:
  Joint();
  int jointId;
  int lastPosition;
  int begin(int jointId, JointType jointType, int minAngle, int maxAngle);

  int moveJoint(Board board, int commandedAngle, int time, bool immediate);

  int readPosition(Board board);

 private:
  JointType jointType;
  int minAngle;
  int maxAngle;
};