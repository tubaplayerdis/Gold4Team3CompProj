#pragma once
#include "vex.h"

class Bot {
    public:
        //Drivetrain Elements
        static vex::controller Controller;
        static vex::drivetrain Drivetrain;
        static vex::motor LeftFront;
        static vex::motor RightFront;
        static vex::motor LeftRear;
        static vex::motor RightRear;
        static vex::motor_group LeftMotors;
        static vex::motor_group RightMotors;

        //Others
        static vex::motor MGPM; //Mobile goal post motor

};