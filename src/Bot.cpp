#include "Bot.h"
#include "Device.h"

//Define Brain
vex::brain Bot::Brain = vex::brain();

//Define Motors
vex::motor Bot::LeftFront = vex::motor(vex::PORT1, vex::ratio18_1,false);//Regular
vex::motor Bot::LeftRear = vex::motor(vex::PORT11, vex::ratio18_1, false);//Regular
vex::motor Bot::RightFront = vex::motor(vex::PORT10, vex::ratio18_1, true);//Regular
vex::motor Bot::RightRear = vex::motor(vex::PORT20, vex::ratio18_1, true);//Regular

vex::motor Bot::MGPM = vex::motor(vex::PORT4, vex::ratio6_1, false);//High Speed
vex::motor Bot::Intake = vex::motor(vex::PORT5, vex::ratio18_1, false);//Low Power

vex::motor Bot::ConveyorTop = vex::motor(vex::PORT6, vex::ratio6_1, true);//High Speed
vex::motor Bot::ConveyorBottom = vex::motor(vex::PORT7, vex::ratio6_1, true);//High Speed
vex::motor_group Bot::ConveyorMotors = vex::motor_group(Bot::ConveyorTop, Bot::ConveyorBottom);//Dual High Speed
bool Bot::autoConveyor = false;

//Define Motor Groups
vex::motor_group Bot::LeftMotors = vex::motor_group(Bot::LeftFront, Bot::LeftRear);
vex::motor_group Bot::RightMotors = vex::motor_group(Bot::RightFront, Bot::RightRear);

//Define important stuff
vex::controller Bot::Controller = vex::controller(vex::primary);
vex::drivetrain Bot::Drivetrain = vex::drivetrain(Bot::LeftMotors, Bot::RightMotors, 4.0, 14.25, 11.0, vex::inches, 1);

//hidden api
std::vector<Device> Bot::DeviceList = std::vector<Device>();
int Bot::NumDevices = 0;

void Bot::controllerNotification(std::string notif) {
    Controller.Screen.setCursor(1,1);
    Controller.Screen.print(notif.c_str());
}

//Helper method
std::string to_string_int(int x){
  std::stringstream s;
  s << x;
  return s.str();
}

std::string to_string_bool(bool x) {
    if(x) return "1";
    else return "0";
}

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

    //int voltagelimold = vexDeviceMotorVoltageLimitGet(Device::getInternalDevicePointer(MGPM));
    //vexDeviceMotorVoltageLimitSet(Device::getInternalDevicePointer(MGPM), 15);
    //int voltagelimnew = vexDeviceMotorVoltageLimitGet(Device::getInternalDevicePointer(MGPM));
    //Brain.Screen.printAt(300, 10, "Voltage Limit Test!");
    //Brain.Screen.printAt(300, 30, to_string_int(voltagelimold).c_str());
    //Brain.Screen.printAt(330, 30, to_string_int(voltagelimnew).c_str());

}

int Bot::mainLoop() {
    setup();
    updateDeviceList(); //If not done already.
    //Brain.Screen.printAt(100,100, "Main Loop started");
    while (true)
    {
        //Abort Loop
        if(Controller.ButtonDown.pressing()) break;

        //MGPM impl
        if(Controller.ButtonA.pressing() && Controller.ButtonB.pressing()) {
            MGPM.setVelocity(0, vex::rpm);
            MGPM.stop();
        } else if (Controller.ButtonA.pressing()) {
            MGPM.setVelocity(400, vex::rpm);
            MGPM.spin(vex::directionType::fwd);
            //Controller Stuff
        Controller.Screen.setCursor(3,1);
        Controller.Screen.print("MGPM UP");
        } else if(Controller.ButtonB.pressing()) {
            MGPM.setVelocity(400, vex::rpm);
            MGPM.spin(vex::directionType::rev);
            //Controller Stuff
        Controller.Screen.setCursor(3,1);
        Controller.Screen.print("MGMP DOWN");
        } else {
            MGPM.setVelocity(0, vex::rpm);
            MGPM.stop();
        }

        if(Controller.ButtonL2.pressing() && Controller.ButtonR2.pressing()) {
            ConveyorMotors.setVelocity(0, vex::rpm);
            ConveyorMotors.stop();
            Intake.setVelocity(0, vex::rpm);
            Intake.stop();
        } else if (Controller.ButtonL2.pressing()) {
            ConveyorMotors.setVelocity(100, vex::rpm);
            ConveyorMotors.spin(vex::forward);
            Intake.setVelocity(200, vex::rpm);
            Intake.spin(vex::forward);
        } else if (Controller.ButtonR2.pressing()) {
            ConveyorMotors.setVelocity(100, vex::rpm);
            ConveyorMotors.spin(vex::reverse);
            Intake.setVelocity(200, vex::rpm);
            Intake.spin(vex::forward);
        } else {
            ConveyorMotors.setVelocity(0, vex::rpm);
            ConveyorMotors.stop();
            Intake.setVelocity(0, vex::rpm);
            Intake.stop();
        }

        
        vex::wait(20, vex::msec);
        //Add some delay for computations
    }
    Brain.Screen.setPenColor("#c96638");
    Brain.Screen.printAt(100,100, "Main Loop has been aborted!");
    controllerNotification("Main Loop Aborted!");
    Brain.Screen.setPenColor(vex::color::white);
    return 1; //Return error as this should not happen.
}