#pragma once
#include "vex.h"
#include "Bot.h"

//This file consists mostly of helper functions for certian parts of the robot that involve PID.

#define TURNPIDKP 0.6
#define TURNPIDKI 0.0005
#define TURNPIDKD 0.5

#define TURN_PID_MAX_MOTOR_POWER 100.0
#define TURN_PID_MAX_TOLERANCE 1.0

#define ARMPIDKP 0.9
#define ARMPIDKI 0.0005
#define ARMPIDKD 0.5

#define ARM_PID_MAX_MOTOR_POWER 100.0
#define ARM_PID_MAX_TOLERANCE 1.0

int turnToPID(double targetAngle);
int turnForPID(double angle);

int turnArmToPID(double target);
