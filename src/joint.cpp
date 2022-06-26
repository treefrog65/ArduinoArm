
#include "joint.h"
#include "board.h"

int Joint::begin(int id, JointType jType, int min, int max) {
  jointType = jType;
  jointId = id;
  minAngle = min;
  maxAngle = max;
  return 1;
};

// Takes a commanded angle and tries to move the joint to that angle
// Returns a 1 if succesful and a 0 if not
int Joint::moveJoint(Board board, int commandedAngle) {
  // Verify commanded angle is valid
  if ((commandedAngle <= maxAngle) && (commandedAngle >= minAngle)) {
    int time = 500;
    //Break given angle into byte sized chunks for command tx
    uint8_t params[] = {
    (uint8_t)commandedAngle,
    (uint8_t)(commandedAngle >> 8),
    (uint8_t)time,
    (uint8_t)(time >> 8)};

    //Serial.println("Sending command:");
    //board.debug("Command:");
    board.sendCommand(CommandType::SERVO_MOVE_TIME_WRITE, jointId, 4, params);
    return 1;
  }
  else{
    Serial.println("Bad Angle:");
    Serial.print(commandedAngle);
    Serial.print(" is out of range(");
    Serial.print(minAngle);
    Serial.print(" to ");
    Serial.print(maxAngle);
    Serial.print(") for joint ");
    Serial.println(jointId);
  }
  return 0;
}