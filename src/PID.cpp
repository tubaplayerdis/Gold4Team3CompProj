#include "PID.h"
#include "vex.h"
#include "Bot.h"

template<typename T>
T clamp(T val, T mn, T mx){
    return std::max(std::min(val, mx), mn);
}
double getShortestPathError(double target, double current) {
    double error = target - current;
    if (error > 180) error -= 360;  // Convert large positive angles
    if (error < -180) error += 360; // Convert large negative angles
    return error;
}
//maybe add timeout that return 1.
int turnToPID(double targetAngle) {
    double error, integral = 0, derivative, lastError = 0;
    double power;
    
    while (true) {
        double currentAngle = Bot::Inertial.heading();
        error = getShortestPathError(targetAngle, currentAngle);
        //error = targetAngle - currentAngle;
        
        // Break if within a small margin
        if (fabs(error) < TURN_PID_MAX_TOLERANCE) break;

        integral += error;
        derivative = error - lastError;
        
        // Calculate motor power
        power = (TURNPIDKP * error) + (TURNPIDKI * integral) + (TURNPIDKD * derivative);
        
        // Limit power
        power = clamp(power, -TURN_PID_MAX_MOTOR_POWER, TURN_PID_MAX_MOTOR_POWER);
        
        // Apply power to motors (opposite directions for turning)
        Bot::LeftMotors.spin(vex::fwd, power, vex::pct);
        Bot::RightMotors.spin(vex::fwd, -power, vex::pct);

        lastError = error;
        vex::task::sleep(20);  // Small delay for stability
    }

    // Stop motors once target is reached
    Bot::LeftMotors.stop();
    Bot::RightMotors.stop();

    return 0;
}

int turnForPID(double angle) {
    double desire = angle + Bot::Inertial.heading();
    if(desire < 0) {
        return turnToPID(360 + desire);
    }
    if(desire > 360) {
        return turnToPID(desire - 360);
    }
    return turnToPID(desire);
}


int turnArmToPID(double targetAngle) {
    double error, integral = 0, derivative, lastError = 0;
    double power;
    int timeout = 0;
    
    while (true) {
        double currentAngle = Bot::ArmPot.position(vex::deg);
        error = targetAngle - currentAngle;
        if(timeout >= 90) {
            Bot::IgnoreArm = false;
            return 1;
        }
        //error = targetAngle - currentAngle;
        
        // Break if within a small margin
        if (fabs(error) < ARM_PID_MAX_TOLERANCE) break;

        integral += error;
        derivative = error - lastError;
        
        // Calculate motor power
        power = (ARMPIDKP * error) + (ARMPIDKI * integral) + (ARMPIDKD * derivative);
        
        // Limit power
        power = clamp(power, -ARM_PID_MAX_MOTOR_POWER, ARM_PID_MAX_MOTOR_POWER);
        
        // Apply power to motors
        Bot::Arm.spin(vex::forward, power, vex::pct);

        lastError = error;
        vex::task::sleep(20);  // Small delay for stability
        timeout++;
    }

    // Stop motors once target is reached
    Bot::Arm.stop();
    Bot::Controller.rumble("..");

    return 0;
}