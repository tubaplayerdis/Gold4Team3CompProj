#include "Skills.h"
#include "vex.h"
#include "Odometry.h"
#include "Bot.h"
#include "UISystem.h"

void SkillsEngine::init(std::vector<SkillsTask> _tasks) {
    tasks = _tasks;
    onTaskIndex = 0;
    executor = vex::task(_worker);
    executor.suspend();
}

void SkillsEngine::execute() {
    executor.resume();
}

void SkillsEngine::pause() {
    executor.suspend();
}

void SkillsEngine::restart() {
    executor.suspend();
    for(SkillsTask obj : tasks)
    {
        obj.isComplete = false;
    }
    executor.resume();
}

void SkillsEngine::addTask(SkillsTask task) {
    tasks.push_back(task);
}

int SkillsEngine::currentTaskIndex() {
    return onTaskIndex;
}

SkillsTask SkillsEngine::currentTask() {
    return tasks[onTaskIndex];
}

int SkillsEngine::_worker() {
    while(true) {
        for(int i = 0; i < tasks.size(); i++) {
            onTaskIndex = i;
            //Actual Processing




        }
        bool allComplete = true;
        for(SkillsTask obj : tasks)
        {
            if(obj.isComplete == false) {
                allComplete = false;
            }
        }
        if(allComplete) {
            Bot::controllerNotification("SKILLS COMPLETE");
            break;
        }
    }
}



bool Skills::isActive = false;
vex::task Skills::skillsTask = vex::task(Skills::_skillsFunc);
int Skills::skillsPositionSelect = 0;

double Skills::x = -1;
double Skills::y = -1;
double Skills::h = -1;


double qualityCheck(vex::gps g) {
    if(g.quality() < 95) return 0.0;
    return 1.0;
}

double calcAverages(int w) {
    if(Bot::GpsF.quality() < 95 && Bot::GpsL.quality() && Bot::GpsR.quality() && Bot::GpsB.quality()) {
        Notifications::addNotification("DEFAULT ODOM!");        
        //Doomsday Scenario
        switch(w) {
            case 0:
                return Odometry::x * 25.4;
            case 1:
                return Odometry::y * 25.4;
            case 2:
                return Odometry::heading;
            default:
                return -1;
    }
    }
    switch(w) {
        case 0:
            return ((Bot::GpsF.xPosition() * qualityCheck(Bot::GpsF)) *  + (Bot::GpsL.xPosition() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.xPosition() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.xPosition() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        case 1:
            return ((Bot::GpsF.yPosition() * qualityCheck(Bot::GpsF)) *  + (Bot::GpsL.yPosition() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.yPosition() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.yPosition() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        case 2:
            return ((Bot::GpsF.heading() * qualityCheck(Bot::GpsF)) *  + (Bot::GpsL.heading() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.heading() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.heading() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        default:
         return -1;
    }
}

int Skills::_skillsFunc() {
    while(!isActive) {
        vex::this_thread::sleep_for(100);
    }


    if(!Bot::GpsF.installed() || !Bot::GpsL.installed() || !Bot::GpsR.installed() || !Bot::GpsB.installed()) return 1;



    while(true) {
        x = calcAverages(0);
        y = calcAverages(1);
        h = calcAverages(2);
        //Odometry::x = x;
        //Odometry::y = y;
        //Odometry::heading = h;





    }

}

bool Skills::isSkillsActive() {
    return Skills::isActive;
}

void Skills::deactiveSkills() {

}

void Skills::runSkills(int p) {
    //Stop Controller Input and Notification System For Now.
    //Bot::IgnoreMain = true;
   // Bot::IgnoreDisplay = true;
   // Bot::IgnoreVision = true;

    //Change UI odo map on bot to recieve information from gps sensors
    UISystem::odoMap.elements.clear();
    UISystem::odoMap.xref = &x;
    UISystem::odoMap.yref = &y;
    UISystem::odoMap.headingref = &h;



    isActive = true;

    Bot::IgnoreDisplay = false;
}

