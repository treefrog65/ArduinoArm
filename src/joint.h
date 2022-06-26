
#pragma once

#include <board.h>

enum JointType {
  revolute = 0,
  prismatic = 1,
};

class Joint {
 public:
  int jointId;
  int begin(int jointId, JointType jointType, int minAngle, int maxAngle);

  int moveJoint(Board, int);

  int moveJointTimed(int, double);

 private:
  JointType jointType;
  int minAngle;
  int maxAngle;
};