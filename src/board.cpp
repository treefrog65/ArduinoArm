
#include "board.h"

Board::Board(HardwareSerial *bus){
  port = bus;
};

int Board::debug(String message){
  port->println(message);
}

uint8_t Board::checkSum(uint8_t bufferLength, uint8_t *buffer) {
  // Initalize chksum to 0 for calculation
  uint8_t cksum = 0;

  // Loop through buffer to calculate checksum
  for (int i = 2; i < bufferLength - 1; i++) {
    cksum += buffer[i];
  }
  buffer[bufferLength - 1] = ~cksum;
};

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
  checkSum(bufferLength,buffer);

  //Transmit the command byte-wise over the serial bus
  port->write(buffer, bufferLength);

};
