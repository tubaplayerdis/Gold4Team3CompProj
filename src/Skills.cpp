#include <iostream>
#include <fstream>
#include <string>
#include "Skills.h"
#include "vex.h"
#include "Odometry.h"
#include "Bot.h"
#include "UISystem.h"
#include "json/json11.hpp"

double qualityCheck(vex::gps g) {
    if(g.quality() < 90) return 0.0;
    return 1.0;
}

double calcAverages(int w) {
    if(Bot::GpsF.quality() < 90 && Bot::GpsL.quality() < 90 && Bot::GpsR.quality() < 90 && Bot::GpsB.quality() < 90) {
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
    } else {
        for(int i = 0; i < Notifications::NotificationList.size(); i++) {
            if(Notifications::NotificationList[i] == "DEFAULT ODOM!") Notifications::NotificationList.erase(Notifications::NotificationList.begin() + i);
        }
    }
    switch(w) {
        case 0:
            return ((Bot::GpsF.xPosition() * qualityCheck(Bot::GpsF)) + (Bot::GpsL.xPosition() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.xPosition() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.xPosition() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        case 1:
            return ((Bot::GpsF.yPosition() * qualityCheck(Bot::GpsF)) + (Bot::GpsL.yPosition() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.yPosition() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.yPosition() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        case 2:
            return ((Bot::GpsF.heading() * qualityCheck(Bot::GpsF)) + (Bot::GpsL.heading() * qualityCheck(Bot::GpsL)) + (Bot::GpsR.heading() * qualityCheck(Bot::GpsR)) + (Bot::GpsB.heading() * qualityCheck(Bot::GpsB)))/(qualityCheck(Bot::GpsF) + qualityCheck(Bot::GpsL) + qualityCheck(Bot::GpsR) + qualityCheck(Bot::GpsB));
        default:
         return -1;
    }
}

int SkillsEngine::onTaskIndex = 0;
vex::task SkillsEngine::executor = vex::task(SkillsEngine::_worker);
std::vector<SkillsTask> SkillsEngine::tasks = std::vector<SkillsTask>();

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

void interpretSkill(SkillsTask task) {
    if(SkillsEngine::tasks.size() == 0) return;
    switch (task.stype)
    {
        case Driving:
            /*
            Do IMPL
            */
            break;
        case Segmenting:
            Bot::Drivetrain.driveFor(task.segAmount, vex::mm, true);
            break;
        case Turning:
            Bot::Drivetrain.turnFor(task.turnBy, vex::degrees, true);
            break;
        case Mogo:
            Bot::MogoMech.set(task.toggleMogoTo);
            break;
        case Arm:
            Bot::desiredARMAngle = task.turnArmTo;
            Bot::isArmPIDActive = true;
            break;
        case EndGame:
            Bot::Clutch.set(task.togglePTUTo);
            break;
        case Doinker:
            Bot::Doinker.set(task.toggleDoinkerTO);
            break;
        default:
            break;
    }

}

int SkillsEngine::_worker() {
    while(true) {
        for(int i = 0; i < tasks.size(); i++) {
            onTaskIndex = i;
            Skills::x = calcAverages(0);
            Skills::y = calcAverages(1);
            Skills::h = calcAverages(2);
            Skills::ix = Skills::x/25.4;
            Skills::iy = Skills::y/25.4;
            //Actual Processings

            interpretSkill(tasks[i]);


        }
        bool allComplete = true;
        for(SkillsTask obj : tasks)
        {
            if(obj.isComplete == false) {
                allComplete = false;
            }
        }
        if(allComplete) {
            if(!Bot::Comp.isAutonomous())  Bot::controllerNotification("SKILLS COMPLETE");
            break;
        }
    }
    return 1;
}



bool Skills::isActive = false;

double Skills::x = -1;
double Skills::y = -1;
double Skills::h = -1;

double Skills::ix = -1;
double Skills::iy = -1;

std::vector<SkillsTask> getTasksFromFileData() {
    std::ifstream inputFile("tasks.json"); // Open the file

    if (!inputFile.is_open()) {
        Notifications::addNotification("JSON FILE ERROR");
        return std::vector<SkillsTask>{SkillsTask{"Error"}};
    }

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    std::string content = buffer.str();

    std::string errbuffer;

    json11::Json data = json11::Json::parse(content, errbuffer);
    std::vector<SkillsTask> tasks = std::vector<SkillsTask>();
    if(data["tasks"].is_array()) {
        json11::Json::array main = data["tasks"].array_items();
        for(int i = 0; i < main.size(); i++) {
            if(main[i].is_object()) {
                json11::Json::object obj = main[i].object_items();
                
                SkillsTask task = {};
                task.name = obj["name"].string_value();

                if(obj["stype"].string_value() == "Driving" || obj["stype"].string_value() == "drv") {
                    task.stype = Driving;
                    task.tp = obj["tp"].int_value();
                    task.vp = obj["vp"].int_value();
                    task.destx = obj["destx"].number_value();
                    task.desty = obj["desty"].number_value();
                    task.desth = obj["desth"].number_value();

                } else if (obj["stype"].string_value() == "Segmenting" || obj["stype"].string_value() == "seg") {
                    task.stype = Segmenting;
                    task.vp = obj["vp"].int_value();
                    task.segAmount = obj["segDist"].number_value();

                } else if (obj["stype"].string_value() == "Turning" || obj["stype"].string_value() == "tur") {
                    task.stype = Turning;
                    task.tp = obj["tp"].int_value();
                    task.turnBy = obj["turnBy"].number_value();

                } else if (obj["stype"].string_value() == "Mogo" || obj["stype"].string_value() == "mog") {
                    task.stype = Mogo;
                    task.toggleMogoTo = obj["setTo"].bool_value();


                } else if (obj["stype"].string_value() == "Arm" || obj["stype"].string_value() == "arm") {
                    task.stype = Arm;
                    task.turnArmTo = obj["armTo"].number_value();

                } else if (obj["stype"].string_value() == "Endgame" || obj["stype"].string_value() == "end") {
                    task.stype = EndGame;
                    task.togglePTUTo = obj["endTo"].bool_value();
                }
                else if (obj["stype"].string_value() == "Doinker" || obj["stype"].string_value() == "doi") {
                    task.stype = Doinker;
                    task.toggleDoinkerTO = obj["doinkTo"].bool_value();
                } else {
                    task.stype = Error;
                    task.name = "Error: " + obj["name"].string_value();
                }

                tasks.push_back(task);
            }
        }
    }

    return tasks;

}

bool Skills::isSkillsActive() {
    return Skills::isActive;
}

void Skills::deactiveSkills() {
    //Do not use
}

void Skills::runSkills(int p) {
    vex::competition Comp;
    if(!Comp.isAutonomous()) {
        Notifications::addNotification("DRIVER");
        return;
    }
    //Stop Controller Input and Notification System For Now.
    //Bot::IgnoreMain = true;
   // Bot::IgnoreDisplay = true;
   // Bot::IgnoreVision = true;

    //Change UI odo map on bot to recieve information from gps sensors
    UISystem::odoMap.elements.clear();
    UISystem::odoMap.xref = &ix;
    UISystem::odoMap.yref = &iy;
    UISystem::odoMap.headingref = &h;

    if(!Bot::GpsF.installed() || !Bot::GpsL.installed() || !Bot::GpsR.installed() || !Bot::GpsB.installed()) {
        Notifications::addNotification("GPS MISSING");
        return;
    }

    if(Bot::Brain.SDcard.isInserted() && Bot::Brain.SDcard.exists("tasks.json")) {
        SkillsEngine::init(getTasksFromFileData());
    } else {
        Notifications::addNotification("NO SD CARD");
        return;
    }

}

