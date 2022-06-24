
#include "joint.h"
#include "board.h"

int main(){

    Board board;
    Joint joint1(1,JointType::revolute,100,150);

    joint1.moveJoint(&board, 145);

}