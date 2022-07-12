
#include <Arduino.h>

#include "board.h"
#include "joint.h"

#define DEBUG 1

int commandedPosition;

unsigned long updatePeriod = 1000;
unsigned long lastUpdate = 0;
static const long baudRate = 115200;

Board board(&Serial1, baudRate);
Joint joints[] = {Joint(), Joint(), Joint(), Joint(), Joint(), Joint()};

void setup() {
  Serial.begin(baudRate);
  Serial1.begin(baudRate);
  joints[0].begin(0, JointType::revolute, 100, 900);
  joints[1].begin(1, JointType::revolute, 100, 900);
  joints[2].begin(2, JointType::revolute, 400, 500);
  joints[3].begin(3, JointType::revolute, 400, 500);
  joints[4].begin(4, JointType::revolute, 400, 500);
  joints[5].begin(5, JointType::revolute, 400, 500);
  delay(500);
}

void loop() {
  while (Serial.available() > 0) {
    char cmd = Serial.read();

    // Update all joints at once with
    if (cmd == 'A') {
      Serial.println("All joint update mode");
      for (int i = 0; i < 7; i++) {
        commandedPosition = Serial.parseInt();

        int time = Serial.parseInt();
        // Move joint object Immediately
        joints[i].moveJoint(board, commandedPosition, time, 1);
      }
    }

    // Update a single joint
    if (cmd == 'S') {
      Serial.println("Single joint update mode");
      int joint = Serial.parseInt();
      commandedPosition = Serial.parseInt();
      int time = Serial.parseInt();
      if (joints[joint].moveJoint(board, commandedPosition, time, 1)) {
        Serial.print("Joint ");
        Serial.print(joint);
        Serial.print(" angle updated to: ");
        Serial.print(commandedPosition);
        Serial.print(" in ");
        Serial.print(time);
        Serial.println(" ms.");

      } else {
        Serial.print("Joint ");
        Serial.print(joint);
        Serial.println(" angle out of range");
      }
    }
  }
/*
  // Print the current joint positions to the serial monitor for debugging
  if (millis() - lastUpdate > updatePeriod) {
    if (DEBUG) {
      char msg[80];
      //joints[1].moveJoint(board, 500, 500, 1);

      // Print the data header
      board.logError("");
      board.logError("------------Joint Positions------------");

      // Update all joint positions and print to the serial monitor
      for (int i = 0; i < sizeof(joints) / sizeof(joints[0]); i++) {
        int currentPosition = joints[i].readPosition(board);
        sprintf(msg, "Joint %i: %i", i, currentPosition);
        Serial.println(msg);
      }
    }
    lastUpdate = millis();
  }*/
}