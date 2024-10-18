#include "Bot.h"
#include "Device.h"

//Define Brain
vex::brain Bot::Brain = vex::brain();

//Define Motors
vex::motor Bot::LeftFront = vex::motor(vex::PORT1, vex::ratio18_1,false);
vex::motor Bot::LeftRear = vex::motor(vex::PORT11, vex::ratio18_1, false);
vex::motor Bot::RightFront = vex::motor(vex::PORT10, vex::ratio18_1, true);
vex::motor Bot::RightRear = vex::motor(vex::PORT20, vex::ratio18_1, true);

vex::motor Bot::MGPM = vex::motor(vex::PORT4, vex::ratio18_1, false);

//Define Motor Groups
vex::motor_group Bot::LeftMotors = vex::motor_group(Bot::LeftFront, Bot::LeftRear);
vex::motor_group Bot::RightMotors = vex::motor_group(Bot::RightFront, Bot::RightRear);

//Define important stuff
vex::controller Bot::Controller = vex::controller(vex::primary);
vex::drivetrain Bot::Drivetrain = vex::drivetrain(Bot::LeftMotors, Bot::RightMotors, 4.0, 14.25, 11.0, vex::inches, 1);

//hidden api
std::vector<Device> Bot::DeviceList = std::vector<Device>();
int Bot::NumDevices = 0;

void Bot::updateDeviceList() {
    V5_DeviceTypeBuffer devicel;
    Bot::NumDevices = vexDeviceGetStatus(devicel);
    for(int i = 0; i < vexDevicesGetNumber(); i++) {
        std::string devicename = Device::getDeviceTypeString(devicel[i]);
        if(devicename != "kDeviceTypeNoSensor") {
            DeviceList.push_back(Device(devicename, i, vexDeviceGetByIndex(i)));
        } 
    }

}

void Bot::setup() {
    LeftFront.setBrake(vex::hold);
    LeftRear.setBrake(vex::hold);
    RightFront.setBrake(vex::hold);
    RightRear.setBrake(vex::hold);
    MGPM.setBrake(vex::hold);
}

int Bot::mainLoop() {
    setup();

    updateDeviceList(); //If not done already.
    Brain.Screen.printAt(100,100, "Main Loop started");
    while (true)
    {
        //Abort Loop
        if(Controller.ButtonDown.pressing()) break;

        //MGPM impl
        if(Controller.ButtonA.pressing() && Controller.ButtonB.pressing()) {
            MGPM.setVelocity(0, vex::rpm);
            MGPM.stop();
        } else if (Controller.ButtonA.pressing()) {
            MGPM.setVelocity(200, vex::rpm);
            MGPM.spin(vex::directionType::fwd);
        } else if(Controller.ButtonB.pressing()) {
            MGPM.setVelocity(200, vex::rpm);
            MGPM.spin(vex::directionType::rev);
        } else {
            MGPM.setVelocity(0, vex::rpm);
            MGPM.stop();
        }
        
        vex::wait(20, vex::msec);
        //Add some delay for computations
    }
    Brain.Screen.setPenColor("#c96638");
    Brain.Screen.printAt(100,100, "Main Loop has been aborted!");
    Brain.Screen.setPenColor(vex::color::white);
    return 1; //Return error as this should not happen.
}