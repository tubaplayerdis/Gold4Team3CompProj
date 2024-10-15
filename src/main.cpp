/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       aaronwilk                                                 */
/*    Created:      10/11/2024, 12:09:17 PM                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "v5_api.h"
#include "Bot.h"
#include "Drivetrain.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace vex;

// A global instance of competition
competition Competition;

//Tasks!
task ControllerLoop = task(Drivetrain::ControllerLoop);



// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  while (true) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

/*
  kDeviceTypeNoSensor        = 0,
  kDeviceTypeMotorSensor     = 2,
  kDeviceTypeLedSensor       = 3,
  kDeviceTypeAbsEncSensor    = 4,
  kDeviceTypeCrMotorSensor   = 5,
  kDeviceTypeImuSensor       = 6,
  kDeviceTypeRangeSensor     = 7, // obsolete
  kDeviceTypeDistanceSensor  = 7,
  kDeviceTypeRadioSensor     = 8,
  kDeviceTypeTetherSensor    = 9,
  kDeviceTypeBrainSensor     = 10,
  kDeviceTypeVisionSensor    = 11,
  kDeviceTypeAdiSensor       = 12,
  kDeviceTypeRes1Sensor      = 13,
  kDeviceTypeRes2Sensor      = 14,
  kDeviceTypeRes3Sensor      = 15,
  kDeviceTypeOpticalSensor   = 16,
  kDeviceTypeMagnetSensor    = 17,
  kDeviceTypeBumperSensor    = 0x40,
  kDeviceTypeGyroSensor      = 0x46,
  kDeviceTypeSonarSensor     = 0x47,
  kDeviceTypeGenericSensor   = 128,
  kDeviceTypeGenericSerial   = 129,
  kDeviceTypeUndefinedSensor = 255
*/

std::string getDeviceTypeString(int type) {
  switch (type)
  {
    case 0:
      return std::string("kDeviceTypeNoSensor");
    case 1:
      return std::string("Unsupported");  // This case was not defined above; keep or modify as needed.
    case 2:
      return std::string("kDeviceTypeMotorSensor");
    case 3:
      return std::string("kDeviceTypeLedSensor");
    case 4:
      return std::string("kDeviceTypeAbsEncSensor");
    case 5:
      return std::string("kDeviceTypeCrMotorSensor");
    case 6:
      return std::string("kDeviceTypeImuSensor");
    case 7:
      return std::string("kDeviceTypeDistanceSensor");  // Note: kDeviceTypeRangeSensor is obsolete.
    case 8:
      return std::string("kDeviceTypeRadioSensor");
    case 9:
      return std::string("kDeviceTypeTetherSensor");
    case 10:
      return std::string("kDeviceTypeBrainSensor");
    case 11:
      return std::string("kDeviceTypeVisionSensor");
    case 12:
      return std::string("kDeviceTypeAdiSensor");
    case 13:
      return std::string("kDeviceTypeRes1Sensor");
    case 14:
      return std::string("kDeviceTypeRes2Sensor");
    case 15:
      return std::string("kDeviceTypeRes3Sensor");
    case 16:
      return std::string("kDeviceTypeOpticalSensor");
    case 17:
      return std::string("kDeviceTypeMagnetSensor");
    case 0x40:
      return std::string("kDeviceTypeBumperSensor");
    case 0x46:
      return std::string("kDeviceTypeGyroSensor");
    case 0x47:
      return std::string("kDeviceTypeSonarSensor");
    case 128:
      return std::string("kDeviceTypeGenericSensor");
    case 129:
      return std::string("kDeviceTypeGenericSerial");
    case 255:
      return std::string("kDeviceTypeUndefinedSensor");
    default:
      return std::string("Unsupported");
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {

  //Bot::Brain.Screen.printAt(100, 100, "Begining Setup!");
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  V5_DeviceTypeBuffer devicel;
  int devicenum = vexDeviceGetStatus(devicel);
  Bot::Brain.Screen.printAt(10, 10, "Device Num: %d", devicenum);
  std::vector<std::string> typelist = std::vector<std::string>();
  for(int i = 0; i < devicenum; i++) {
    std::stringstream stream;
    stream << i;
    typelist.push_back("Device Type: " + getDeviceTypeString(devicel[i]) + " Index: "+ stream.str());
  }

  Bot::Brain.Screen.printAt(10, 30, "Device Type List");
  int ind = 50;
  for(std::string obj : typelist) {
    Bot::Brain.Screen.printAt(10, ind, obj.c_str());
    ind += 20;
  }

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  //while (true) {
  //  wait(100, msec);
  //}
}
