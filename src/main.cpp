
#include <Arduino.h>

#include "board.h"
#include "joint.h"
#include "logger.h"

int commandedPosition;

unsigned long updatePeriod = 1000;
unsigned long lastUpdate = 0;
static const long baudRate = 115200;
char msg[128];

Board board(&Serial1, baudRate);
Logger logger(&Serial);
Joint joints[] = {Joint(), Joint(), Joint(), Joint(), Joint(), Joint()};

void setup() {
  Serial.begin(baudRate);
  Serial1.begin(baudRate);
  logger.setLoggerLevel(LOGGER_LEVEL::DEBUG);
  joints[0].begin(0, JointType::revolute, 0, 1000);
  joints[1].begin(1, JointType::revolute, 125, 690);
  joints[2].begin(2, JointType::revolute, 250, 725);
  joints[3].begin(3, JointType::revolute, 250, 725);
  joints[4].begin(4, JointType::revolute, 250, 725);
  joints[5].begin(5, JointType::revolute, 0, 1000);
  delay(500);
}

void loop() {
  while (Serial.available() > 0) {
    char cmd = Serial.read();

    // Update all joints at once with
    if (cmd == 'A') {
      Serial.println("All joint update mode");
      for (int i = 0; i < 6; i++) {
        commandedPosition = Serial.parseInt();

        int time = Serial.parseInt();
        // Move joint object Immediately
        sprintf(msg,"Moving Joint %i to %i in %i ms", i, commandedPosition, time);
        logger.log(LOGGER_LEVEL::DEBUG,msg);
        joints[i].moveJoint(board, commandedPosition, time, 1);
        delay(700);
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

  // Print the current joint positions to the serial monitor for debugging
  if (millis() - lastUpdate > updatePeriod) {
    // Print the data header
    logger.log(LOGGER_LEVEL::DEBUG, "");
    logger.log(LOGGER_LEVEL::DEBUG, "------------Joint Positions------------");

    // Update all joint positions and print to the serial monitor
    for (int i = 0; i < sizeof(joints) / sizeof(joints[0]); i++) {
      int currentPosition = joints[i].readPosition(board);
      sprintf(msg, "Joint %i: %i", i, currentPosition);
      logger.log(LOGGER_LEVEL::DEBUG, msg);
      delay(10);
    }
    lastUpdate = millis();
  }
}