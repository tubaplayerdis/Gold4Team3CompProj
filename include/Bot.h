#pragma once
#include "vex.h"
#include "Device.h"
#include <vector>
#include <sstream>

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

        //Misc
        static vex::motor MGPM; //Mobile goal post motor
        static vex::motor Intake;
        static vex::motor Jank;

        //Conveyor Motors
        static vex::motor ConveyorTop;
        static vex::motor ConveyorBottom;
        static vex::motor_group ConveyorMotors;
        static bool autoConveyor;

        //Controller Stuff
        static void controllerNotification(std::string notif);

        //Hidden API Compatability
        static void updateDeviceList();
        static int NumDevices;
        static std::vector<Device> DeviceList;

        //setup stuff
        static void setup();
        //Main Loop Function use at task
        static int mainLoop();

};