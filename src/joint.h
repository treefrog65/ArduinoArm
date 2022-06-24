
enum JointType{
    revolute = 0,
    prismatic = 1,
};

class Joint{

    public:
        Joint(int jointId, JointType jointType, int minAngle, int maxAngle);

        int moveJoint(Board*, int);
        
        int moveJointTimed(int,double);

    private:
        JointType jointType;
        int jointId;
        int minAngle;
        int maxAngle;
};