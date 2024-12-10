#include <iostream>
#include <fstream>
#include <string>
#include "Skills.h"
#include "vex.h"
#include "Odometry.h"
#include "Bot.h"
#include "UISystem.h"
#include "json/json11.hpp"

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
            Skills::x = calcAverages(0);
            Skills::y = calcAverages(1);
            Skills::h = calcAverages(2);
            //Actual Processings




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

std::vector<SkillsTask> getTasksFromFileData() {
    std::ifstream inputFile("tasks.json"); // Open the file

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return std::vector<SkillsTask>{SkillsTask{"Error"}};
    }

    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    std::string content = buffer.str();

    std::string errbuffer;

    json11::Json data = json11::Json::parse(content, errbuffer);
    if(data["tasks"].is_array()) {
        json11::Json::array main = data["tasks"].array_items();
        for(int i = 0; i < main.size(); i++) {
            if(main[i].is_object()) {
                json11::Json::object obj = main[i].object_items();
                
                SkillsTask task = {};
                task.name = obj["name"].string_value();

                if(obj["stype"].string_value() == "Driving") {

                } else if (obj["stype"].string_value() == "Segmenting") {

                } else if (obj["stype"].string_value() == "Turning") {
                    
                } else if (obj["stype"].string_value() == "Mogo") {
                    
                } else if (obj["stype"].string_value() == "Arm") {
                    
                } else if (obj["stype"].string_value() == "Endgame") {
                    
                }
            }
        }
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

