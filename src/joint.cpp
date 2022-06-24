
#include "joint.h"
#include "board.h"

Joint::Joint(int jointNum, JointType jointType, int minAngle, int maxAngle ){
    jointType = jointType;
    jointId = jointId;
    minAngle = minAngle;
    maxAngle = maxAngle;
};

//Takes a commanded angle and tries to move the joint to that angle
//Returns a 1 if succesful and a 0 if not
int Joint::moveJoint(Board* board, int commandedAngle){
    if(!((commandedAngle > maxAngle) && (commandedAngle < minAngle))){
        board
    }
}