#include "Bot.h"
#include "Device.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Notifications.h"

//Define Brain
vex::brain Bot::Brain = vex::brain();

//Define Motors
vex::motor Bot::LeftA = vex::motor(vex::PORT1, vex::ratio6_1,true);//High Speed
vex::motor Bot::LeftB = vex::motor(vex::PORT2, vex::ratio6_1, true);//High Speed
vex::motor Bot::LeftC = vex::motor(vex::PORT3, vex::ratio6_1, false);//High Speed
vex::motor Bot::RightA = vex::motor(vex::PORT4, vex::ratio6_1, false);//High Speed
vex::motor Bot::RightB = vex::motor(vex::PORT5, vex::ratio6_1, false);//High Speed
vex::motor Bot::RightC = vex::motor(vex::PORT6, vex::ratio6_1, true);//High Speed


vex::motor Bot::Intake = vex::motor(vex::PORT7, vex::ratio18_1, true);//Low Power
vex::motor Bot::Arm = vex::motor(vex::PORT8, vex::ratio36_1, false);//High Torque
//vex::motor Bot::LiftL = vex::motor(vex::PORT9, vex::ratio18_1, true);//Low Power
//vex::motor Bot::LiftR = vex::motor(vex::PORT10, vex::ratio18_1, true);//Low Power
vex::digital_out Bot::MogoMech = vex::digital_out(Bot::Brain.ThreeWirePort.A);
vex::digital_out Bot::Clutch = vex::digital_out(Bot::Brain.ThreeWirePort.B);

vex::inertial Bot::Inertial = vex::inertial(vex::PORT11);
vex::rotation Bot::RotationForward = vex::rotation(vex::PORT12); //Forwards
vex::rotation Bot::RotationLateral = vex::rotation(vex::PORT13);; //Lateral
vex::optical Bot::ColorSensor = vex::optical(vex::PORT14);;
aliance Bot::Aliance = aliance::Nuetral;

// AI Vision Color Descriptions
// AI Vision Code Descriptions
vex::aivision Bot::AIVisionF(vex::PORT20, vex::aivision::ALL_TAGS, vex::aivision::ALL_AIOBJS);

//Define Motor Groups
vex::motor_group Bot::LeftMotors = vex::motor_group(Bot::LeftA, Bot::LeftB, Bot::LeftC);
vex::motor_group Bot::RightMotors = vex::motor_group(Bot::RightA, Bot::RightB, Bot::RightC);
//vex::motor_group Bot::LiftMotors = vex::motor_group(Bot::LiftL, Bot::LiftR);

//Define important stuff
vex::controller Bot::Controller = vex::controller(vex::primary);
vex::drivetrain Bot::Drivetrain = vex::drivetrain(Bot::LeftMotors, Bot::RightMotors, 4.0, 14.25, 11.0, vex::inches, 1);

//hidden api
std::vector<Device> Bot::DeviceList = std::vector<Device>();
int Bot::NumDevices = 0;

//Realisticly dont use this. I dont like it.
void Bot::controllerNotification(std::string notif) {
    Notifications::addNotification(notif);
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

const char* Bot::getGameElementNameFromID(int32_t id) {
    switch (id)
    { 
    case 0:
        return "0";
    case 1:
        return "1";
    case 2:
        return "2";
    
    default:
        return "-1";
    }
}

void Bot::setup() {
    LeftA.setBrake(vex::hold);
    RightA.setBrake(vex::hold);
    LeftB.setBrake(vex::hold);
    RightB.setBrake(vex::hold);
    LeftC.setBrake(vex::hold);
    RightC.setBrake(vex::hold);

    MogoMech.set(false);
    Clutch.set(true);

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
        //if(Controller.ButtonDown.pressing()) break;

        if(Controller.ButtonL2.pressing() && Controller.ButtonR2.pressing()) {
            Intake.setVelocity(0, vex::rpm);
            Intake.stop();
        } else if (Controller.ButtonL2.pressing()) {
            Intake.setVelocity(200, vex::rpm);
            Intake.spin(vex::forward);
        } else if (Controller.ButtonR2.pressing()) {
            Intake.setVelocity(200, vex::rpm);
            Intake.spin(vex::reverse);
        } else {
            Intake.setVelocity(0, vex::rpm);
            Intake.stop();
        }

        if(Controller.ButtonUp.pressing()) {
            Arm.setVelocity(30, vex::rpm);
        } else if (Controller.ButtonDown.pressing()) {
            Arm.setVelocity(30, vex::rpm);
            Arm.spin(vex::forward);
        } else {
            Arm.stop();
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

void Bot::switchAlliance() {
    switch (Bot::Aliance)
    {
        case aliance::Nuetral:
            Bot::Aliance = aliance::Blue;
            break;

        case aliance::Blue:
            Bot::Aliance = aliance::Red;
            break;

        case aliance::Red:
            Bot::Aliance = aliance::Nuetral;
            break;

        default:
            Bot::Aliance = aliance::Nuetral;
            break;
    }
}

void Bot::clampMobileGoal() {
    Bot::MogoMech.set(true);
}

void Bot::releaseMobileGoal() {
    Bot::MogoMech.set(false);
}

void Bot::pushClutch() {
    Bot::Clutch.set(true);
}

void Bot::releaseClutch() {
    Bot::Clutch.set(false);
}
//use this eventually? no!
void Bot::checkInstall() {
    Bot::Brain.Screen.printAt(0, 30, "Arm: %d", Bot::Arm.installed());
    Bot::Brain.Screen.printAt(0, 50, "Intake: %d", Bot::Intake.installed());
    Bot::Brain.Screen.printAt(0, 70, "LeftA: %d", Bot::LeftA.installed());
    Bot::Brain.Screen.printAt(0, 90, "LeftB: %d", Bot::LeftB.installed());
    Bot::Brain.Screen.printAt(0, 110, "LeftC: %d", Bot::LeftC.installed());
    Bot::Brain.Screen.printAt(0, 130, "RightA: %d", Bot::RightA.installed());
    Bot::Brain.Screen.printAt(0, 150, "RightB: %d", Bot::RightB.installed());
    Bot::Brain.Screen.printAt(0, 170, "RightC: %d", Bot::RightC.installed());
    Bot::Brain.Screen.printAt(0, 190, "Interial: %d", Bot::Inertial.installed());
    Bot::Brain.Screen.printAt(0, 210, "RotationForward: %d", Bot::RotationForward.installed());
    Bot::Brain.Screen.printAt(0, 230, "RotationLateral: %d", Bot::RotationLateral.installed());
    Bot::Brain.Screen.printAt(0, 250, "ColorSensor: %d", Bot::ColorSensor.installed());
}

int Bot::displayLoop() {
    Bot::Controller.Screen.clearScreen();
    while (true) {
        /*
            CONTROLLER
        */
        Bot::Controller.Screen.setCursor(1,1);
        Bot::Controller.Screen.print("X:%.2f, Y:%.2f", Odometry::x, Odometry::y);
        Bot::Controller.Screen.setCursor(3,1);
        switch (Bot::Aliance)
        {
            case aliance::Nuetral:
                Bot::Controller.Screen.print("NO ALIANCE   CS:%01d", ColorDetection::isEnabled);
                break;
            case aliance::Blue:
                Bot::Controller.Screen.print("BLUE ALIANCE CS:%01d", ColorDetection::isEnabled);
                break;
            case aliance::Red:
                Bot::Controller.Screen.print("RED ALIANCE  CS:%01d", ColorDetection::isEnabled);
                break;
        
        }

        if(Notifications::NotificationList.empty()) continue;
        if(Notifications::notifIndex > Notifications::NotificationList.size()-1 || Notifications::notifIndex < 0) {
            Notifications::notifIndex = 0;
            continue;
        }
        
        if(!Notifications::isNotifying) {
            Bot::Controller.Screen.setCursor(2,1);
            Bot::Controller.Screen.clearLine(2);
            Bot::Controller.Screen.print("%02d:", Notifications::notifIndex);
            Bot::Controller.Screen.setCursor(2,4);
            Bot::Controller.Screen.print(Notifications::NotificationList.at(Notifications::notifIndex).c_str());
        }


        /*
            BRAIN
        */
       /*
        Bot::Brain.Screen.clearScreen(); // Clear the screen for updated information
        Bot::Brain.Screen.setCursor(1, 1);
        Bot::Brain.Screen.print("X = %.2f, Y = %.2f", Odometry::x, Odometry::y); // Print current coordinates
        */

        /*
            REFRESH RATE
        */
        vex::this_thread::sleep_for(20); // Update rate for display (in milliseconds)
    }
    return 0;
}

int Bot::monitorLoop() {
    while (true) {
        if(LeftA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftA TEMP");
        if(LeftB.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftB TEMP");
        if(LeftC.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftC TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightA TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightB TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightC TEMP");
        if(Intake.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Intake TEMP");
        if(Arm.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Arm TEMP");

        if(!LeftA.installed()) Notifications::addNotification("LeftA DISCONNECT");
        if(!LeftB.installed()) Notifications::addNotification("LeftB DISCONNECT");
        if(!LeftC.installed()) Notifications::addNotification("LeftC DISCONNECT");
        if(!RightA.installed()) Notifications::addNotification("RightA DISCONNECT");
        if(!RightB.installed()) Notifications::addNotification("RightB DISCONNECT");
        if(!RightC.installed()) Notifications::addNotification("RightC DISCONNECT");
        if(!Intake.installed()) Notifications::addNotification("Intake DISCONNECT");
        if(!Arm.installed()) Notifications::addNotification("Arm DISCONNECT");
        if(!RotationForward.installed()) Notifications::addNotification("RotationF DISCONNECT");
        if(!RotationLateral.installed()) Notifications::addNotification("RotationL DISCONNECT");


        vex::this_thread::sleep_for(1000);
    }
    return 0;
}


int Bot::aiLoop() {
    while(true) {
        AIVisionF.takeSnapshot(vex::aivision::ALL_AIOBJS);
        //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
        for(int i = 0; i < AIVisionF.objectCount; i++) {
            const char* str = strcat(strcat("Name: ", getGameElementNameFromID(Bot::AIVisionF.objects[i].id)),", X:%d, Y:%d");
            //Brain.Screen.printAt(0, 70+(20*i), str, AIVisionF.objects[i].centerX, AIVisionF.objects[i].centerY);
            //ID: %s, 
            //AIVisionF.objects[i].name
        }

        vex::this_thread::sleep_for(50);
    }
    return 0;
}