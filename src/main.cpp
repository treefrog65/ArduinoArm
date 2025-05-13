
#include <Arduino.h>

#include "CommandParser.h"
#include "SoftwareSerial.h"
#include "BusLinker.h"
#include "Joint.h"

int commandedPosition;

unsigned long updatePeriod = 1000;
unsigned long lastUpdate = 0;
static const long baudRate = 115200;

typedef CommandParser<> MyCommandParser;
MyCommandParser parser;
bool debugEnabled = false;
bool commandReady{false};
char input[64];
uint8_t inputPos{0};

// For nano
auto& pcSerial = Serial;
SoftwareSerial boardSerial = SoftwareSerial(2, 3);


BusLinker bus(&boardSerial, baudRate);
  // Setup joint array with angles limited to physical interferences
Joint joints[6] = {
  Joint(&bus, 0, JointType::revolute, 0, 100),
  Joint(&bus, 1, JointType::revolute, 100, 660),
  Joint(&bus, 2, JointType::revolute, 125, 910),
  Joint(&bus, 3, JointType::revolute, 125, 910),
  Joint(&bus, 4, JointType::revolute, 0, 1000),
  Joint(&bus, 5, JointType::revolute, 0, 1000)
};

void move(MyCommandParser::Argument* args, char* response) {
  // Create and send servo command
  char msg[64];
  uint8_t joint = args[0].asUInt64;
  uint16_t position = args[1].asUInt64;
  uint16_t time = args[2].asUInt64;
  bool now = args[3].asUInt64;

  if (joint > 5) {
    sprintf(msg, "Joint %u out of range", joint);
    memmove(response, msg, MyCommandParser::MAX_RESPONSE_SIZE);
    return;
  }

  if (joints[joint].moveJoint(position, time, true)) {
    sprintf(msg, "Moving Joint %i to %i in %i ms", joint, position, time);
  } else {
    sprintf(msg, "Error!");
  }
  memmove(response, msg, MyCommandParser::MAX_RESPONSE_SIZE);
}

void move_wait(MyCommandParser::Argument* args, char* response) {
  // Create and send servo command
  char msg[64];
  uint8_t joint = args[0].asUInt64;
  uint16_t position = args[1].asUInt64;
  uint16_t time = args[2].asUInt64;
  bool now = args[3].asUInt64;

  if (joint > 5) {
    sprintf(msg, "Joint %u out of range", joint);
    memmove(response, msg, MyCommandParser::MAX_RESPONSE_SIZE);
    return;
  }

  if (joints[joint].moveJoint(position, time, false)) {
    sprintf(msg, "Scheduling Joint %i to %i in %i ms", joint, position, time);
  } else {
    sprintf(msg, "Error!");
  }
  memmove(response, msg, MyCommandParser::MAX_RESPONSE_SIZE);
}

void debug(MyCommandParser::Argument* args, char* response) {
  if (args[0].asUInt64 == 0) {
    debugEnabled = false;
    strlcpy(response, "Debugging disabled", MyCommandParser::MAX_RESPONSE_SIZE);
  } else {
    debugEnabled = true;
    strlcpy(response, "Debugging enabled", MyCommandParser::MAX_RESPONSE_SIZE);
  }
}

void setup() {
  pcSerial.begin(baudRate);
  boardSerial.begin(baudRate);

  parser.registerCommand("move", "uuu", &move);
  parser.registerCommand("move_wait", "uuu", &move);
  parser.registerCommand("debug", "u", &debug);
  parser.registerCommand("get_vin", "u", &debug);
  parser.registerCommand("get_vmax", "u", &debug);
  parser.registerCommand("set_vmax", "uu", &debug);
  parser.registerCommand("get_tin", "u", &debug);
  parser.registerCommand("get_tmax", "u", &debug);
  parser.registerCommand("set_tmax", "uu", &debug);

  delay(500);

  pcSerial.println("Welcome to ArduinoArm");
}

void loop() {
  while (pcSerial.available() > 0 && !commandReady) {  // Check if data is available to read
    char next = pcSerial.peek();
    if (next == '\n' || next == '\r') {
      commandReady = true;
      while (next == '\n' || next == '\r') {
        pcSerial.read();
        next = pcSerial.peek();  // Get rid of the new line byte
      }
      break;
    }
    input[inputPos] = pcSerial.read();
    inputPos++;
  }

  if (commandReady) {
    char response[MyCommandParser::MAX_RESPONSE_SIZE];
    parser.processCommand(input, response);
    pcSerial.println(response);
    memset(input, 0, sizeof(input));
    inputPos = 0;
    commandReady = false;
  }

  // Print the current joint positions to the serial monitor for debugging
  if ((millis() - lastUpdate > updatePeriod) && debugEnabled) {
    // Serial.println("Start Debug");
    // Print the data header
    char msg[128];
    sprintf(msg, "\n------------Joint Data------------\n");
    Serial.println(msg);

    // Update all joint positions and print to the serial monitor
    for (uint8_t i = 0; i < sizeof(joints) / sizeof(joints[0]); i++) {
      // Update position of joint
      joints[i].readPosition();

      // Update temparature of joint
      joints[i].readTemp();
      joints[i].readMaxTemp();

      joints[i].readVoltageLimits();

      joints[i].readVoltage();

      sprintf(msg, "Joint %i: Max Temp=%u Temp=%i Position=%i Vmin=%i Vmax=%u Vin=%i", i, joints[i].getMaxTemp(), joints[i].getTemp(),
              joints[i].getLastPosition(), joints[i].getMinVoltage(), joints[i].getMaxVoltage(), joints[i].getVoltage());
      Serial.println(msg);
      // Serial.println("End Debug");
    }
    lastUpdate = millis();
  }
}