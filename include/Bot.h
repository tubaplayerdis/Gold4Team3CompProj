#pragma once
#include "vex.h"
#include "Device.h"
#include <vector>

class Bot {
    public:
        //Brain
        static vex::brain Brain;

        //Drivetrain Elements
        static vex::controller Controller;
        static vex::drivetrain Drivetrain;
        static vex::motor LeftFront;
        static vex::motor LeftRear;
        static vex::motor RightFront;
        static vex::motor RightRear;
        static vex::motor_group LeftMotors;
        static vex::motor_group RightMotors;

        //Others
        static vex::motor MGPM; //Mobile goal post motor

        //Hidden API Compatability
        static void updateDeviceList();
        static int NumDevices;
        static std::vector<Device> DeviceList;

};