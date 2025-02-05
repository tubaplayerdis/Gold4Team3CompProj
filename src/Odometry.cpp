#include "Odometry.h"
#include "vex.h"
#include <cmath>
#include <algorithm>
#include "Notifications.h"

// Odometry Variables
double Odometry::x = 0.0;   // X position in inches (forward/backward)
double Odometry::y = 0.0;   // Y position in inches (left/rsight)
double Odometry::heading = 0.0; // spin in degress?
bool Odometry::trackingWheelsInstalled = true;


gameElementPosition Odometry::startMobilePP = { 24, 24, vexui::gameElements::mobileGoal};
gameElementPosition Odometry::startMobileNP = { -24, 24, vexui::gameElements::mobileGoal};
gameElementPosition Odometry::startMobileNN = { -24, -24, vexui::gameElements::mobileGoal};
gameElementPosition Odometry::startMobilePN = { 24, -24, vexui::gameElements::mobileGoal};

vex::controller::button Odometry::MotorRunKey = Bot::Controller.ButtonX;

// Calculate distance between robot and a wall stake
double Odometry::calculateDistanceToStake(double stakeX, double stakeY) {
    return sqrt(pow(stakeX - Odometry::x, 2) + pow(stakeY - Odometry::y, 2));
}

void Odometry::setHeading(double heading) {
    Bot::Inertial.setHeading(heading, vex::degrees);
}

#define MIN(x,y) ((x) < (y) ? (x) : (y)) 

// Drive to a specific position
void Odometry::driveToPosition(double targetX, double targetY, bool override) {
    double distance_to_target;
    double angle_to_target;
    bool dostop = false;
    while (true) {
        if(!Odometry::MotorRunKey.pressing() && !override) {
            if(dostop) {
                Bot::LeftMotors.stop();
                Bot::RightMotors.stop();
                dostop = false;
            }
            continue;
        }
        distance_to_target = calculateDistanceToStake(targetX, targetY);
        angle_to_target = atan2(targetY - Odometry::y, targetX - Odometry::x) * (180 / M_PI) - Odometry::heading;

        //Bot::Brain.Screen.printAt(0, 50, "Distance to Target: %f", distance_to_target);

        if (distance_to_target < 1) { // Tolerance in mm (1 inch)
            Bot::LeftMotors.stop();
            Bot::RightMotors.stop();
            break;
        }

        double forwardSpeed = MIN(distance_to_target * 0.2, 50); // Scale speed, limit to 50
        double turnSpeed = angle_to_target * 0.5;

        Bot::LeftMotors.spin(vex::forward, forwardSpeed + turnSpeed, vex::pct);
        Bot::RightMotors.spin(vex::forward, forwardSpeed - turnSpeed, vex::pct);

        dostop = true;
        vex::this_thread::sleep_for(20);
    }
}

// Rotate to specified angle using inertial sensor
void Odometry::rotateToAngle(double targetAngle, bool override) {
    bool dostop = false;
    while (fabs(Odometry::heading - targetAngle) > 1.0) {
        if(!Odometry::MotorRunKey.pressing() && !override) {
            if(dostop) {
                Bot::LeftMotors.stop();
                Bot::RightMotors.stop();
                dostop = false;
            }
            continue;
        }
        double error = targetAngle - Odometry::heading;
        double turnSpeed = error * 0.5;

        Bot::LeftMotors.spin(vex::forward, turnSpeed, vex::pct);
        Bot::RightMotors.spin(vex::reverse, turnSpeed, vex::pct);

        dostop = true;
        vex::this_thread::sleep_for(20);
    }
    Bot::LeftMotors.stop();
    Bot::RightMotors.stop();
}

// Drive to the nearest wall stake and align perpendicular
int Odometry::driveToNearestWallStake(bool override) {
    while (true)
    {
        double distanceToStake1 = Odometry::calculateDistanceToStake(Odometry::startMobileNN.x, Odometry::startMobileNN.y);
        double distanceToStake2 = Odometry::calculateDistanceToStake(Odometry::startMobileNP.x, Odometry::startMobileNP.y);
        double distanceToStake3 = Odometry::calculateDistanceToStake(Odometry::startMobilePP.x, Odometry::startMobilePP.y);
        double distanceToStake4 = Odometry::calculateDistanceToStake(Odometry::startMobilePN.x, Odometry::startMobilePN.y);

        //Bot::Brain.Screen.printAt(0, 70, "Calculated Distance Of Stakes");

        double target = std::min<double>( { distanceToStake1, distanceToStake2, distanceToStake3, distanceToStake4 } );

        double targetX, targetY, perpendicularAngle;

        if (target == distanceToStake1) {
            targetX = Odometry::startMobileNN.x;
            targetY = Odometry::startMobileNN.y;
            perpendicularAngle = heading;  // Align perpendicular to stake at (-1750, 0)
        } else if (target == distanceToStake2) {
            targetX = Odometry::startMobileNN.x;
            targetY = Odometry::startMobileNN.y;
            perpendicularAngle = heading; // Align perpendicular to stake at (1750, 0)
        } else if (target == distanceToStake3) {
            targetX = Odometry::startMobileNN.x;
            targetY = Odometry::startMobileNN.y;
            perpendicularAngle = heading; // Align perpendicular to stake at (1750, 0)
        } else if (target == distanceToStake4) {
            targetX = Odometry::startMobileNN.x;
            targetY = Odometry::startMobileNN.y;
            perpendicularAngle = heading; // Align perpendicular to stake at (1750, 0)
        }

        // Drive to the target position
        driveToPosition(targetX, targetY, override);

        // Rotate to face perpendicular
        rotateToAngle(perpendicularAngle, override);   
    }
    return 0;
}

int Odometry::wrapperwallstake() {
    driveToNearestWallStake(false);
    return 1;
}


// Function to Update Odometry Using Encoders and Bot:: Sensor
void Odometry::updateOdometry() {
    Odometry::heading = Bot::Inertial.heading(vex::degrees);

    if(!trackingWheelsInstalled) return;

    double forward_distance = (Bot::RotationForward.position(vex::degrees) / 360) * Odometry::tracking_wheel_circumference; // Forward movement
    //Bot::Brain.Screen.printAt(100,100,"Forward Dis %f", forward_distance);
    double lateral_distance = (Bot::RotationLateral.position(vex::degrees) / 360) * Odometry::tracking_wheel_circumference; // Lateral movement

    Bot::RotationForward.resetPosition();
    Bot::RotationLateral.resetPosition();

    double deltaX = forward_distance * cos(Odometry::heading * M_PI / 180);
    double deltaY = lateral_distance * cos(Odometry::heading * M_PI / 180);

    x += deltaX;
    y += deltaY;

    if(Bot::Comp.isAutonomous() && (x < 10 || x > -10)) {
        Bot::Drivetrain.setDriveVelocity(0, vex::percent);
        Bot::Drivetrain.setTurnVelocity(0, vex::percent);
        Bot::Drivetrain.stop();
        Notifications::addNotification("AUTON OVERRIDE OUT");
    }
}

int Odometry::odometry() {
    while (true) {
        Odometry::updateOdometry();
        vex::this_thread::sleep_for(10);  // Update rate in milliseconds
    }
    return 0;
}


//DO NOT USE THIS FUNCTION
int displayCoordinates() {
    while (false) {
        //Bot::Controller.Screen.setCursor(1,1);
        //Bot::Controller.Screen.setCursor(1,1);
        //Bot::Controller.Screen.print("X:%.2f, Y:%.2f", Odometry::x, Odometry::y);

        //Bot::Brain.Screen.clearScreen(); // Clear the screen for updated information
        //Bot::Brain.Screen.setCursor(1, 1);
        //Bot::Brain.Screen.print("X = %.2f, Y = %.2f", Odometry::x, Odometry::y); // Print current coordinates
        //vex::this_thread::sleep_for(20); // Update rate for display (in milliseconds)
    }
    return 0;
}

// "when started" hat block
int Odometry::setupAndStartOdometry() {
    if(!Bot::RotationForward.installed() || !Bot::RotationLateral.installed()) {
        Notifications::addNotification("ODO DISABLE DISC");
        Odometry::x = -1;
        Odometry::y = -1;
        trackingWheelsInstalled=false;
    };
    vex::task updateLoop(odometry);
    //This dosent work and its kinda useless.
    //vex::task driveLoop(wrapperwallstake);
    return 0;
}
