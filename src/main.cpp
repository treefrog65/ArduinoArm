
#include <Arduino.h>

#include "board.h"
#include "joint.h"

Board board(&Serial);
Joint joint0;
Joint joint1;
Joint joint2;
Joint joint3;
Joint joint4;
Joint joint5;

void setup() {
  Serial.begin(115200);
  joint0.begin(0, JointType::revolute, 100, 900);
  joint1.begin(1, JointType::revolute, 100, 900);
  joint2.begin(2, JointType::revolute, 400, 500);
  joint3.begin(3, JointType::revolute, 400, 500);
  joint4.begin(4, JointType::revolute, 400, 500);
  joint5.begin(5, JointType::revolute, 400, 500);
  delay(500);
}

void loop() {

  delay(1000);

  joint1.moveJoint(board, 100);
  delay(100);
  joint2.moveJoint(board, 400);
  delay(100);
  joint3.moveJoint(board, 400);
  delay(100);
  joint4.moveJoint(board, 400);
  delay(100);
  joint5.moveJoint(board, 400);
  delay(1000);

  joint1.moveJoint(board, 150);
  delay(100);
  joint2.moveJoint(board, 450);
  delay(100);
  joint3.moveJoint(board, 450);
  delay(100);
  joint4.moveJoint(board, 450);
  delay(100);
  joint5.moveJoint(board, 450);
  delay(1000);

  board.sendCommand(CommandType::SERVO_LED_ERROR_READ,0,0,0); 
  
}