#pragma once
#include "vex.h"
#include "api/vexui.h"
#include "Device.h"
#include <vector>
#include <sstream>

enum aliance {
    Nuetral = 0,
    Blue = 1,
    Red = 2
};

struct gameElementPosition 
{
    gameElementPosition(double x_, double y_, vexui::gameElements objtype_) {
        x = x_;
        y = y_;
        objtype = objtype_;
    }

    double x;
    double y;
    vexui::gameElements objtype;
};


inline const char* getGameElementNameFromID(int32_t id) {
    switch (id)
    { 
    case 0:
        return "0";
    case 1:
        return "1";
    case 2:
        return "2";
    
    default:
        return "-1";
    }
}

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
        //static vex::motor LiftL;
        //static vex::motor LiftR;
        //static vex::motor_group LiftMotors;
        static vex::digital_out MogoMech;
        static vex::digital_out Clutch;
        static vex::limit MogoLimitSwitch;

        //Gryos and stuff
        static vex::inertial Inertial;
        static vex::rotation RotationForward; //Forward
        static vex::rotation RotationLateral; //Lateral
        static vex::optical ColorSensor;
        static aliance Aliance;
        static vex::aivision AIVisionF;

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
        //"tasks"
        static void switchAlliance();
        static void clampMobileGoal();
        static void releaseMobileGoal();
        static void pushClutch();
        static void releaseClutch();
        static void checkInstall();
        static int displayLoop();
        static int monitorLoop();



        static int redRingNum;
        static int blueRingNum;
        static int mobileGoalNum;
        static int aiLoop();

};