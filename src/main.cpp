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
#include "Device.h"
#include <string>
#include <vector>
#include <sstream>

using namespace vex;

// A global instance of competition
competition Competition;

// Tasks!
task ControllerLoop = task(Drivetrain::ControllerLoop);
task MainLoop = task(Bot::mainLoop);

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

void pre_auton(void)
{

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

void autonomous(void)
{
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

void usercontrol(void)
{
  // User control code here, inside the loop
  while (true)
  {
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



std::string to_string(int x)
{
  std::stringstream s;
  s << x;
  return s.str();
}

//
// Main will set up the competition functions and callbacks.
//
int main()
{

  // Bot::Brain.Screen.printAt(100, 100, "Begining Setup!");
  //  Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  Bot::updateDeviceList();
  Bot::Brain.Screen.printAt(10, 10, "Device Num: %d", Bot::NumDevices);

  Bot::Brain.Screen.printAt(10, 30, "Device List");
  int ind = 50;
  for (Device obj : Bot::DeviceList)
  {
    Bot::Brain.Screen.printAt(10, ind, obj.toString().c_str());
    ind += 20;
  }

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  // while (true) {
  //  wait(100, msec);
  //}
}
