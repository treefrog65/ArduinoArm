
#include <Arduino.h>

#include "board.h"
#include "joint.h"

int commandedPosition;

unsigned long updatePeriod = 1000;
unsigned long lastUpdate = 0;
static const long baudRate = 115200;
bool debug = false;
char msg[128];

Board board(&Serial1, baudRate);
Joint joints[6]{Joint()};

void setup() {
  Serial.begin(baudRate);
  Serial1.begin(baudRate);

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
          Serial.println(msg);
        } else {
          sprintf(msg, "Joint %i out of range", commandedPosition);
          Serial.println(msg);
        }
      }
    }

    // Update a single joint
    if (cmd == 'S') {
      // Log that single joint mode command has been recieved
      sprintf(msg, "Single joint command received");
      Serial.println(msg);

      // Parse Parameters (1s timeout per parameter)
      int joint = Serial.parseInt();
      commandedPosition = Serial.parseInt();
      int time = Serial.parseInt();

      // Verify parameters and send servo command
      if (time) {
        sprintf(msg, "Moving Joint %i", joint);
        Serial.println(msg);
        if (joints[joint].moveJoint(board, commandedPosition, time, 1)) {
          sprintf(msg, "Moving Joint %i to %i in %i ms", joint, commandedPosition, time);
          Serial.println(msg);

          // Angle parameter out of range
        } else {
          sprintf(msg, "Joint %i out of range", commandedPosition);
          Serial.println(msg);
        }

        //<3 parameters received
      } else {
        sprintf(msg, "Not all parameters received.");
        Serial.println(msg);
      }
    }
    // Update all joint voltage limits at once with
    if (cmd == 'V') {
      sprintf(msg, "Voltage limits command received.");
      Serial.println(msg);

      int vMin = Serial.parseInt();
      int vMax = Serial.parseInt();

      for (int i = 0; i < 6; i++) {
        // Update joint voltage Immediately
        if (joints[i].setVoltageLimits(board, vMin, vMax)) {
          sprintf(msg, "Joint %i max voltage updated to:%i and min voltage update to:%i", i, vMax, vMin);
          Serial.println(msg);
        } else {
          sprintf(msg, "Joint %i voltage out of range", i);
          Serial.println(msg);
        }
      }
    }

    // Set max temperature of all servos
    if (cmd == 'T') {
      sprintf(msg, "Temperature limits command received.");
      Serial.println(msg);

      uint8_t maxTemp = Serial.parseInt();

      for (int i = 0; i < 6; i++) {
        // Update joint voltage Immediately
        if (joints[i].setMaxTemp(board, maxTemp)) {
          sprintf(msg, "Updating Joint %i max temperature to %u °C", i, maxTemp);
          Serial.println(msg);
        } else {
          sprintf(msg, "Joint %i temp out of range", i);
          Serial.println(msg);
        }
      }
    }

    if (cmd == 'D') {
      String val = Serial.readString();
      val.trim();
      if (val == "True"){
         debug = true;
         Serial.println("Debugging on.");
         }else if (val == "False"){
        debug = false;
        Serial.println("Debugging off.");
        }
      else{
        Serial.print("Expected ('True' or 'False) got:");
        Serial.println(val);
      }
    }
  }

  // Print the current joint positions to the serial monitor for debugging
  if ((millis() - lastUpdate > updatePeriod) && debug) {
    // Print the data header
    sprintf(msg, "\n------------Joint Data------------\n");
    Serial.println(msg);

    // Update all joint positions and print to the serial monitor
    for (uint8_t i = 0; i < sizeof(joints) / sizeof(joints[0]); i++) {
      // Update position of joint
      joints[i].readPosition(board);

      // Update temparature of joint
      joints[i].readTemp(board);
      joints[i].readMaxTemp(board);

      joints[i].readVoltageLimits(board);

      joints[i].readVoltage(board);

      sprintf(msg, "Joint %i: Max Temp=%u Temp=%i Position=%i Vmin=%i Vmax=%u Vin=%i", i, joints[i].getMaxTemp(), joints[i].getTemp(),
              joints[i].getLastPosition(), joints[i].getMinVoltage(), joints[i].getMaxVoltage(), joints[i].getVoltage());
      Serial.println(msg);
    }
    lastUpdate = millis();
  }
}