#include "Bot.h"

//Define Motors
vex::motor bot::LeftFront = vex::motor(vex::PORT1, false);
vex::motor bot::RightFront = vex::motor(vex::PORT11, false);
vex::motor bot::LeftRear = vex::motor(vex::PORT10, true);
vex::motor bot::RightRear = vex::motor(vex::PORT20, true);

vex::motor bot::MGPM = vex::motor(vex::PORT4, false);

//Define Motor Groups
vex::motor_group bot::LeftMotors = vex::motor_group(bot::LeftFront, bot::LeftRear);
vex::motor_group bot::RightMotors = vex::motor_group(bot::RightFront, bot::RightRear);

//Define important stuff
vex::controller bot::Controller = vex::controller();
vex::drivetrain bot::Drivetrain = vex::drivetrain(bot::LeftMotors, bot::RightMotors, 4.0, 14.25, 11.0, vex::inches, 1);