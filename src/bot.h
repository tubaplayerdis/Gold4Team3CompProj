#include "vex.h"

class bot {
    public:
        static vex::controller Controller;
        static vex::drivetrain Drivetrain;
        static vex::motor LeftFront;
        static vex::motor RightFront;
        static vex::motor LeftRear;
        static vex::motor LeftRear;
};

//Define Motors
vex::motor bot::LeftFront = vex::motor(vex::PORT1, true);


//Define important stuff
vex::controller bot::Controller = vex::controller();
vex::drivetrain bot::Drivetrain = vex::drivetrain(/*Add motors and other sussy info*/);