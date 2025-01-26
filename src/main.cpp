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
  Bot::Drivetrain.setDriveVelocity(45, vex::percent);
  Bot::Drivetrain.setTurnVelocity(10, vex::percent);
  Bot::Drivetrain.setStopping(vex::brake);
  
  
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

int percentageFromRange(int input, int lower, int upper) {
  return 100 * ((upper - input)/(upper-lower));
}

int capPercentage(int percentage, int cap) {
  if(percentage > cap) {
    return cap;
  }
  return cap;
}


#define ABANDON_ITEM_WIDTH_THRESHOLD 20
#define RING_INTAKEN_WIDTH_THRESHOLD 240

void autonomous(void) {

  Bot::IgnoreDisplay = true;
  Bot::AIVisionF.modelDetection(false);
  Bot::AIVisionF.colorDetection(true);
  //Bot::AIVisionF.startAwb();

  


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

  Bot::Drivetrain.driveFor(10, vex::mm, true);

  while (true)
  {
    Bot::Drivetrain.stop();
    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, 1);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, 1);
    }
    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    Bot::Controller.Screen.clearScreen();
    Bot::Controller.Screen.setCursor(1,1);
    Bot::Controller.Screen.print("OBJ NUM: %f", Bot::AIVisionF.objectCount);

    if(Bot::AIVisionF.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      //Bot::Drivetrain.turnFor(5, vex::degrees, true);
      vex::this_thread::sleep_for(70);
      continue;
    }

    //The AI Vision Sensor has a resolution of 320 x 240 pixels.

    //Turning to face
    Bot::Controller.Screen.setCursor(2,1);
    Bot::Controller.Screen.print("PURSUIT  ");
    while (true)
    {
      //vex::this_thread::sleep_for(10);
      if(Bot::Aliance == Blue) {
        Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, 1);
      } else {
        //red
        Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, 1);
      }

      if(Bot::AIVisionF.objectCount == 0) {
        //Lost Ring
        Bot::Drivetrain.stop();
        break;
      }

      pursuit = Bot::AIVisionF.objects[0];

      bool isTurningtoDriving = false;

      //Center of screen is 160,160.
      if(pursuit.originX + pursuit.width < 160) {
        Bot::Drivetrain.turn(vex::left);
        isTurningtoDriving = true;
      } else if (pursuit.originX > 160) {
        Bot::Drivetrain.turn(vex::right);
        isTurningtoDriving = true;
      } else {   
        if(isTurningtoDriving) {
          Bot::Drivetrain.stop();
          isTurningtoDriving = false;
        } //Stop turning
        Bot::Drivetrain.drive(reverse);
        if(pursuit.width >= RING_INTAKEN_WIDTH_THRESHOLD) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.print("ENDING  ");
          Bot::Drivetrain.driveFor(-10, vex::inches, true);
          Bot::Drivetrain.stop();
          break;
        }        
      }
    }
  }
  

  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................
}

/*
if(pursuit.centerX < 130) {
          Bot::Drivetrain.setTurnVelocity(15, vex::percent);
        } else {
          //[130 - 160]
          //percentage = 100 * ((upper - x)/(upper - lower))
          Bot::Drivetrain.setTurnVelocity(capPercentage(percentageFromRange(pursuit.centerX, 130, 160), 15) ,vex::percent);
        }
        Bot::Drivetrain.turn(vex::left);
        isTurningtoDriving = true;
      } else if (pursuit.centerX > 180) {
        if(pursuit.centerX > 190) {
          Bot::Drivetrain.setTurnVelocity(15, vex::percent);
        } else {
          //[180 - 160]
          //percentage = 100 * ((upper - x)/(upper - lower))
          Bot::Drivetrain.setTurnVelocity(capPercentage(percentageFromRange(pursuit.centerX, 180, 160), 15) ,vex::percent);
        }
        Bot::Drivetrain.turn(vex::right);
        isTurningtoDriving = true;


*/

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

  Bot::isArmPIDActive = false;

  double error = 0.0;       // Current error
  double previousError = 0.0; // Error from the previous step
  double integral = 0.0;    // Accumulated error
  double derivative = 0.0;  // Rate of error change
  double power = 0.0;       // Motor power

  while (true) {

    if(!Bot::isArmPIDActive) {
      vex::this_thread::sleep_for(50);
      continue; 
    }

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
  //vex::task colorsensing(ColorDetection::visionTask);
  vex::task monitoring(Bot::monitorLoop);
  //vex::task ArmPIDLoop(ArmLoop);
  //vex::task aivisionLoop(Bot::aiLoop);
  //Bot::Brain.Screen.printAt(0, 150, "Systems Go!");
  
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  UISystem::setup();
  vex::task uiloop(UISystem::renderLoop);


  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(101, msec);
  }
}
