#pragma once
#include "vex.h"
#include "api/vexui.h"
#include "Device.h"
#include <vector>
#include <sstream>
#include "macros.h"

//BLUE
  //rgb(88,156,156)

  //RED
  //rgb(225,42,68)

enum aliance {
    Blue = 0,
    Red = 1
};

enum autons {
    Red_Goal_Elim = 0, //Elimination Autons. Works with caspars autons
    Red_Ring_Elim = 1,
    Blue_Goal_Elim = 2,
    Blue_Ring_Elim = 3,

    Red_Goal_AlainceStake = 4, //AWP based autons. use whter or not team has alaince stake or not. All of these should touch bar
    Red_Goal_GoalRush = 5,
    Blue_Goal_AlainceStake = 6,
    Blue_Goal_GoalRush = 7,
    Test = 8
};

enum BlinkTypes {
    Off = 0,
    Solid = 1,
    Single = 2,
    Double = 3
};

typedef struct gameElementPosition 
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

        static autons AutonomusRoutine;

        //Drivetrain Elements
        static vex::controller Controller;
        static vex::smartdrive Drivetrain;
        static vex::motor LeftA;
        static vex::motor LeftB;
        static vex::motor LeftC;
        static vex::motor RightA;
        static vex::motor RightB;
        static vex::motor RightC;
        static vex::motor_group LeftMotors;
        static vex::motor_group RightMotors;

        //Conveyor/Lady Brown
        static vex::motor Conveyor;
        static vex::motor IntakeReal;
        static vex::motor Arm;
        static vex::motor_group Intake;
        //static vex::motor ArmR;
        //static vex::motor_group Arm;
        static bool isArmPIDActive;
        static int desiredARMAngle;

        //3 Wrire Port
        static vex::digital_out MogoMech;
        static vex::digital_out Gripper;
        static vex::digital_out Doinker;
        static vex::digital_out Lift;
        static vex::pot AutonSelect;
        static vex::pot ArmPot;
        static vex::limit GripperSwitch;
        static int RingsIntaken;

        //Gryos and stuff
        static vex::inertial Inertial;
        static vex::rotation RotationForward; //Forward
        static vex::rotation RotationLateral; //Lateral
        static vex::optical ColorSensor;
        static vex::distance DistanceF;
        static vex::distance DistanceC;
        static aliance Aliance;

        //AI
        static vex::aivision::colordesc BLUEDESJ;
        static vex::aivision::colordesc REDDESJ;
        static vex::aivision AIVisionF;
        static vex::aivision::colordesc MOGODESJ;
        static vex::aivision AIVisionM;

        //"Game Posistioning System"
        static vex::gps GpsF;
        static vex::gps GpsL;
        static vex::gps GpsR;
        static vex::gps GpsB;

        static vex::competition Comp;

        //Led Array
        /*
        static vex::led ModeLED;
        static vex::led ControllerInputLED;
        static vex::led NotificationLED;
        static vex::led WarningLED;
        static vex::led CriticalErrorCodeLED;
        static vex::led CriticalErrorLED;
        */


        static bool feedGps;
        static void swapFeedPos();

        //Controller Stuff
        static void controllerNotification(std::string notif);

        //Hidden API Compatability
        static void updateDeviceList();
        static int NumDevices;
        static std::vector<Device> DeviceList;

        //bools for loops
        static bool IgnoreMain;
        static bool IgnoreDisplay;
        static bool IgnoreMonitor;
        static bool IgnoreVision;

        static bool IgnoreArm;
        static bool IgnoreIntake;

        //setup stuf
        static void setup();
        //Main Loop Function use at task
        static int mainLoop();
        //"tasks"
        static bool ClutchToggle, MogoToggle, DoinkerToggle, LiftToggle;
        static void toggleClutch();
        static void toggleMogo();
        static void toggleDoinker();
        static void toggleLift();
        static void switchAlliance();
        static void clampMobileGoal();
        static void releaseMobileGoal();
        static void pushClutch();
        static void releaseClutch();
        static void checkInstall();
        static void checkMonitors();
        static int blinkerLoop();//dont use
        static int displayLoop();
        static int monitorLoop();//dont use



        static int redRingNum;
        static int blueRingNum;
        static int mobileGoalNum;
        static int aiLoop();
        
        static void iterateStartingPosistion();

};