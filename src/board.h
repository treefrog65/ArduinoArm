
#include <Arduino.h>

enum CommandType{
    SERVO_MOVE_TIME_WRITE = 1,
};

class Board{
    
    public:
        Board();
        int sendCommand(Serial* bus, CommandType command,int numOfParams, int* params);
    
    private:

};