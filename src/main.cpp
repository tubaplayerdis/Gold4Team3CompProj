/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Aaron Wilk                                                */
/*    Created:      11/6/2024, 3:26:54 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//end of season.


#include "vex.h"
#include "Bot.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Drivetrain.h"
#include "Notifications.h"
#include "UISystem.h"
#include "Skills.h"
#include "PID.h"

using namespace vex;

// A global instance of competition
competition Competition;

void selectAutonBasedOfPotentiometer() {
  double a = Bot::AutonSelect.angle(vex::degrees);
  if(a >= 0 && a < 33.75) Bot::AutonomusRoutine = Red_Goal_Elim;
  else if(a >= 33.75 && a < 67.5) Bot::AutonomusRoutine = Red_Goal_AlainceStake;
  else if(a >= 67.5 && a < 101.25) Bot::AutonomusRoutine = Red_Goal_GoalRush;
  else if(a >= 101.25 && a < 135) Bot::AutonomusRoutine = Red_Ring_Elim;
  else if(a >= 135 && a < 168.75) Bot::AutonomusRoutine = Blue_Goal_Elim;
  else if(a >= 168.75 && a < 202.5) Bot::AutonomusRoutine = Blue_Goal_AlainceStake;
  else if(a >= 202.5 && a < 236.25) Bot::AutonomusRoutine = Blue_Goal_GoalRush;
  else if(a >= 236.25 && a < 240) Bot::AutonomusRoutine = Blue_Ring_Elim;
  else if(a >= 240 && a < 250) Bot::AutonomusRoutine = Test;
  //250 is the max for some reason.
}


void cycleStartingPosistions() {
  UISystem::SelectedPosition++;
  if(UISystem::SelectedPosition > UISystem::positions.size() -1 || UISystem::SelectedPosition < 0) UISystem::SelectedPosition = 0;
  UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
  UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
  UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
  Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
  UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);
}



void turnCorrectToMogo() {
  Bot::IgnoreDisplay = true;
  while (true)
  {
    Bot::Controller.Screen.clearScreen();

    Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, 3);

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionM.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      if(Bot::Aliance == Blue) turnForPID(5); else turnForPID(-5);
      //vex::this_thread::sleep_for(20);
      continue;
    }

    while (true)
    {
      //vex::this_thread::sleep_for(1000);

      //vex::this_thread::sleep_for(10);
      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, 3);
    

      if(Bot::AIVisionM.objectCount == 0) {
        //Lost Goal
        Bot::Drivetrain.stop();
        break;
      }


      pursuit = Bot::AIVisionM.objects[0];
      for (size_t i = 0; i < 3; i++)
      {
        if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
      }
      
      if(pursuit.centerX < 152) {
        turnForPID(-3);
        continue;
      } else if (pursuit.centerX > 168) {
        turnForPID(3);
        continue;
      } else {   
          return;
      }    
    }
  }
}



#define APPROACH_VELOCITY_PERCENT -25
#define LINEAR_CHANGE_VELOCITY_CORRECTION 0.65

#define APPROACH_VELOCITY_PERCENT_MOGO 35


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

  //Setup AIVision Sensors
  Bot::AIVisionF.modelDetection(false);
  Bot::AIVisionF.colorDetection(true);
  Bot::AIVisionF.startAwb();
  Bot::AIVisionM.modelDetection(false);
  Bot::AIVisionM.colorDetection(true);
  Bot::AIVisionM.startAwb();
  ColorDetection::isEnabled = true;
  //Preload
  Bot::RingsIntaken = 1;
  
  
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

void turnForEnhanced(double amount) {
  turnForPID(amount);
}

void turnToAccounting(double amount) {
  switch (Bot::AutonomusRoutine)
  {
    case 0 ... 1:
        turnToPID(360 - amount);
        break;
    case 4 ... 5:
        turnToPID(360 - amount);
        break;
    case 2 ... 3:
        turnToPID(amount);
        break;
    case 6 ... 7:
        turnToPID(amount);
        break;
  
  }
}

void turnForAccounting(double amount) {
  switch (Bot::AutonomusRoutine)
  {
    case 0 ... 1:
        turnForPID(-1*amount);
        break;
    case 4 ... 5:
        turnForPID(-1*amount);
        break;
    case 2 ... 3:
        turnForPID(amount);
        break;
    case 6 ... 7:
        turnForPID(amount);
        break;
  
  }
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
#define RING_INTAKEN_WIDTH_THRESHOLD 180 //2/3 of the screen

#define BOT_SLOWDOWN_DISTANCE 200

#define MAX_OBJ_TO_TRACK 3 //Cannot be larger than 8


void redGoalElim() {
    
  #pragma region Setup
  bool isExitAiLoop = false;
  //drive full speed till 100mm
  Bot::Drivetrain.setDriveVelocity(35, percent);
  while(Bot::DistanceF.objectDistance(vex::mm) > BOT_SLOWDOWN_DISTANCE) {
    Bot::Drivetrain.drive(vex::forward);
  }

  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("SLOWING!");

  //Slow down and drive for another 100mm
  Bot::Drivetrain.setDriveVelocity(20, percent);
  Bot::Drivetrain.drive(vex::forward);
  vex::this_thread::sleep_for(700);
  Bot::MogoToggle = true;
  Bot::MogoMech.set(true);
  vex::this_thread::sleep_for(200);
  Bot::Drivetrain.stop();

  Bot::Controller.Screen.clearLine(2);
  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("STARTING API:A");
  
  //Turn to face ring
  Bot::Drivetrain.setTurnVelocity(5, vex::percent);
  turnToAccounting(325);

  Bot::Intake.setMaxTorque(100, vex::percent);
  Bot::Intake.setVelocity(600, vex::rpm);
  Bot::Intake.spin(vex::forward);

  Bot::IgnoreDisplay = true;
  
  #pragma endregion

  #pragma region PAIA
  while (true)
  {

    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

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
        if(pursuit.width >= 120) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.stop();
          Bot::LiftToggle = true;
          Bot::Lift.set(true);
          Bot::Drivetrain.setDriveVelocity(4, vex::percent);
          Bot::Drivetrain.drive(reverse);
          Bot::Drivetrain.driveFor(-200, vex::mm, true);
          Bot::Drivetrain.driveFor(300, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }
  #pragma endregion PAIA
  //Got First Ring
  isExitAiLoop = false; 

  Bot::LiftToggle = false;
  Bot::Lift.set(false);
  vex::this_thread::sleep_for(200);
  turnToAccounting(110);

  #pragma region PAIB

  int searches = 0;

  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionF.objectCount == 0) {
      if(searches > 20) break;
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      vex::this_thread::sleep_for(20);
      searches++;
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
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.setDriveVelocity(35, vex::percent);
          Bot::Drivetrain.driveFor(-500, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }


  #pragma endregion

  isExitAiLoop = false;
  vex::this_thread::sleep_for(300);//allow ring to not be heaved off bot during turn
  Bot::Drivetrain.setTurnVelocity(15, vex::percent);
  turnForAccounting(-100);
  Bot::DoinkerToggle = true;
  Bot::Doinker.set(true);

  #pragma region PAIC

  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

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
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("DOINKING");
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }

  #pragma endregion

  Bot::Drivetrain.setTurnVelocity(35, percent);
  turnToAccounting(240);

  #pragma region PAID

  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

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
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.setDriveVelocity(35, vex::percent);
          Bot::Drivetrain.driveFor(-300, vex::mm, true);
          //isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }  

  #pragma endregion PAID
  //Drive to corner and throw mobile goal in

}

void redGoalAlianceStake() {
  while(true) {
    if(!Bot::Inertial.isCalibrating()) break;
    this_thread::sleep_for(1);
  }
  Bot::Drivetrain.driveFor(-6.9, inches, true);
  turnToPID(90);
  Bot::Drivetrain.driveFor(2, inches, true);
  Bot::Conveyor.setVelocity(100, percent);
  Bot::Conveyor.spinFor(1.5, seconds);
  Bot::Drivetrain.driveFor(-3, inches, true);
  turnToPID(310);



  #pragma region PAIA

  Bot::IgnoreDisplay = true;
  bool isExitAiLoop = false;
  while (true)
  {

    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionM.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      Bot::Drivetrain.driveFor(5, inches, true);
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

    Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::LeftMotors.spin(vex::forward);
    Bot::RightMotors.spin(vex::forward);
    vex::this_thread::sleep_for(10);

    Bot::Controller.Screen.clearScreen();

    while (true)
    {
      //vex::this_thread::sleep_for(1000);

      //vex::this_thread::sleep_for(10);
      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);
    

      if(Bot::AIVisionM.objectCount == 0) {
        //Lost Ring
        Bot::Drivetrain.stop();
        break;
      }


      pursuit = Bot::AIVisionM.objects[0];
      for (size_t i = 0; i < 3; i++)
      {
        if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
      }
      

      bool isTurningtoDriving = false;
      //Center of screen is 160,160.

      if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);


      Bot::Controller.Screen.setCursor(3,1);
      Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
      
      if(Bot::DistanceF.objectDistance(mm) < 30) {
        this_thread::sleep_for(200);
        Bot::MogoMech.set(true);
        this_thread::sleep_for(200);
        Bot::Drivetrain.stop();
        isExitAiLoop = true;
        break;
      }

      if(pursuit.originX + pursuit.width < 160) {
        Bot::LeftMotors.spin(vex::forward);
        Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else if (pursuit.originX > 160) {
        Bot::RightMotors.spin(vex::forward);
        Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else {   
        Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        if(isTurningtoDriving) {
          //Bot::Drivetrain.stop();
          isTurningtoDriving = false;
        } //Stop turning
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("DONE");
          Bot::Drivetrain.setDriveVelocity(25, percent);
          Bot::Drivetrain.drive(forward);
          this_thread::sleep_for(300);
          Bot::MogoMech.set(true);
          Bot::MogoToggle = true;
          this_thread::sleep_for(300);
          Bot::Drivetrain.stop();
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }

    //turnToPID(90);
  }

  #pragma endregion

  Bot::Drivetrain.driveFor(-400, mm, true);
  turnToPID(155);
  Bot::Intake.spin(forward);

  Bot::Controller.Screen.clearScreen();
  #pragma region PAIB

  
  int searches = 0;
  isExitAiLoop = false;
  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionF.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      vex::this_thread::sleep_for(20);
      turnForPID(-2);
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
        if(pursuit.width >= 210) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.setDriveVelocity(35, vex::percent);
          Bot::Drivetrain.driveFor(-500, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }


  #pragma endregion


  turnToPID(90);
}

//Untested and might not work. run alaince stake 
void redGoalGoalRush() {
  ColorDetection::isEnabled = false;
  //Bot::Doinker.set(true);
  Bot::Drivetrain.setDriveVelocity(50, percent);
  Bot::IntakeReal.setVelocity(100, vex::pct);
  Bot::IntakeReal.spin(vex::forward);
  bool gotogal = false;
  for(int i = 0; i < 700; i++) {
    if(i == 300) Bot::Doinker.set(true);
    Bot::Drivetrain.drive(reverse);
    if(Bot::GripperSwitch.pressing()) {
      Bot::Gripper.set(true);
      Bot::Drivetrain.drive(forward);
      gotogal = true;
      break;
    }
    vex::this_thread::sleep_for(1);
  }
  Bot::Drivetrain.driveFor(600, vex::mm, true);
  if(gotogal) {
    Bot::Gripper.set(false);
    Bot::Drivetrain.driveFor(100, vex::mm, true);
    turnForAccounting(205);//do a flip!
    Bot::Doinker.set(false);

    Bot::Drivetrain.setDriveVelocity(15, percent);
    for(int i = 0; i < 1000; i++) {
      if(Bot::DistanceF.objectDistance(vex::mm) < 25) {
        Bot::Drivetrain.drive(forward);
        vex::this_thread::sleep_for(300);
        Bot::MogoMech.set(true);
        Bot::MogoToggle = true;
        Bot::Drivetrain.drive(forward);
        vex::this_thread::sleep_for(200);
        Bot::Drivetrain.stop();
        break;
      }
      Bot::Drivetrain.drive(vex::forward);
      vex::this_thread::sleep_for(1);
    }
    
    #pragma region PAIGOAL
    /*
    Bot::IgnoreDisplay = true;
    bool isExitAiLoop = false;
    while (true)
    {

      if(isExitAiLoop) {
        isExitAiLoop = true;
        break;
      }


      Bot::Controller.Screen.clearScreen();

      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);

      //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
      vex::aivision::object pursuit = vex::aivision::object();

      //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
      //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


      if(Bot::AIVisionM.objectCount == 0) {
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

      Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      Bot::LeftMotors.spin(vex::forward);
      Bot::RightMotors.spin(vex::forward);
      vex::this_thread::sleep_for(10);

      Bot::Controller.Screen.clearScreen();

      while (true)
      {
        //vex::this_thread::sleep_for(1000);

        //vex::this_thread::sleep_for(10);
        Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);
      

        if(Bot::AIVisionM.objectCount == 0) {
          //Lost Ring
          Bot::Drivetrain.stop();
          break;
        }


        pursuit = Bot::AIVisionM.objects[0];
        for (size_t i = 0; i < 3; i++)
        {
          if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
        }
        

        bool isTurningtoDriving = false;
        //Center of screen is 160,160.

        if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);


        Bot::Controller.Screen.setCursor(3,1);
        Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
        
        

        if(pursuit.originX + pursuit.width < 160) {
          Bot::LeftMotors.spin(vex::forward);
          Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)+(LINEAR_CHANGE_VELOCITY_CORRECTION+0.3), vex::percent);
          continue;
        } else if (pursuit.originX > 160) {
          Bot::RightMotors.spin(vex::forward);
          Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)+(LINEAR_CHANGE_VELOCITY_CORRECTION+0.3), vex::percent);
          continue;
        } else {   
          Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
          Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
          if(isTurningtoDriving) {
            //Bot::Drivetrain.stop();
            isTurningtoDriving = false;
          } //Stop turning
          if(pursuit.width >= 200) {
            Bot::Controller.Screen.setCursor(2,1);
            Bot::Controller.Screen.clearLine(2);
            Bot::Controller.Screen.print("DONE");
            Bot::Drivetrain.setDriveVelocity(20, percent);
            Bot::Drivetrain.drive(forward);
            this_thread::sleep_for(400);
            Bot::MogoMech.set(true);
            Bot::MogoToggle = true;
            this_thread::sleep_for(400);
            Bot::Drivetrain.stop();
            isExitAiLoop = true;
            break;
          }    

          
            
        }

        
      }
    }
    */
    #pragma endregion
    ColorDetection::isEnabled = false;
    //Bot::Drivetrain.driveFor(-500, mm, true);
    //Bot::IntakeReal.stop();
    Bot::Conveyor.setVelocity(100, vex::percent);
    Bot::Conveyor.spin(forward);
    vex::this_thread::sleep_for(550);
    Bot::Conveyor.stop();
    Bot::MogoMech.set(false);
    Bot::MogoToggle = false;
    Bot::Conveyor.spinFor(reverse, 0.2, vex::seconds);


    turnForAccounting(100);
    turnCorrectToMogo();
    turnArmToPID(69.7);

    for(int i = 0; i < 1000; i++) {
      if(Bot::DistanceF.objectDistance(vex::mm) < 30) {
        Bot::Drivetrain.driveFor(100, mm, true);
        Bot::MogoMech.set(true);
        Bot::MogoToggle = true;
        Bot::Drivetrain.driveFor(100, mm, true);
        Bot::Drivetrain.stop();
        break;
      }
      Bot::Drivetrain.drive(vex::forward);
      vex::this_thread::sleep_for(1);
    }
    Bot::Drivetrain.setDriveVelocity(15, percent);
    Bot::Drivetrain.driveFor(-300, mm, true);
    Bot::Intake.spin(forward);
    this_thread::sleep_for(1000);
    turnArmToPID(131);
    turnToAccounting(79);
    Bot::Drivetrain.setDriveVelocity(15, percent);
    while(true) {
      vex::this_thread::sleep_for(1);
      Bot::Drivetrain.drive(vex::reverse);
    }

  } else {
      turnCorrectToMogo();
      for(int i = 0; i < 5000; i++) {
        if(Bot::DistanceF.objectDistance(vex::mm) < 30) {
          Bot::Drivetrain.driveFor(100, mm, true);
          Bot::MogoMech.set(true);
          Bot::MogoToggle = true;
          Bot::Drivetrain.driveFor(100, mm, true);
          Bot::Drivetrain.stop();
          while(true) {
            Bot::Intake.spin(forward);  
          }
          Bot::Intake.spin(forward);
          break;
        }
        Bot::Drivetrain.drive(vex::forward);
        vex::this_thread::sleep_for(1);
      }
  }
}

void redRingElim() {
  Bot::Drivetrain.setDriveVelocity(35, percent);
  while(Bot::DistanceF.objectDistance(vex::mm) > BOT_SLOWDOWN_DISTANCE) {
    Bot::Drivetrain.drive(vex::forward);
  }

  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("SLOWING!");

  //Slow down and drive for another 100mm
  Bot::Drivetrain.setDriveVelocity(20, percent);
  Bot::Drivetrain.drive(vex::forward);
  vex::this_thread::sleep_for(700);
  Bot::MogoToggle = true;
  Bot::MogoMech.set(true);
  vex::this_thread::sleep_for(200);
  Bot::Drivetrain.stop();
  Bot::Intake.setVelocity(100, vex::percent);
  while(true) {
    Bot::Intake.spin(forward);
  }
}

void blueGoalElim() {

  #pragma region Setup
  bool isExitAiLoop = false;
  //drive full speed till 100mm
  Bot::Drivetrain.setDriveVelocity(35, percent);
  while(Bot::DistanceF.objectDistance(vex::mm) > BOT_SLOWDOWN_DISTANCE) {
    Bot::Drivetrain.drive(vex::forward);
  }

  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("SLOWING!");

  //Slow down and drive for another 100mm
  Bot::Drivetrain.setDriveVelocity(20, percent);
  Bot::Drivetrain.drive(vex::forward);
  vex::this_thread::sleep_for(700);
  Bot::MogoToggle = true;
  Bot::MogoMech.set(true);
  vex::this_thread::sleep_for(200);
  Bot::Drivetrain.stop();

  Bot::Controller.Screen.clearLine(2);
  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("STARTING API:A");
  
  //Turn to face ring
  Bot::Drivetrain.setTurnVelocity(5, vex::percent);
  turnToAccounting(325);

  Bot::Intake.setMaxTorque(100, vex::percent);
  Bot::Intake.setVelocity(600, vex::rpm);
  Bot::Intake.spin(vex::forward);

  Bot::IgnoreDisplay = true;
  
  #pragma endregion

  #pragma region PAIA
  while (true)
  {

    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

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
        if(pursuit.width >= 120) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.stop();
          Bot::LiftToggle = true;
          Bot::Lift.set(true);
          Bot::Drivetrain.setDriveVelocity(4, vex::percent);
          Bot::Drivetrain.drive(reverse);
          Bot::Drivetrain.driveFor(-200, vex::mm, true);
          Bot::Drivetrain.driveFor(300, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }
  #pragma endregion PAIA
  //Got First Ring
  isExitAiLoop = false; 

  Bot::LiftToggle = false;
  Bot::Lift.set(false);
  vex::this_thread::sleep_for(200);
  turnToAccounting(110);

  #pragma region PAIB

  int searches = 0;

  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionF.objectCount == 0) {
      if(searches > 20) break;
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      vex::this_thread::sleep_for(20);
      searches++;
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
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.setDriveVelocity(35, vex::percent);
          Bot::Drivetrain.driveFor(-500, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }


  #pragma endregion

  isExitAiLoop = false;
  vex::this_thread::sleep_for(300);//allow ring to not be heaved off bot during turn
  Bot::Drivetrain.setTurnVelocity(15, vex::percent);
  turnForAccounting(-100);
  //Bot::DoinkerToggle = true;
  //Bot::Doinker.set(true);

  #pragma region PAIC

  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

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
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("DOINKING");
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }

  #pragma endregion

  Bot::Drivetrain.setTurnVelocity(35, percent);
  turnToAccounting(240);

  #pragma region PAID
}

void blueGoalAlainceStake() {
  while(true) {
    if(!Bot::Inertial.isCalibrating()) break;
    this_thread::sleep_for(1);
  }
  Bot::Drivetrain.driveFor(-6.9, inches, true);
  turnToPID(360-90);
  Bot::Drivetrain.driveFor(2, inches, true);
  Bot::Conveyor.setVelocity(100, percent);
  Bot::Conveyor.spinFor(1.5, seconds);
  Bot::Drivetrain.driveFor(-3, inches, true);
  turnToPID(360-310);



  #pragma region PAIA

  Bot::IgnoreDisplay = true;
  bool isExitAiLoop = false;
  while (true)
  {

    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionM.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      Bot::Drivetrain.driveFor(5, inches, true);
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

    Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::LeftMotors.spin(vex::forward);
    Bot::RightMotors.spin(vex::forward);
    vex::this_thread::sleep_for(10);

    Bot::Controller.Screen.clearScreen();

    while (true)
    {
      //vex::this_thread::sleep_for(1000);

      //vex::this_thread::sleep_for(10);
      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);
    

      if(Bot::AIVisionM.objectCount == 0) {
        //Lost Ring
        Bot::Drivetrain.stop();
        break;
      }


      pursuit = Bot::AIVisionM.objects[0];
      for (size_t i = 0; i < 3; i++)
      {
        if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
      }
      

      bool isTurningtoDriving = false;
      //Center of screen is 160,160.

      if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);


      Bot::Controller.Screen.setCursor(3,1);
      Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
      
      if(Bot::DistanceF.objectDistance(mm) < 30) {
        this_thread::sleep_for(200);
        Bot::MogoMech.set(true);
        this_thread::sleep_for(200);
        Bot::Drivetrain.stop();
        isExitAiLoop = true;
        break;
      }

      if(pursuit.originX + pursuit.width < 160) {
        Bot::LeftMotors.spin(vex::forward);
        Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else if (pursuit.originX > 160) {
        Bot::RightMotors.spin(vex::forward);
        Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else {   
        Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        if(isTurningtoDriving) {
          //Bot::Drivetrain.stop();
          isTurningtoDriving = false;
        } //Stop turning
        if(pursuit.width >= 180) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("DONE");
          Bot::Drivetrain.setDriveVelocity(25, percent);
          Bot::Drivetrain.drive(forward);
          this_thread::sleep_for(300);
          Bot::MogoMech.set(true);
          Bot::MogoToggle = true;
          this_thread::sleep_for(300);
          Bot::Drivetrain.stop();
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }

    //turnToPID(90);
  }

  #pragma endregion

  Bot::Drivetrain.driveFor(-400, mm, true);
  turnToPID(360-155);
  Bot::Intake.spin(forward);

  Bot::Controller.Screen.clearScreen();
  #pragma region PAIB

  
  int searches = 0;
  isExitAiLoop = false;
  while (true)
  {
    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    //Defualt blue
    if(Bot::Aliance == Blue) {
      Bot::AIVisionF.takeSnapshot(Bot::BLUEDESJ, MAX_OBJ_TO_TRACK);
    } else {
      //red
      Bot::AIVisionF.takeSnapshot(Bot::REDDESJ, MAX_OBJ_TO_TRACK);
    }

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionF.objectCount == 0) {
      Bot::Controller.Screen.setCursor(2,1);
      Bot::Controller.Screen.print("SEARCHING  ");
      vex::this_thread::sleep_for(20);
      turnForPID(-2);
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
        if(pursuit.width >= 210) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("INTAKING");
          Bot::Drivetrain.setDriveVelocity(35, vex::percent);
          Bot::Drivetrain.driveFor(-500, vex::mm, true);
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }


  #pragma endregion


  turnToPID(360-90);
}

//Done
void blueGoalGoalRush() {
  ColorDetection::isEnabled = false;
  //Bot::Doinker.set(true);
  Bot::Drivetrain.setDriveVelocity(50, percent);
  Bot::IntakeReal.setVelocity(100, vex::pct);
  Bot::IntakeReal.spin(vex::forward);
  bool gotogal = false;
  for(int i = 0; i < 650; i++) {
    if(i == 300) Bot::Doinker.set(true);
    Bot::Drivetrain.drive(reverse);
    if(Bot::GripperSwitch.pressing()) {
      Bot::Gripper.set(true);
      Bot::Drivetrain.drive(forward);
      gotogal = true;
      break;
    }
    vex::this_thread::sleep_for(1);
  }
  Bot::Drivetrain.driveFor(600, vex::mm, true);
  if(gotogal) {
    Bot::Gripper.set(false);
    Bot::Drivetrain.driveFor(100, vex::mm, true);
    turnForAccounting(195);//do a flip!
    Bot::Doinker.set(false);

    Bot::Drivetrain.setDriveVelocity(15, percent);
    for(int i = 0; i < 1000; i++) {
      if(Bot::DistanceF.objectDistance(vex::mm) < 25) {
        Bot::Drivetrain.drive(forward);
        vex::this_thread::sleep_for(300);
        Bot::MogoMech.set(true);
        Bot::MogoToggle = true;
        Bot::Drivetrain.drive(forward);
        vex::this_thread::sleep_for(200);
        Bot::Drivetrain.stop();
        break;
      }
      Bot::Drivetrain.drive(vex::forward);
      vex::this_thread::sleep_for(1);
    }
    
    #pragma region PAIGOAL
    /*
    Bot::IgnoreDisplay = true;
    bool isExitAiLoop = false;
    while (true)
    {

      if(isExitAiLoop) {
        isExitAiLoop = true;
        break;
      }


      Bot::Controller.Screen.clearScreen();

      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);

      //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
      vex::aivision::object pursuit = vex::aivision::object();

      //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
      //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


      if(Bot::AIVisionM.objectCount == 0) {
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

      Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      Bot::LeftMotors.spin(vex::forward);
      Bot::RightMotors.spin(vex::forward);
      vex::this_thread::sleep_for(10);

      Bot::Controller.Screen.clearScreen();

      while (true)
      {
        //vex::this_thread::sleep_for(1000);

        //vex::this_thread::sleep_for(10);
        Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);
      

        if(Bot::AIVisionM.objectCount == 0) {
          //Lost Ring
          Bot::Drivetrain.stop();
          break;
        }


        pursuit = Bot::AIVisionM.objects[0];
        for (size_t i = 0; i < 3; i++)
        {
          if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
        }
        

        bool isTurningtoDriving = false;
        //Center of screen is 160,160.

        if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);


        Bot::Controller.Screen.setCursor(3,1);
        Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
        
        

        if(pursuit.originX + pursuit.width < 160) {
          Bot::LeftMotors.spin(vex::forward);
          Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)+(LINEAR_CHANGE_VELOCITY_CORRECTION+0.3), vex::percent);
          continue;
        } else if (pursuit.originX > 160) {
          Bot::RightMotors.spin(vex::forward);
          Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)+(LINEAR_CHANGE_VELOCITY_CORRECTION+0.3), vex::percent);
          continue;
        } else {   
          Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
          Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
          if(isTurningtoDriving) {
            //Bot::Drivetrain.stop();
            isTurningtoDriving = false;
          } //Stop turning
          if(pursuit.width >= 200) {
            Bot::Controller.Screen.setCursor(2,1);
            Bot::Controller.Screen.clearLine(2);
            Bot::Controller.Screen.print("DONE");
            Bot::Drivetrain.setDriveVelocity(20, percent);
            Bot::Drivetrain.drive(forward);
            this_thread::sleep_for(400);
            Bot::MogoMech.set(true);
            Bot::MogoToggle = true;
            this_thread::sleep_for(400);
            Bot::Drivetrain.stop();
            isExitAiLoop = true;
            break;
          }    

          
            
        }

        
      }
    }
    */
    #pragma endregion
    ColorDetection::isEnabled = false;
    //Bot::Drivetrain.driveFor(-500, mm, true);
    //Bot::IntakeReal.stop();
    Bot::Conveyor.setVelocity(100, vex::percent);
    Bot::Conveyor.spin(forward);
    vex::this_thread::sleep_for(550);
    Bot::Conveyor.stop();
    Bot::MogoMech.set(false);
    Bot::MogoToggle = false;
    Bot::Conveyor.spinFor(reverse, 0.2, vex::seconds);


    turnForAccounting(100);
    turnCorrectToMogo();
    turnArmToPID(69.7);

    for(int i = 0; i < 1000; i++) {
      if(Bot::DistanceF.objectDistance(vex::mm) < 30) {
        Bot::Drivetrain.driveFor(100, mm, true);
        Bot::MogoMech.set(true);
        Bot::MogoToggle = true;
        Bot::Drivetrain.driveFor(100, mm, true);
        Bot::Drivetrain.stop();
        break;
      }
      Bot::Drivetrain.drive(vex::forward);
      vex::this_thread::sleep_for(1);
    }
    Bot::Drivetrain.setDriveVelocity(15, percent);
    Bot::Drivetrain.driveFor(-300, mm, true);
    Bot::Intake.spin(forward);
    this_thread::sleep_for(1000);
    turnArmToPID(131);
    turnToAccounting(79);
    Bot::Drivetrain.setDriveVelocity(15, percent);
    while(true) {
      vex::this_thread::sleep_for(1);
      Bot::Drivetrain.drive(vex::reverse);
    }

  } else {
      turnCorrectToMogo();
      for(int i = 0; i < 5000; i++) {
        if(Bot::DistanceF.objectDistance(vex::mm) < 30) {
          Bot::Drivetrain.driveFor(100, mm, true);
          Bot::MogoMech.set(true);
          Bot::MogoToggle = true;
          Bot::Drivetrain.driveFor(100, mm, true);
          Bot::Drivetrain.stop();
          while(true) {
            Bot::Intake.spin(forward);  
          }
          Bot::Intake.spin(forward);
          break;
        }
        Bot::Drivetrain.drive(vex::forward);
        vex::this_thread::sleep_for(1);
      }
  }
}

void blueRingElim() {
  Bot::Drivetrain.setDriveVelocity(35, percent);
  while(Bot::DistanceF.objectDistance(vex::mm) > BOT_SLOWDOWN_DISTANCE) {
    Bot::Drivetrain.drive(vex::forward);
  }

  Bot::Controller.Screen.setCursor(2,1);
  Bot::Controller.Screen.print("SLOWING!");

  //Slow down and drive for another 100mm
  Bot::Drivetrain.setDriveVelocity(20, percent);
  Bot::Drivetrain.drive(vex::forward);
  vex::this_thread::sleep_for(700);
  Bot::MogoToggle = true;
  Bot::MogoMech.set(true);
  vex::this_thread::sleep_for(200);
  Bot::Drivetrain.stop();
  Bot::Intake.setVelocity(100, vex::percent);
  while(true) {
    Bot::Intake.spin(forward);
  }
}

void skills() {
  
}

//Currently drives to a mogo.
void autonTest() {
  Bot::IgnoreDisplay = true;
  bool isExitAiLoop = false;
  while (true)
  {

    if(isExitAiLoop) {
      isExitAiLoop = true;
      break;
    }


    Bot::Controller.Screen.clearScreen();

    Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);

    //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
    vex::aivision::object pursuit = vex::aivision::object();

    //Bot::Drivetrain.setDriveVelocity(15, vex::percent);
    //Bot::Drivetrain.setTurnVelocity(15, vex::percent);


    if(Bot::AIVisionM.objectCount == 0) {
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

    Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
    Bot::LeftMotors.spin(vex::forward);
    Bot::RightMotors.spin(vex::forward);
    vex::this_thread::sleep_for(10);

    Bot::Controller.Screen.clearScreen();

    while (true)
    {
      //vex::this_thread::sleep_for(1000);

      //vex::this_thread::sleep_for(10);
      Bot::AIVisionM.takeSnapshot(Bot::MOGODESJ, MAX_OBJ_TO_TRACK);
    

      if(Bot::AIVisionM.objectCount == 0) {
        //Lost Ring
        Bot::Drivetrain.stop();
        break;
      }


      pursuit = Bot::AIVisionM.objects[0];
      for (size_t i = 0; i < 3; i++)
      {
        if(Bot::AIVisionM.objects[i].width > pursuit.width) pursuit = Bot::AIVisionM.objects[i];
      }
      

      bool isTurningtoDriving = false;
      //Center of screen is 160,160.

      if(Bot::LeftMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
      if(Bot::RightMotors.velocity(vex::percent) > APPROACH_VELOCITY_PERCENT_MOGO) Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);


      Bot::Controller.Screen.setCursor(3,1);
      Bot::Controller.Screen.print("LV :%.2f  RV: %.2f    ", Bot::LeftMotors.velocity(vex::percent), Bot::RightMotors.velocity(vex::percent));
      
      

      if(pursuit.originX + pursuit.width < 160) {
        Bot::LeftMotors.spin(vex::forward);
        Bot::LeftMotors.setVelocity(Bot::LeftMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else if (pursuit.originX > 160) {
        Bot::RightMotors.spin(vex::forward);
        Bot::RightMotors.setVelocity(Bot::RightMotors.velocity(vex::percent)+LINEAR_CHANGE_VELOCITY_CORRECTION, vex::percent);
        continue;
      } else {   
        Bot::LeftMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        Bot::RightMotors.setVelocity(APPROACH_VELOCITY_PERCENT_MOGO, vex::percent);
        if(isTurningtoDriving) {
          //Bot::Drivetrain.stop();
          isTurningtoDriving = false;
        } //Stop turning
        if(pursuit.width >= 200) {
          Bot::Controller.Screen.setCursor(2,1);
          Bot::Controller.Screen.clearLine(2);
          Bot::Controller.Screen.print("DONE");
          Bot::Drivetrain.setDriveVelocity(25, percent);
          Bot::Drivetrain.drive(forward);
          this_thread::sleep_for(100);
          Bot::MogoMech.set(true);
          Bot::MogoToggle = true;
          this_thread::sleep_for(300);
          Bot::Drivetrain.stop();
          isExitAiLoop = true;
          break;
        }    

        
          
      }

      
    }
  }
}


void autonomous(void) {
  //Setup AIVision Sensors.
  //Bot::AIVisionF.startAwb();

  Bot::IgnoreDisplay = true;

  //Force red auton.
  //UISystem::SelectedPosition = 2;
  Bot::Inertial.setHeading(0, degrees);

  
  Bot::IgnoreDisplay = true; 
  Bot::Controller.Screen.clearScreen();

  selectAutonBasedOfPotentiometer();
  switch (Bot::AutonomusRoutine)
  {
      case Red_Goal_Elim:
          Bot::Aliance = Red;
          redGoalElim();
          break;
      case Red_Goal_AlainceStake:
          Bot::Aliance = Red;
          redGoalAlianceStake();
          break;
      case Red_Goal_GoalRush:
          Bot::Aliance = Red;
          redGoalGoalRush();
          break;
      case Red_Ring_Elim:
          Bot::Aliance = Red;
          redRingElim();
          break;
      case Blue_Goal_Elim:
          Bot::Aliance = Blue;
          blueGoalElim();
          break;
      case Blue_Goal_AlainceStake:
          Bot::Aliance = Blue;
          blueGoalAlainceStake();
          break;
      case Blue_Goal_GoalRush:
          Bot::Aliance = Blue;
          blueGoalGoalRush();
          break;
      case Blue_Ring_Elim:
          Bot::Aliance = Blue;
          blueRingElim();
          break;
      case Test:
          Bot::Aliance = Blue;
          autonTest();
          break;
      default:
          skills();
          break;
  
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


void ToggleLadyBrown() {
  Bot::IgnoreArm = true;
  turnArmToPID(34);
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
    //Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
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

//200 ms for "double click"
void pGDP(){
  Bot::DoinkerToggle = true;
  Bot::Doinker.set(true);
}

void primeTheGripper() {
  //Release
  Bot::Gripper.set(false);

  //Wait until the Limit switch is pressed.
  Bot::Controller.rumble("..");
  waitUntil(Bot::GripperSwitch.pressing());
  Bot::Gripper.set(true);
}

void releaseGripper() {
  Bot::Gripper.set(false);
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  //Skills::runSkills(1);
  //Odometry::setupAndStartOdometry();
  Odometry::x = -1;
  Odometry::y = -1;
  Odometry::trackingWheelsInstalled=false;
  Bot::Aliance = aliance::Blue;
  selectAutonBasedOfPotentiometer();
  Bot::AutonSelect.changed(selectAutonBasedOfPotentiometer);
  //Bot::Controller.ButtonY.pressed(cycleStartingPosistions);
  //Bot::Controller.ButtonX.pressed(Bot::swapFeedPos);

  Bot::Controller.ButtonX.pressed(ToggleLadyBrown);
  Bot::Controller.ButtonY.pressed(Bot::toggleLift);
  Bot::Controller.ButtonA.pressed(Bot::toggleMogo);
  Bot::Controller.ButtonB.pressed(Bot::toggleDoinkerMacro);

  Bot::Controller.ButtonLeft.pressed(ColorDetection::toggleEnabled);
  Bot::Controller.ButtonRight.pressed(Bot::toggleDoinker);

  Bot::Controller.ButtonUp.pressed(releaseGripper);
  Bot::Controller.ButtonDown.pressed(primeTheGripper);



  vex::task drivetrian(Drivetrain::ControllerLoop);
  vex::task mainLoop(Bot::mainLoop);
  vex::task displayLoop(Bot::displayLoop);
  //vex::task blinkerLoop(Bot::blinkerLoop);
  ColorDetection::isEnabled = true;
  vex::task colorsensing(ColorDetection::visionTask);
  //vex::task monitoring(Bot::monitorLoop); //part of main loop now
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
