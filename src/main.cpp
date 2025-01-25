/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Aaron Wilk                                                */
/*    Created:      11/6/2024, 3:26:54 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "Bot.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Drivetrain.h"
#include "Notifications.h"
#include "UISystem.h"
#include "Skills.h"

using namespace vex;

// A global instance of competition
competition Competition;


void cycleStartingPosistions() {
  UISystem::SelectedPosition++;
  if(UISystem::SelectedPosition > UISystem::positions.size() -1 || UISystem::SelectedPosition < 0) UISystem::SelectedPosition = 0;
  UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
  UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
  UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
  Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
  UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);
}



vex::task autonTimer(timerAutonController);

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
  Bot::updateDeviceList();
  Bot::setup();
  Bot::Drivetrain.setDriveVelocity(100, vex::percent);
  Bot::Drivetrain.setTurnVelocity(100, vex::percent);
  Bot::Drivetrain.setStopping(vex::coast);
  
  
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

#define ABANDON_ITEM_WIDTH_THRESHOLD 50
#define RING_INTAKEN_WIDTH_THRESHOLD 140


int timerAutonController() {
  while (true)
  {
    char computed[20];
    sprintf(computed, "CLOCK: %2d", Bot::Brain.Timer.value());
    vexControllerTextSet(kControllerMaster, 3, 1, computed);
    vex::this_thread::sleep_for(10);
  }
  return 0;
}

void autonomous(void) {

  Bot::IgnoreDisplay = true;
  Bot::Brain.Timer.reset();
  autonTimer.resume();

  


  //Motor Through the Getting the goal

  /*
  Bot::isArmPIDActive = false;
  Bot::Drivetrain.setDriveVelocity(20, vex::percent);
  Bot::Drivetrain.driveFor(910, vex::mm, true);
  Bot::Drivetrain.setStopping(vex::coast);
  Bot::MogoMech.set(true);
  Bot::Arm.setVelocity(100,vex::percent);
  Bot::Arm.spinTo(-160, vex::degrees);
  Bot::Intake.setVelocity(600, vex::rpm);
  Bot::Intake.spinFor(0.1, vex::seconds);
  */


  while (true)
  {
    Bot::AIVisionF.takeSnapshot(vex::aivision::ALL_AIOBJS);
    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //has pursuit been filled?
    bool hasFirst = false;
    bool didFind = false;

    Bot::Controller.Screen.setCursor(1,1);
    Bot::Controller.Screen.print("OBJ NUM: %i", Bot::AIVisionF.objectCount);

    if(Bot::AIVisionF.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCH  ");
      Bot::Drivetrain.turnFor(5, vex::degrees, true);
      vex::this_thread::sleep_for(20);
      continue;
    }

    Bot::Controller.Screen.setCursor(2,1);
    Bot::Controller.Screen.print("NUM REQ  ");

    for(int i = 0; i < Bot::AIVisionF.objectCount; i++) {
      int id = Bot::AIVisionF.objects[i].id;
      if(id == 0 || id == 1) {
        vex::aivision::object temp = Bot::AIVisionF.objects[i];
        if(temp.width > ABANDON_ITEM_WIDTH_THRESHOLD) {
          if(!hasFirst) { 
            pursuit = temp;
            hasFirst = true;
          }
          else {
            if(temp.width > pursuit.width) {
              pursuit = temp;
              break;
            }
          }
        }
      }
    } 


    Bot::Controller.Screen.setCursor(2,1);
    Bot::Controller.Screen.print("FOUND  ");

    //The AI Vision Sensor has a resolution of 320 x 240 pixels.

    if(hasFirst) {
      //Turning to face
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("PURSUIT  ");
      while (true)
      {
        if(pursuit.centerX < 150) {
          Bot::Drivetrain.turn(vex::left);
        } else if (pursuit.centerX > 170) {
          Bot::Drivetrain.turn(vex::right);
        } else {
          Bot::Drivetrain.drive(reverse);
          if(pursuit.width >= RING_INTAKEN_WIDTH_THRESHOLD) {
            Bot::Drivetrain.driveFor(10, vex::inches, true);
            break;
          }
        }
      }
      
    } else {
      continue;
    }
  }
  

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
  autonTimer.stop();
  Bot::IgnoreDisplay = false;
  // User control code here, inside the loop
  while (1) {
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

const double Kp = 0.5; // Proportional gain
const double Ki = 0.01; // Integral gain
const double Kd = 0.1; // Derivative gain
const double maxPower = 100; // Maximum motor power (percent)

//-----------------------------------------------------------------------

const double tolerance = 1.0; // Allowable error (degrees)

//-----------------------------------------------------------------------

void ToggleLadyBrown() {
  Bot::desiredARMAngle = -22;
  Bot::isArmPIDActive = !Bot::isArmPIDActive;
  //Bot::Arm.stop();
}

int ArmLoop() {

  Bot::isArmPIDActive = true;

  double error = 0.0;       // Current error
  double previousError = 0.0; // Error from the previous step
  double integral = 0.0;    // Accumulated error
  double derivative = 0.0;  // Rate of error change
  double power = 0.0;       // Motor power

  while (true) {

    if(!Bot::isArmPIDActive) continue; 

    // Get the current position of the motor
    double currentAngle = Bot::Arm.position(degrees);

    // Calculate error
    error = Bot::desiredARMAngle - currentAngle;

    // Break the loop if the error is within the tolerance
    if (fabs(error) <= tolerance) {
        Bot::Arm.stop(brakeType::hold);
        Bot::isArmPIDActive = false;
        Bot::Controller.rumble("...");
        continue;
    }

    // Calculate integral and derivative terms
    integral += error; // Accumulate error
    derivative = error - previousError;

    // Calculate PID output
    power = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // Constrain the power to the motor's limits
    if (power > maxPower) power = maxPower;
    if (power < -maxPower) power = -maxPower;

    // Apply power to the motor
    Bot::Arm.spin(forward, power, percent);

    // Save the current error for the next loop
    previousError = error;

    // Small delay to prevent overwhelming the CPU
    vex::task::sleep(20);
  }
  return 0;
}

void SendBackConveyor() {
  Bot::IgnoreIntake = true;
  Bot::Intake.setVelocity(100, vex::percent);
  Bot::Intake.setMaxTorque(100, vex::percent);
  Bot::Intake.spinFor(-100, vex::degrees, true);
  Bot::IgnoreIntake = false;
}

void IncreaseSelectedPosisitonAuton() {
    UISystem::SelectedPosition++;
    if(UISystem::SelectedPosition > UISystem::positions.size() -1 || UISystem::SelectedPosition < 0) UISystem::SelectedPosition = 0;
    UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
    UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
    UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
    Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
    UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);

    switch (UISystem::SelectedPosition)
    {
        case 0:
            Bot::Aliance = Blue;
            break;
        case 1:
            Bot::Aliance = Blue;
            break;
        case 2:
            Bot::Aliance = Red;
            break;
        case 3:
            Bot::Aliance = Red;
            break;
    
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  autonTimer.suspend();
  //Skills::runSkills(1);
  Odometry::setupAndStartOdometry();
  Bot::Aliance = aliance::Blue;

  //Bot::Controller.ButtonY.pressed(cycleStartingPosistions);
  //Bot::Controller.ButtonX.pressed(Bot::swapFeedPos);

  Bot::Controller.ButtonX.pressed(ToggleLadyBrown);
  Bot::Controller.ButtonY.pressed(SendBackConveyor);
  Bot::Controller.ButtonA.pressed(Bot::toggleMogo);
  Bot::Controller.ButtonB.pressed(Bot::toggleDoinker);

  Bot::Controller.ButtonLeft.pressed(Notifications::notifBackward);
  Bot::Controller.ButtonRight.pressed(Notifications::notifForward);

  Bot::Controller.ButtonUp.pressed(IncreaseSelectedPosisitonAuton);
  Bot::Controller.ButtonDown.pressed(ColorDetection::toggleEnabled);


  vex::task drivetrian(Drivetrain::ControllerLoop);
  vex::task mainLoop(Bot::mainLoop);
  vex::task displayLoop(Bot::displayLoop);
  //vex::task blinkerLoop(Bot::blinkerLoop);
  vex::task colorsensing(ColorDetection::visionTask);
  vex::task monitoring(Bot::monitorLoop);
  vex::task ArmPIDLoop(ArmLoop);
  //vex::task aivisionLoop(Bot::aiLoop);
  //Bot::Brain.Screen.printAt(0, 150, "Systems Go!");
  
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);


  // Run the pre-autonomous function.
  pre_auton();

  UISystem::setup();
  vex::task uiloop(UISystem::renderLoop);

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(101, msec);
  }
}
