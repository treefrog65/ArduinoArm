
#include "board.h"

Board::Board(HardwareSerial *bus, long baudRate) {
  port = bus;
  baud = baudRate;
}

uint8_t Board::checkSum(uint8_t bufferLength, uint8_t *buffer) {
  // Initalize chksum to 0 for calculation
  uint8_t cksum = 0;

  // Loop through buffer to calculate checksum
  for (int i = 2; i < bufferLength - 1; i++) {
    cksum += buffer[i];
  }

  return ~cksum;
}

int Board::sendCommand(CommandType command, uint8_t id, uint8_t numOfParams, uint8_t *params) {
  // Create a buffer to contain the command packet
  // the values 6 comes from the header(5 bytes) plus the checksum(1 byte)
  uint8_t bufferLength = 6 + numOfParams;
  uint8_t buffer[bufferLength];

  // Create command packet within the buffer
  buffer[0] = 0x55;
  buffer[1] = 0x55;
  buffer[2] = id;
  buffer[3] = bufferLength - 3;
  buffer[4] = command;
  // Add command parameters to buffer
  for (int i = 0; i < numOfParams; i++) {
    buffer[5 + i] = params[i];
  }
  // Add checksum to end of buffer
  buffer[bufferLength - 1] = checkSum(bufferLength, buffer);

  // Transmit the command byte-wise over the serial bus
  port->write(buffer, bufferLength);

  return 1;
}

int Board::read(CommandType command, uint8_t *params, int numOfParams, uint8_t id) {
  unsigned long start = millis();
  int received = 0;

  // Length of FULL packet
  int len = 6 + numOfParams;

  uint8_t data[len]{0};
  char msg[80];

  // Timeout based on 10 bits/char and 50ms for servo to process command
  uint32_t timeout = (len * 10000) / baud + 500;
  // delayMicroseconds(1);

  // 7 is the minimum length of the receiving packets
  // max time is limited by timeout parameter
  while ((received <= len) && ((millis() - start) < timeout)) {
    if (port->available()) {
      data[received] = port->read();
      sprintf(msg, "%i:%x", received, data[received]);
      //logError(msg);
      // Check recieved commands for known data
      // Parameters will be received if the header data is found to be good
      switch (received) {
        case 0:
        case 1:
          if (data[received] != 0x55) {
            sprintf(msg, "Error (Header): Expected 0x55 got 0x%x", data[received]);
            //logError(msg);
            return false;
          }
          break;
        case 2:
          if (data[received] != id) {
            sprintf(msg, "Error (Id): Expected %i got %i", id, data[received]);
            //logError(msg);
            return false;
          }
          break;
        case 3:
          if (data[received] != (len - 3)) {
            sprintf(msg, "Error (Length): Expected %i got %i", len, data[received]);
            //logError(msg);
            return false;
          }
          break;
        case 4:
          if (data[received] != command) {
            sprintf(msg, "Error (Command): Expected %i got %i", command, data[received]);
            //logError(msg);
            return false;
          }
          break;
        default:
          sprintf(msg, "%i received of %i: %x", received, len, data[received]);
          //logError(msg);
          if (received == (len-1)) {
            if (checkSum(len, data) == data[received]) {
              for (int i = 0; i < numOfParams; i++) {
                params[i] = data[i + 5];
              }
              return true;
            }
          }
          break;
      }  // switch
      received++;
    }
  }
  // Time out
  //logError("Command receive timeout");
  return false;
}
