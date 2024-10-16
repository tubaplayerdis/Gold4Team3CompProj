#include "Bot.h"
#include "Device.h"

//Define Brain
vex::brain Bot::Brain = vex::brain();

//Define Motors
vex::motor Bot::LeftFront = vex::motor(vex::PORT1, false);
vex::motor Bot::RightFront = vex::motor(vex::PORT11, false);
vex::motor Bot::LeftRear = vex::motor(vex::PORT10, true);
vex::motor Bot::RightRear = vex::motor(vex::PORT20, true);

vex::motor Bot::MGPM = vex::motor(vex::PORT4, false);

//Define Motor Groups
vex::motor_group Bot::LeftMotors = vex::motor_group(Bot::LeftFront, Bot::LeftRear);
vex::motor_group Bot::RightMotors = vex::motor_group(Bot::RightFront, Bot::RightRear);

//Define important stuff
vex::controller Bot::Controller = vex::controller();
vex::drivetrain Bot::Drivetrain = vex::drivetrain(Bot::LeftMotors, Bot::RightMotors, 4.0, 14.25, 11.0, vex::inches, 1);

//hidden api
std::vector<Device> Bot::DeviceList = std::vector<Device>();