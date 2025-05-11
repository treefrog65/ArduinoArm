
#include <Arduino.h>

#include "CommandParser.h"
#include "board.h"
#include "joint.h"

#include "SoftwareSerial.h"

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
auto pcSerial = Serial;
SoftwareSerial boardSerial = SoftwareSerial(2,3);

Board board(&boardSerial, baudRate);
Joint joints[6]{Joint()};

void move(MyCommandParser::Argument* args, char* response) {
  // Create and send servo command
  char msg[64];
  uint8_t joint = args[0].asUInt64;
  uint16_t position = args[1].asUInt64;
  uint16_t time = args[2].asUInt64;

  if (joint > 5 ){
    sprintf(msg, "Joint %u out of range", joint);
    memmove(response, msg, MyCommandParser::MAX_RESPONSE_SIZE);
    return;
  }

  int success = joints[joint].moveJoint(board, position, time, true);

  switch (success) {
    case 0:
      sprintf(msg, "Moving Joint %i to %i in %i ms", joint, position, time);
      break;
    case 1:
      sprintf(msg, "Joint %i requested position %i too low!", joint, position);
      break;
    case 2:
      sprintf(msg, "Joint %i requested position %i too high!", joint, position);
      break;
    case 3:
      sprintf(msg, "Joint %i requested move time %i < 0", joint, time);
      break;
    case 4:
      sprintf(msg, "Joint %i requested move time %i > 30000!", joint, time);
      break;
    default:
      sprintf(msg, "Unknown error");
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
  parser.registerCommand("debug", "u", &debug);

  // Setup joint array with angles limited to physical interferences
  joints[0].begin(0, JointType::revolute, 0, 100);
  joints[1].begin(1, JointType::revolute, 100, 660);
  joints[2].begin(2, JointType::revolute, 125, 910);
  joints[3].begin(3, JointType::revolute, 125, 910);
  joints[4].begin(4, JointType::revolute, 0, 1000);
  joints[5].begin(5, JointType::revolute, 0, 1000);
  delay(500);
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
    pcSerial.println(input);
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
      joints[i].readPosition(board);

      // Update temparature of joint
      joints[i].readTemp(board);
      joints[i].readMaxTemp(board);

      joints[i].readVoltageLimits(board);

      joints[i].readVoltage(board);

      sprintf(msg, "Joint %i: Max Temp=%u Temp=%i Position=%i Vmin=%i Vmax=%u Vin=%i", i, joints[i].getMaxTemp(), joints[i].getTemp(),
              joints[i].getLastPosition(), joints[i].getMinVoltage(), joints[i].getMaxVoltage(), joints[i].getVoltage());
      Serial.println(msg);
      // Serial.println("End Debug");
    }
    lastUpdate = millis();
  }
}