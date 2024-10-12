#pragma once
#include "vex.h"
#include "Bot.h"

class Drivetrain {
  public:
    // define variable for remote controller enable/disable
    static bool RemoteControlCodeEnabled;
    // define variables used for controlling motors based on controller inputs
    static bool DrivetrainLNeedsToBeStopped;
    static bool DrivetrainRNeedsToBeStopped;

    // define a task that will handle monitoring inputs from bot::Controller
    static int ControllerLoop();

};


//task rc_auto_loop_task_bot::Controller(rc_auto_loop_function_bot::Controller);