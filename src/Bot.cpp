#include "Bot.h"
#include "Device.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Notifications.h"
#include "UISystem.h"
#include "Skills.h"
#include "macros.h"

//Define Brain
vex::brain Bot::Brain = vex::brain();

autons Bot::AutonomusRoutine = Red_Goal_AlainceStake;

//Define Motors
vex::motor Bot::LeftA = vex::motor(vex::PORT1, vex::ratio6_1,true);//High Speed
vex::motor Bot::LeftB = vex::motor(vex::PORT2, vex::ratio6_1, true);//High Speed
vex::motor Bot::LeftC = vex::motor(vex::PORT3, vex::ratio6_1, true);//High Speed
vex::motor Bot::RightA = vex::motor(vex::PORT4, vex::ratio6_1, false);//High Speed
vex::motor Bot::RightB = vex::motor(vex::PORT5, vex::ratio6_1, false);//High Speed
vex::motor Bot::RightC = vex::motor(vex::PORT6, vex::ratio6_1, false);//High Speed


vex::motor Bot::Conveyor = vex::motor(vex::PORT7, vex::ratio6_1, false);//High Speed
vex::motor Bot::IntakeReal = vex::motor(vex::PORT8, vex::ratio18_1, true); //Low Power
vex::motor Bot::Arm = vex::motor(vex::PORT9, vex::ratio18_1, false); //Low Power
vex::motor_group Bot::Intake = vex::motor_group(Bot::Conveyor, Bot::IntakeReal);
bool Bot::isArmPIDActive = false;
int Bot::desiredARMAngle = LADYBROWN_DESIRED_ANGLE;


vex::digital_out Bot::MogoMech = vex::digital_out(Bot::Brain.ThreeWirePort.A);
vex::digital_out Bot::Gripper = vex::digital_out(Bot::Brain.ThreeWirePort.B);
vex::digital_out Bot::Doinker = vex::digital_out(Bot::Brain.ThreeWirePort.C);
vex::digital_out Bot::Lift = vex::digital_out(Bot::Brain.ThreeWirePort.E);
vex::pot Bot::AutonSelect = vex::pot(Bot::Brain.ThreeWirePort.D);
vex::rotation Bot::ArmPot = vex::rotation(vex::PORT12);
vex::limit Bot::GripperSwitch = vex::limit(Bot::Brain.ThreeWirePort.G);
vex::digital_out Bot::DoinkerDos = vex::digital_out(Bot::Brain.ThreeWirePort.H);
int Bot::RingsIntaken = 0;

vex::inertial Bot::Inertial = vex::inertial(vex::PORT11);
vex::rotation Bot::RotationForward = vex::rotation(vex::PORT12); //Forwards
vex::rotation Bot::RotationLateral = vex::rotation(vex::PORT13);; //Lateral
vex::optical Bot::ColorSensor = vex::optical(vex::PORT14);
vex::distance Bot::DistanceF = vex::distance(vex::PORT10);
vex::distance Bot::DistanceC = vex::distance(vex::PORT15);
aliance Bot::Aliance = aliance::Blue;

// AI Vision Color Descriptions
// AI Vision Code Descriptions
vex::aivision::colordesc Bot::BLUEDESJ = vex::aivision::colordesc(1, 42,103,172, AI_HEU_TOLERANCE, AI_SATURATION_TOLERANCE);
vex::aivision::colordesc Bot::REDDESJ = vex::aivision::colordesc(2, 209,30,105, AI_HEU_TOLERANCE, AI_SATURATION_TOLERANCE);
vex::aivision Bot::AIVisionF = vex::aivision(vex::PORT20, BLUEDESJ, REDDESJ);


vex::aivision::colordesc Bot::MOGODESJ = vex::aivision::colordesc(1, 161, 193, 129, 32, 0.21);
vex::aivision Bot::AIVisionM = vex::aivision(vex::PORT19, MOGODESJ);

//These are not installed.
vex::gps Bot::GpsF = vex::gps(vex::PORT16, -16, 16, vex::inches, 0);
vex::gps Bot::GpsL = vex::gps(vex::PORT17, -16, -15, vex::inches, -90);
vex::gps Bot::GpsR = vex::gps(vex::PORT18, 16, -15, vex::inches, 90);
vex::gps Bot::GpsB = vex::gps(vex::PORT19, 17, -17, vex::inches, 180);

bool Bot::feedGps = false;

vex::competition Bot::Comp = vex::competition();

//Led Array
/*
vex::led Bot::ModeLED = vex::led(Bot::Brain.ThreeWirePort.C);
vex::led Bot::ControllerInputLED = vex::led(Bot::Brain.ThreeWirePort.D);
vex::led Bot::NotificationLED = vex::led(Bot::Brain.ThreeWirePort.E);
vex::led Bot::WarningLED = vex::led(Bot::Brain.ThreeWirePort.F);
vex::led Bot::CriticalErrorCodeLED = vex::led(Bot::Brain.ThreeWirePort.G);
vex::led Bot::CriticalErrorLED = vex::led(Bot::Brain.ThreeWirePort.H);
*/
BlinkTypes ModeType = BlinkTypes::Solid;
BlinkTypes CriticalErrorType = BlinkTypes::Solid;

//Define Motor Groups
vex::motor_group Bot::LeftMotors = vex::motor_group(Bot::LeftA, Bot::LeftB, Bot::LeftC);
vex::motor_group Bot::RightMotors = vex::motor_group(Bot::RightA, Bot::RightB, Bot::RightC);
//vex::motor_group Bot::LiftMotors = vex::motor_group(Bot::LiftL, Bot::LiftR);

int Bot::redRingNum = 0;
int Bot::blueRingNum = 0;
int Bot::mobileGoalNum = 0;

bool Bot::IgnoreMain = false;
bool Bot::IgnoreDisplay = false;
bool Bot::IgnoreMonitor = false;
bool Bot::IgnoreVision = false;
bool Bot::IgnoreArm = false;
bool Bot::IgnoreIntake = false;

//Define important stuff
vex::controller Bot::Controller = vex::controller(vex::primary);
vex::smartdrive Bot::Drivetrain = vex::smartdrive(Bot::LeftMotors, Bot::RightMotors, Bot::Inertial,260, 393, 317, vex::mm, 0.6);

//hidden api
std::vector<Device> Bot::DeviceList = std::vector<Device>();
int Bot::NumDevices = 0;

bool Bot::ClutchToggle = false;
bool Bot::MogoToggle = true;
bool Bot::DoinkerToggle = false;
bool Bot::LiftToggle = false;

void selectAutonBasedOfPotentiometer_priv() {
    double a = Bot::AutonSelect.angle(vex::degrees);
    if(a >= 0 && a < 135) Bot::Aliance = Red; else Bot::Aliance = Blue;
    if(a >= 0 && a < 33.75) Bot::AutonomusRoutine = Red_Goal_Elim;
    else if(a >= 33.75 && a < 67.5) Bot::AutonomusRoutine = Red_Goal_AlainceStake;
    else if(a >= 67.5 && a < 101.25) Bot::AutonomusRoutine = Red_Goal_GoalRush;
    else if(a >= 101.25 && a < 135) Bot::AutonomusRoutine = Red_Ring_Elim;
    else if(a >= 135 && a < 168.75) Bot::AutonomusRoutine = Blue_Goal_Elim;
    else if(a >= 168.75 && a < 202.5) Bot::AutonomusRoutine = Blue_Goal_AlainceStake;
    else if(a >= 202.5 && a < 236.25) Bot::AutonomusRoutine = Blue_Goal_GoalRush;
    else if(a >= 236.25 && a < 240) Bot::AutonomusRoutine = Blue_Ring_Elim;
    else if(a >= 240 && a < 250) {
        Bot::AutonomusRoutine = Test;
        Bot::Aliance = Blue;
    }
    //250 is the max for some reason.
  }

void Bot::swapFeedPos() {
    feedGps = !feedGps;
}

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

void Bot::setup() {
    LeftA.setBrake(vex::brakeType::coast);
    RightA.setBrake(vex::brakeType::coast);
    LeftB.setBrake(vex::brakeType::coast);
    RightB.setBrake(vex::brakeType::coast);
    LeftC.setBrake(vex::brakeType::coast);
    RightC.setBrake(vex::brakeType::coast);
    Intake.setStopping(vex::brakeType::coast);
    Conveyor.setBrake(vex::brakeType::coast);

    Arm.setPosition(0, vex::degrees);
    Arm.setStopping(vex::hold);
    Arm.setBrake(vex::hold);

    Bot::Arm.setStopping(vex::hold);
    Bot::Arm.setVelocity(100, vex::percent);
    Bot::Arm.setMaxTorque(100, vex::percent);


    MogoMech.set(false);
    Gripper.set(false);
    Doinker.set(false);
    Lift.set(false);
    DoinkerDos.set(false);

    Bot::Inertial.calibrate();
    Bot::Controller.Screen.clearScreen();
    Bot::Controller.Screen.setCursor(1,2);
    while(Bot::Inertial.isCalibrating()) {
        Bot::Controller.Screen.print("CALIB INRERT");
        if(Bot::Controller.ButtonA.pressing()) break;
        vex::this_thread::sleep_for(30);
    }
    selectAutonBasedOfPotentiometer_priv();
    Bot::Controller.Screen.clearScreen();
    IgnoreDisplay = false;
    IgnoreMain = false;

    //int voltagelimold = vexDeviceMotorVoltageLimitGet(Device::getInternalDevicePointer(MGPM));
    //vexDeviceMotorVoltageLimitSet(Device::getInternalDevicePointer(MGPM), 15);
    //int voltagelimnew = vexDeviceMotorVoltageLimitGet(Device::getInternalDevicePointer(MGPM));
    //Brain.Screen.printAt(300, 10, "Voltage Limit Test!");
    //Brain.Screen.printAt(300, 30, to_string_int(voltagelimold).c_str());
    //Brain.Screen.printAt(330, 30, to_string_int(voltagelimnew).c_str());

}

//FOR PID
const double Kp = KP; // Proportional gain
const double Ki = KI; // Integral gain
const double Kd = KD; // Derivative gain
const double maxPower = 100; // Maximum motor power (percent)


const double tolerance = LADYBROWN_ANGLE_TOLERANCE; // Allowable error (degrees)


int Bot::mainLoop() {
    setup();
    updateDeviceList(); //If not done already.
    Bot::isArmPIDActive = false;

    double error = 0.0;       // Current error
    double previousError = 0.0; // Error from the previous step
    double integral = 0.0;    // Accumulated error
    double derivative = 0.0;  // Rate of error change
    double power = 0.0;       // Motor power
    //Brain.Screen.printAt(100,100, "Main Loop started");
    vex::competition Comp;
    while (true)
    {
        vex::this_thread::sleep_for(1);

        /*
        if(false/*Bot::isArmPIDActive) {
        
            // Get the current position of the motor
            double currentAngle = Bot::Arm.position(vex::degrees)/4;//Divided by 4 to imitate rotation sensor on the Arm.

            // Calculate error
            error = Bot::desiredARMAngle - currentAngle;

            // Break the loop if the error is within the tolerance
            if (fabs(error) <= tolerance) {
                Bot::Arm.stop(vex::brakeType::hold);
                Bot::isArmPIDActive = false;
                Bot::Controller.rumble("...");
                
            } else {
                // Calculate integral and derivative terms
                integral += error; // Accumulate error
                derivative = error - previousError;

                // Calculate PID output
                power = (Kp * error) + (Ki * integral) + (Kd * derivative);

                // Constrain the power to the motor's limits
                if (power > maxPower) power = maxPower;
                if (power < -maxPower) power = -maxPower;

                // Apply power to the motor
                Bot::Arm.spin(vex::forward, power, vex::percent);

                // Save the current error for the next loop
                previousError = error;

                // Small delay to prevent overwhelming the CPU
                vex::task::sleep(5);
            }
        }
        */

        checkMonitors();

        //Allow Auton Full Control Of Bot
        if(IgnoreMain) continue;
        if(Comp.isAutonomous()) continue;

        /*
        if(Controller.Axis1.value() != 0 || Controller.Axis2.value() || Controller.Axis3.value() != 0 || Controller.Axis4.value() || Controller.ButtonA.pressing() || Controller.ButtonB.pressing() || Controller.ButtonX.pressing() || Controller.ButtonY.pressing() || Controller.ButtonUp.pressing() || Controller.ButtonDown.pressing() || Controller.ButtonLeft.pressing() || Controller.ButtonRight.pressing() || Controller.ButtonL1.pressing() || Controller.ButtonL2.pressing() || Controller.ButtonR1.pressing() || Controller.ButtonR2.pressing()) {
            ControllerInputLED.on();
        } else {
            ControllerInputLED.off();
        }
        */

        if(!IgnoreIntake) {
            if(Controller.ButtonL2.pressing() && Controller.ButtonR2.pressing()) {
                Intake.setVelocity(0, vex::pct);
                Intake.stop();
            } else if (Controller.ButtonL2.pressing()) {
                Intake.setVelocity(100, vex::pct);
                Intake.spin(vex::reverse);
            } else if (Controller.ButtonR2.pressing()) {
                Intake.setVelocity(100, vex::pct);
                Intake.spin(vex::forward);
            } else {
                Intake.setVelocity(0, vex::pct);
                Intake.stop();
            }
        }

        if(!IgnoreArm) {
            if(Controller.ButtonL1.pressing() && Controller.ButtonR1.pressing()) {
            Arm.setVelocity(0, vex::rpm);
            Arm.stop();
            } else if (Controller.ButtonL1.pressing()) {
                Arm.setMaxTorque(100, vex::percent);
                Arm.setVelocity(200, vex::rpm);
                Arm.spin(vex::reverse);
            } else if (Controller.ButtonR1.pressing()) {
                Arm.setMaxTorque(100, vex::percent);
                Arm.setVelocity(200, vex::rpm);
                Arm.spin(vex::forward);
            } else {
                Arm.setVelocity(0, vex::rpm);
                Arm.stop();
            }
        }


        //Clear notifications
        if(Controller.ButtonLeft.pressing() && Controller.ButtonRight.pressing()) {
            Notifications::NotificationList.empty();
        }

        vex::this_thread::sleep_for(15);
        //Add some delay for computations
    }
    Brain.Screen.setPenColor("#c96638");
    Brain.Screen.printAt(100,100, "Main Loop has been aborted!");
    controllerNotification("Main Loop Aborted!");
    Brain.Screen.setPenColor(vex::color::white);
    return 1; //Return error as this should not happen.
}

void Bot::toggleClutch() {
    //ClutchToggle = !ClutchToggle;
    //Gripper.set(ClutchToggle);
}

void Bot::toggleMogo() {
    MogoToggle = !MogoToggle;
    MogoMech.set(MogoToggle);
}

void Bot::toggleDoinker() {
    DoinkerToggle = !DoinkerToggle;
    Doinker.set(DoinkerToggle);
}

bool isMacroRunning = false;

void Bot::toggleDoinkerMacro() {
    DoinkerDos.set(!DoinkerDos.value());
}

void Bot::toggleLift() {
    LiftToggle = !LiftToggle;
    Lift.set(LiftToggle);
}

void Bot::switchAlliance() {
    switch (Bot::Aliance)
    {
        case aliance::Blue:
            Bot::Aliance = aliance::Red;
            break;

        case aliance::Red:
            Bot::Aliance = aliance::Blue;
            break;

        default:
            Bot::Aliance = aliance::Blue;
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
    //Bot::Gripper.set(true);
}

void Bot::releaseClutch() {
    //Bot::Gripper.set(false);
}


void Bot::checkMonitors() {

    if(LeftA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftA TEMP");
    if(LeftB.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftB TEMP");
    if(LeftC.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftC TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightA TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightB TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightC TEMP");
    if(Intake.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Intake TEMP");
    if(Arm.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Intake TEMP");
    if(Conveyor.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Conveyor TEMP");

    if(!LeftA.installed()) Notifications::addNotification("LeftA DISCONNECT");
    if(!LeftB.installed()) Notifications::addNotification("LeftB DISCONNECT");
    if(!LeftC.installed()) Notifications::addNotification("LeftC DISCONNECT");
    if(!RightA.installed()) Notifications::addNotification("RightA DISCONNECT");
    if(!RightB.installed()) Notifications::addNotification("RightB DISCONNECT");
    if(!RightC.installed()) Notifications::addNotification("RightC DISCONNECT");
    if(!IntakeReal.installed()) Notifications::addNotification("Intake DISCONNECT");
    if(!Arm.installed()) Notifications::addNotification("ArmL DISCONNECT");
    if(!Conveyor.installed()) Notifications::addNotification("Conveyor DISCONNECT");
    //if(!RotationForward.installed()) Notifications::addNotification("RotationF DISCONNECT");
    //if(!RotationLateral.installed()) Notifications::addNotification("RotationL DISCONNECT");
    if(!Inertial.installed()) Notifications::addNotification("Inertial DISCONNECT");

    if(Brain.Battery.capacity() < 30) Notifications::addNotification("Battery 30 WARN");
    if(Brain.Battery.capacity() < 10) Notifications::addNotification("Battery 10 REPLACE");

}


int Bot::blinkerLoop() {
    /*
    while(true) {
        switch (ModeType)
        {
            case Off:
                Bot::ModeLED.off();
                break;

            case Solid:
                Bot::ModeLED.on();
                break;

            case Single:
                Bot::ModeLED.off();
                vex::this_thread::sleep_for(300);
                Bot::ModeLED.on();
                vex::this_thread::sleep_for(300);
                break;

            case Double:
                Bot::ModeLED.off();
                vex::this_thread::sleep_for(100);
                Bot::ModeLED.on();
                vex::this_thread::sleep_for(100);
                break;

            default:
                break;
        }
        switch (CriticalErrorType)
        {
            case Off:
                Bot::CriticalErrorLED.off();
                break;

            case Solid:
                Bot::CriticalErrorLED.on();
                break;

            case Single:
                Bot::CriticalErrorLED.off();
                vex::this_thread::sleep_for(300);
                Bot::CriticalErrorLED.on();
                vex::this_thread::sleep_for(300);
                break;

            case Double:
                Bot::CriticalErrorLED.off();
                vex::this_thread::sleep_for(100);
                Bot::CriticalErrorLED.on();
                vex::this_thread::sleep_for(100);
                break;

            default:
                break;
        }
        vex::this_thread::sleep_for(50);
    }
    */
    return 0;
}

int Bot::displayLoop() {
    Bot::Controller.Screen.clearScreen();
    selectAutonBasedOfPotentiometer_priv();
    while (true) {
        if(IgnoreDisplay) {
            vex::this_thread::sleep_for(50);
            continue;
        }

        if(Comp.isAutonomous()) {
            vex::this_thread::sleep_for(50);
            continue;
        }
        /*
        double a = Bot::AutonSelect.angle(vex::degrees);
        if(a >= 0 && a < 33.75) Bot::AutonomusRoutine = Red_Goal_Elim;
        else if(a >= 33.75 && a < 67.5) Bot::AutonomusRoutine = Red_Goal_AlainceStake;
        else if(a >= 67.5 && a < 101.25) Bot::AutonomusRoutine = Red_Goal_GoalRush;
        else if(a >= 101.25 && a < 135) Bot::AutonomusRoutine = Red_Ring_Elim;
        else if(a >= 135 && a < 168.75) Bot::AutonomusRoutine = Blue_Goal_Elim;
        else if(a >= 168.75 && a < 202.5) Bot::AutonomusRoutine = Blue_Goal_AlainceStake;
        else if(a >= 202.5 && a < 236.25) Bot::AutonomusRoutine = Blue_Goal_GoalRush;
        else if(a >= 236.25 && a < 270) Bot::AutonomusRoutine = Blue_Ring_Elim;
        */
        /*
            LED
        */

        /*
            CONTROLLER
        */
        Bot::Controller.Screen.setCursor(1,1);
        if(Bot::feedGps, Skills::isSkillsActive()) {
            Bot::Controller.Screen.print("X:%.1f,Y:%.1f,H:%.1f", Skills::x, Skills::y, Skills::h);
        } else {
            Bot::Controller.Screen.print("H:%.1f, DIS:%.1f", Bot::Inertial.heading(), /*DistanceF.objectDistance(vex::mm)*/Bot::ArmPot.position(vex::deg));
        }
        Bot::Controller.Screen.setCursor(3,1);
        if(Skills::isSkillsActive()) {
            Bot::Controller.Screen.print("%02d %s", SkillsEngine::currentTaskIndex(), SkillsEngine::currentTask().name.c_str());
        } else {
            switch (AutonomusRoutine)
            {
                case Red_Goal_Elim:
                    Bot::Controller.Screen.print("RED GOAL E       CS%d  ", ColorDetection::isEnabled);
                    break;
                case Red_Goal_AlainceStake:
                    Bot::Controller.Screen.print("RED GOAL AS      CS%d  ", ColorDetection::isEnabled);
                    break;
                case Red_Goal_GoalRush:
                    Bot::Controller.Screen.print("RED GOAL GR     CS%d  ", ColorDetection::isEnabled);
                    break;
                case Red_Ring_Elim:
                    Bot::Controller.Screen.print("RED RIGHT E      CS%d  ", ColorDetection::isEnabled);
                    break;
                case Blue_Goal_Elim:
                    Bot::Controller.Screen.print("BLUE GOAL E      CS%d  ", ColorDetection::isEnabled);
                    break;
                case Blue_Goal_AlainceStake:
                    Bot::Controller.Screen.print("BLUE GOAL AS     CS%d  ", ColorDetection::isEnabled);
                    break;
                case Blue_Goal_GoalRush:
                    Bot::Controller.Screen.print("BLUE GOAL GR    CS%d  ", ColorDetection::isEnabled);
                    break;
                case Blue_Ring_Elim:
                    Bot::Controller.Screen.print("BLUE RING E      CS%d  ", ColorDetection::isEnabled);
                    break;
                case Test:
                    Bot::Controller.Screen.print("TESTING          CS%d  ", ColorDetection::isEnabled);
                    break;
                default:
                    Bot::Controller.Screen.print("ERROR            CS%d  ", ColorDetection::isEnabled);
                    break;
            
            }
        }

        if(Notifications::NotificationList.empty()) continue;
        if(Notifications::notifIndex > Notifications::NotificationList.size()-1 || Notifications::notifIndex < 0) {
            Notifications::notifIndex = 0;
            continue;
        }
        
        if(!Notifications::isNotifying && !Comp.isAutonomous()) {
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

void Bot::checkInstall() {
    if(IgnoreMonitor) return;

    if(LeftA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftA TEMP");
    if(LeftB.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftB TEMP");
    if(LeftC.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftC TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightA TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightB TEMP");
    if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightC TEMP");
    if(Intake.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Intake TEMP");
    //if(ArmL.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("ArmL TEMP");
    //if(ArmR.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("ArmR TEMP");

    if(!LeftA.installed()) Notifications::addNotification("LeftA DISCONNECT");
    if(!LeftB.installed()) Notifications::addNotification("LeftB DISCONNECT");
    if(!LeftC.installed()) Notifications::addNotification("LeftC DISCONNECT");
    if(!RightA.installed()) Notifications::addNotification("RightA DISCONNECT");
    if(!RightB.installed()) Notifications::addNotification("RightB DISCONNECT");
    if(!RightC.installed()) Notifications::addNotification("RightC DISCONNECT");
    //if(!Intake.installed()) Notifications::addNotification("Intake DISCONNECT");
    //if(!ArmL.installed()) Notifications::addNotification("ArmL DISCONNECT");
    //if(!ArmR.installed()) Notifications::addNotification("ArmR DISCONNECT");
    //if(!RotationForward.installed()) Notifications::addNotification("RotationF DISCONNECT");
    //if(!RotationLateral.installed()) Notifications::addNotification("RotationL DISCONNECT");
    if(!Inertial.installed()) Notifications::addNotification("Inertial DISCONNECT");

    if(Brain.Battery.capacity() < 30) Notifications::addNotification("Battery 30 WARN");
    if(Brain.Battery.capacity() < 10) Notifications::addNotification("Battery 10 REPLACE");
}

//Dont use this. Used by main loop
int Bot::monitorLoop() {
    while (true) {

        if(IgnoreMonitor) continue;

        if(LeftA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftA TEMP");
        if(LeftB.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftB TEMP");
        if(LeftC.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("LeftC TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightA TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightB TEMP");
        if(RightA.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("RightC TEMP");
        if(Intake.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("Intake TEMP");
        //if(ArmL.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("ArmL TEMP");
        //if(ArmR.temperature(vex::temperatureUnits::fahrenheit) > 129) Notifications::addNotification("ArmR TEMP");

        if(!LeftA.installed()) Notifications::addNotification("LeftA DISCONNECT");
        if(!LeftB.installed()) Notifications::addNotification("LeftB DISCONNECT");
        if(!LeftC.installed()) Notifications::addNotification("LeftC DISCONNECT");
        if(!RightA.installed()) Notifications::addNotification("RightA DISCONNECT");
        if(!RightB.installed()) Notifications::addNotification("RightB DISCONNECT");
        if(!RightC.installed()) Notifications::addNotification("RightC DISCONNECT");
        //if(!Intake.installed()) Notifications::addNotification("Intake DISCONNECT");
        //if(!ArmL.installed()) Notifications::addNotification("ArmL DISCONNECT");
        //if(!ArmR.installed()) Notifications::addNotification("ArmR DISCONNECT");
        //if(!RotationForward.installed()) Notifications::addNotification("RotationF DISCONNECT");
        //if(!RotationLateral.installed()) Notifications::addNotification("RotationL DISCONNECT");
        if(!Inertial.installed()) Notifications::addNotification("Inertial DISCONNECT");

        if(Brain.Battery.capacity() < 30) Notifications::addNotification("Battery 30 WARN");
        if(Brain.Battery.capacity() < 10) Notifications::addNotification("Battery 10 REPLACE");


        vex::this_thread::sleep_for(1000);
    }
    return 0;
}

//Dont use this either.
int Bot::aiLoop() {
    while(true) {

        if(IgnoreVision) continue;

        AIVisionF.takeSnapshot(vex::aivision::ALL_AIOBJS);
        //Brain.Screen.printAt(0,50, "AI Vision Count: %d", AIVisionF.objectCount);
        int tempred = 0, tempblue = 0, tempmob = 0;
        for(int i = 0; i < AIVisionF.objectCount; i++) {
            int id = Bot::AIVisionF.objects[i].id;
            switch (id)
            {
            case 0:
                tempred++;
                break;
            case 1:
                tempblue++;
                break;
            case 2:
                tempmob++;
                break;
            }
            //Brain.Screen.printAt(0, 70+(20*i), str, AIVisionF.objects[i].centerX, AIVisionF.objects[i].centerY);
            //ID: %s, 
            //AIVisionF.objects[i].name
        }
        Bot::redRingNum = tempred;
        Bot::blueRingNum = tempred;
        Bot::mobileGoalNum = tempred;

        vex::this_thread::sleep_for(50);
    }
    return 0;
}