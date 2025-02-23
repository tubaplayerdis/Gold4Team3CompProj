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
    if(angle + Bot::Inertial.heading() < 0) return turnToPID(360 + (angle + Bot::Inertial.heading()));
    if(angle + Bot::Inertial.heading() > 360) return turnToPID(fabs(360 - Bot::Inertial.heading() - angle));
    return turnToPID(angle + Bot::Inertial.heading());
}