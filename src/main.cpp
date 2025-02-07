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


#define APPROACH_VELOCITY_PERCENT -35
#define LINEAR_CHANGE_VELOCITY_CORRECTION 0.5


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
  Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
  Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
  Bot::Drivetrain.setStopping(vex::brake);
  Bot::AIVisionF.modelDetection(false);
  Bot::AIVisionF.colorDetection(true);
  Bot::AIVisionF.startAwb();
  //Preload
  Bot::RingsIntaken = 1;
  
  
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


#define ABANDON_ITEM_WIDTH_THRESHOLD 100
#define RING_INTAKEN_WIDTH_THRESHOLD 300

#define MAX_OBJ_TO_TRACK 3 //Cannot be larger than 8

void autonomous(void) {
  //Bot::AIVisionF.startAwb();

  Bot::IgnoreDisplay = true;
  bool isExitAiLoop = false;





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

  //Bot::Drivetrain.driveFor(10, vex::mm, true);


  while (true)
  {
    Bot::IgnoreDisplay = true;

    if(Bot::RingsIntaken >= 6) {
      //Full mobile goal
      break;
    }

    if(isExitAiLoop) break;

    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    vex::timer snpashotTimer;
    snpashotTimer.reset();
    Bot::Controller.Screen.setCursor(3,1);
    Bot::Controller.Screen.print("%02d", snpashotTimer.value());

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    Bot::Controller.Screen.setCursor(1,1);
    Bot::Controller.Screen.print("RING NUM: %f", Bot::RingsIntaken);

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionF.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      vex::this_thread::sleep_for(20);
      continue;
    }

    //Something was found

    Bot::LeftMotors.stop();
    Bot::RightMotors.stop();

    //The AI Vision Sensor has a resolution of 320 x 240 pixels.

    //Turning to face
    Bot::Controller.Screen.setCursor(2,1);
    Bot::Controller.Screen.print("PURSUIT  ");
    //Bot::Drivetrain.setDriveVelocity(35, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(5, vex::percent);

    Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
    Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
    Bot::LeftMotors.spin(vex::forward);
    Bot::RightMotors.spin(vex::forward);
    vex::this_thread::sleep_for(10);

    Bot::Controller.Screen.clearScreen();


    while (true)
    {
      //vex::this_thread::sleep_for(1000);

      //vex::this_thread::sleep_for(10);
      if(Bot::Aliance == Blue) {
        Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
      } else {
        //red
        Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
      }
    

      if(Bot::AIVisionF.objectCount == 0) {
        //Lost Ring
        Bot::Drivetrain.stop();
        break;
      }


      pursuit = Bot::AIVisionF.objects[0];
      for (size_t i = 0; i < 3; i++)
      {
        if(Bot::AIVisionF.objects[i].width > pursuit.width) pursuit = Bot::AIVisionF.objects[i];
      }
      

      bool isTurningtoDriving = false;
      //Center of screen is 160,160.

      if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
      if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);


      Bot::Controller.Screen.setCursor(3,1);
      Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
      
      

      if(pursuit.originX + pursuit.width < 160) {
        Bot::LeftMotors.spin(vex::forward);
        Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)-LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else if (pursuit.originX > 160) {
        Bot::RightMotors.spin(vex::forward);
        Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)-LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else {   
        Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
        Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT, vex::percent);
        if(isTurningtoDriving) {
          //Bot::Drivetrain.stop();
          isTurningtoDriving = false;
        } //Stop turning
        if(pursuit.width >= RING_INTAKEN_WIDTH_THRESHOLD) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.print("ENDING  ");
          Bot::Drivetrain.stop();
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }

  //Drive to corner and throw mobile goal in
  

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


void ToggleLadyBrown() {
  Bot::IgnoreArm = true;
  Bot::Arm.setMaxTorque(100, vex::percent);
  Bot::Arm.setVelocity(100, vex::percent);
  Bot::Arm.spinTo(125, vex::degrees, true);
  Bot::IgnoreArm = false;
  //Bot::desiredARMAngle = LADYBROWN_DESIRED_ANGLE;
  //Bot::isArmPIDActive = !Bot::isArmPIDActive;
  //Bot::Arm.stop();
}

//THIS WAS INCORPERATED INTO THE MAIN LOOP.
int ArmLoop() {
  return 1;
}

void SendBackConveyor() {
  Bot::IgnoreIntake = true;
  Bot::Intake.setVelocity(100, vex::percent);
  Bot::Intake.setMaxTorque(100, vex::percent);
  Bot::Intake.spinFor(70, vex::degrees, true);
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

void ringCounterTripped() {
  Bot::RingsIntaken++;
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

  Bot::Controller.ButtonY.pressed(SendBackConveyor);
  Bot::Controller.ButtonA.pressed(Bot::toggleMogo);
  Bot::Controller.ButtonB.pressed(Bot::toggleDoinker);

  Bot::Controller.ButtonLeft.pressed(Notifications::notifBackward);
  Bot::Controller.ButtonRight.pressed(Notifications::notifForward);

  Bot::Controller.ButtonUp.pressed(IncreaseSelectedPosisitonAuton);
  Bot::Controller.ButtonDown.pressed(ColorDetection::toggleEnabled);

  Bot::RingCounter.pressed(ringCounterTripped);


  vex::task drivetrian(Drivetrain::ControllerLoop);
  vex::task mainLoop(Bot::mainLoop);
  vex::task displayLoop(Bot::displayLoop);
  //vex::task blinkerLoop(Bot::blinkerLoop);
  vex::task colorsensing(ColorDetection::visionTask);
  //vex::task monitoring(Bot::monitorLoop); //part of main loop now
  //Bot::Brain.Screen.printAt(0, 150, "Systems Go!");
  
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  UISystem::setup();
  vex::task uiloop(UISystem::renderLoop);

  Bot::Controller.ButtonX.pressed(ToggleLadyBrown);


  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(101, msec);
  }
}
