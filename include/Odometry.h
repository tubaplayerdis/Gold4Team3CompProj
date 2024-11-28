#pragma once
#include "vex.h"
#include "Bot.h"
#include <cmath>
#include <algorithm>

class Odometry {
    public:
        static int setupAndStartOdometry();

        static void setHeading(double heading);

        static double x;
        static double y;
        static double heading;
        static vex::controller::button MotorRunKey;

        //Functions
        static void updateOdometry();
        static double calculateDistanceToStake(double stakeX, double stakeY);
        static void driveToPosition(double targetX, double targetY, bool override);
        static void rotateToAngle(double targetAngle, bool override);
        static int driveToNearestWallStake(bool override);
        static int wrapperwallstake();
        static int odometry();

        //Consts
        static constexpr double tracking_wheel_radius = 1.375;                  // Radius in inches (assuming 1:1 gear ratio)
        static constexpr double tracking_wheel_circumference = 2 * M_PI * tracking_wheel_radius;
        static const double tallWallStake1X = -30.0; // X position of the first wall stake
        static const double tallWallStake1Y = -35.0;     // Y position of the first wall stake
        static const double tallWallStake2X = 50.0;  // X position of the second wall stake
        static const double tallWallStake2Y = 50.0;     // Y position of the second wall stake
        static gameElementPosition startMobilePP;
        static gameElementPosition startMobileNP;
        static gameElementPosition startMobileNN;
        static gameElementPosition startMobilePN;

        static constexpr double approach_distance = 152.4; // Distance to stop in front of wall stake, 6 inches converted to mm

};