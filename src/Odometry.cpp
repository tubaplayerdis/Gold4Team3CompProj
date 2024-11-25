#include "Odometry.h"
#include "vex.h"
#include <cmath>
#include <algorithm>

// Odometry Variables
double Odometry::x = 0.0;   // X position in inches (forward/backward)
double Odometry::y = 0.0;   // Y position in inches (left/rsight)
double Odometry::heading = 0.0; // spin in degress?

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
void Odometry::driveToPosition(double targetX, double targetY) {
    double distance_to_target;
    double angle_to_target;
    bool dostop = false;
    while (true) {
        if(!Odometry::MotorRunKey.pressing()) {
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

        if (distance_to_target < 25.4) { // Tolerance in mm (1 inch)
            Bot::LeftMotors.stop();
            Bot::RightMotors.stop();
            Bot::Brain.Screen.printAt(130, 130, "Drove To WallStake!");
            break;
        }

        double forwardSpeed = MIN(distance_to_target * 0.2, 50); // Scale speed, limit to 50
        double turnSpeed = angle_to_target * 0.5;

        Bot::LeftMotors.spin(vex::forward, forwardSpeed + turnSpeed, vex::pct);
        Bot::RightMotors.spin(vex::forward, forwardSpeed - turnSpeed, vex::pct);

        Bot::Brain.Screen.printAt(130, 130, "Driving To WallStake");
        dostop = true;
        vex::this_thread::sleep_for(20);
    }
}

// Rotate to specified angle using inertial sensor
void Odometry::rotateToAngle(double targetAngle) {
    bool dostop = false;
    while (fabs(Odometry::heading - targetAngle) > 1.0) {
        if(!Odometry::MotorRunKey.pressing()) {
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
int Odometry::driveToNearestWallStake() {
    while (true)
    {
        double distanceToStake1 = Odometry::calculateDistanceToStake(Odometry::tallWallStake1X, Odometry::tallWallStake1Y);
        double distanceToStake2 = Odometry::calculateDistanceToStake(Odometry::tallWallStake2X, Odometry::tallWallStake2Y);

        //Bot::Brain.Screen.printAt(0, 70, "Calculated Distance Of Stakes");

        double targetX, targetY, perpendicularAngle;

        if (distanceToStake1 < distanceToStake2) {
            targetX = Odometry::tallWallStake1X + Odometry::approach_distance;
            targetY = Odometry::tallWallStake1Y;
            perpendicularAngle = 90.0;  // Align perpendicular to stake at (-1750, 0)
        } else {
            targetX = Odometry::tallWallStake2X - Odometry::approach_distance;
            targetY = Odometry::tallWallStake2Y;
            perpendicularAngle = 270.0; // Align perpendicular to stake at (1750, 0)
        }

        // Drive to the target position
        driveToPosition(targetX, targetY);

        // Rotate to face perpendicular
        rotateToAngle(perpendicularAngle);   
    }
    return 0;
}


// Function to Update Odometry Using Encoders and Bot:: Sensor
void Odometry::updateOdometry() {
    Odometry::heading = Bot::Inertial.heading(vex::degrees);

    double forward_distance = (Bot::RotationForward.position(vex::degrees) / 360) * Odometry::tracking_wheel_circumference; // Forward movement
    //Bot::Brain.Screen.printAt(100,100,"Forward Dis %f", forward_distance);
    double lateral_distance = (Bot::RotationLateral.position(vex::degrees) / 360) * Odometry::tracking_wheel_circumference; // Lateral movement

    Bot::RotationForward.resetPosition();
    Bot::RotationLateral.resetPosition();

    double deltaX = forward_distance * cos(Odometry::heading * M_PI / 180);
    double deltaY = lateral_distance * cos(Odometry::heading * M_PI / 180);

    x += deltaX;
    y += deltaY;
}

int Odometry::odometry() {
    Bot::Inertial.calibrate();
    while(Bot::Inertial.isCalibrating()) {
        if(!Bot::Inertial.installed()) break;
        Bot::Controller.Screen.setCursor(2,1);
        Bot::Controller.Screen.print("Calib Intert!");
        vex::this_thread::sleep_for(20);
    }
    //Bot::Controller.Screen.print("              ");
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
    Bot::Drivetrain.setDriveVelocity(100.0, vex::percent);
    Bot::Drivetrain.setTurnVelocity(100.0, vex::percent);
    vex::task updateLoop(odometry);
    vex::task driveLoop(driveToNearestWallStake);
    return 0;
}
