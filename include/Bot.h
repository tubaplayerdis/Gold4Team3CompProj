#pragma once
#include "vex.h"
#include "Device.h"
#include <vector>
#include <sstream>

enum aliance {
    Nuetral = 0,
    Blue = 1,
    Red = 2
};

class Bot {
    public:
        //Brain
        static vex::brain Brain;

        //Drivetrain Elements
        static vex::controller Controller;
        static vex::drivetrain Drivetrain;
        static vex::motor LeftA;
        static vex::motor LeftB;
        static vex::motor LeftC;
        static vex::motor RightA;
        static vex::motor RightB;
        static vex::motor RightC;
        static vex::motor_group LeftMotors;
        static vex::motor_group RightMotors;

        //Misc
        static vex::motor Intake;
        static vex::motor Arm;
        static vex::digital_out MogoMech;

        //Gryos and stuff
        static vex::inertial Inertial;
        static vex::rotation RotationForward; //Forward
        static vex::rotation RotationLateral; //Lateral
        static vex::optical ColorSensor;
        static aliance Aliance;

        //Controller Stuff
        static void controllerNotification(std::string notif);

        //Hidden API Compatability
        static void updateDeviceList();
        static int NumDevices;
        static std::vector<Device> DeviceList;

        //setup stuf
        static void setup();
        //Main Loop Function use at task
        static int mainLoop();
        static void switchAlliance();
        static void clampMobileGoal();
        static void releaseMobileGoal();
        static void checkInstall();
        static int displayLoop();

};