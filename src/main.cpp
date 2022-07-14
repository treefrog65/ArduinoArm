
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
Joint joints[6]{Joint()};

void setup() {
  Serial.begin(baudRate);
  Serial1.begin(baudRate);
  logger.setLoggerLevel(LOGGER_LEVEL::DEBUG);

  // Setup joint array with angles limited to physical interferences
  joints[0].begin(0, JointType::revolute, 0, 1000);
  joints[1].begin(1, JointType::revolute, 100, 660);
  joints[2].begin(2, JointType::revolute, 125, 910);
  joints[3].begin(3, JointType::revolute, 125, 910);
  joints[4].begin(4, JointType::revolute, 0, 1000);
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
        if (joints[i].moveJoint(board, commandedPosition, time, 1)) {
          sprintf(msg, "Moving Joint %i to %i in %i ms", i, commandedPosition, time);
          logger.log(LOGGER_LEVEL::DEBUG, msg);
        } else {
          sprintf(msg, "Joint %i out of range", commandedPosition);
          logger.log(LOGGER_LEVEL::DEBUG, msg);
        }
      }
    }

    // Update a single joint
    if (cmd == 'S') {
      // Log that single joint mode command has been recieved
      sprintf(msg, "Single joint command received");
      logger.log(LOGGER_LEVEL::DEBUG, msg);

      // Parse Parameters (1s timeout per parameter)
      int joint = Serial.parseInt();
      commandedPosition = Serial.parseInt();
      int time = Serial.parseInt();

      // Verify parameters and send servo command
      if (time) {
        sprintf(msg, "Moving Joint %i", joint);
        logger.log(LOGGER_LEVEL::INFO, msg);
        if (joints[joint].moveJoint(board, commandedPosition, time, 1)) {
          sprintf(msg, "Moving Joint %i to %i in %i ms", joint, commandedPosition, time);
          logger.log(LOGGER_LEVEL::DEBUG, msg);

          // Angle parameter out of range
        } else {
          sprintf(msg, "Joint %i out of range", commandedPosition);
          logger.log(LOGGER_LEVEL::DEBUG, msg);
        }

        //<3 parameters received
      } else {
        sprintf(msg, "Not all parameters received.");
        logger.log(LOGGER_LEVEL::DEBUG, msg);
      }
    }
    // Update all joint voltage limits at once with
    if (cmd == 'V') {
      sprintf(msg, "Voltage limits command received.");
      logger.log(LOGGER_LEVEL::INFO, msg);

      int vMin = Serial.parseInt();
      int vMax = Serial.parseInt();

      for (int i = 0; i < 6; i++) {
        // Update joint voltage Immediately
        if (joints[i].setVoltageLimit(board, vMin, vMax)) {
          sprintf(msg, "Moving Joint %i", i);
          logger.log(LOGGER_LEVEL::INFO, msg);
        } else {
          sprintf(msg, "Joint %i voltage out of range", commandedPosition);
          logger.log(LOGGER_LEVEL::DEBUG, msg);
        }
      }
    }
  }

  // Print the current joint positions to the serial monitor for debugging
  if (millis() - lastUpdate > updatePeriod) {
    // Print the data header
    logger.log(LOGGER_LEVEL::DEBUG, "");
    logger.log(LOGGER_LEVEL::DEBUG, "------------Joint Data------------");

    // Update all joint positions and print to the serial monitor
    for (int i = 0; i < sizeof(joints) / sizeof(joints[0]); i++) {
      // Update position of joint
      int currentPosition = joints[i].readPosition(board);

      // Update temparature of joint
      int temp = joints[i].readTemp(board);

      int minVoltage = 0;
      int maxVoltage = 0;
      if (joints[i].readVoltageLimits(board)) {
        minVoltage = joints[i].getMinVoltage();
        maxVoltage = joints[i].getMaxVoltage();
      }

      int voltageIn = joints[i].readVoltage(board);

      sprintf(msg, "Joint %i: Temp=%i Position=%i Vmin=%i Vmax=%u Vin=%i", i, temp, currentPosition, minVoltage, maxVoltage, voltageIn);
      logger.log(LOGGER_LEVEL::DEBUG, msg);
    }
    lastUpdate = millis();
  }
}