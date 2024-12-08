#include "Skills.h"
#include "vex.h"
#include "Odometry.h"
#include "Bot.h"
#include "UISystem.h"

bool Skills::isActive = false;
vex::task Skills::skillsTask = vex::task(Skills::_skillsFunc);
int Skills::skillsPositionSelect = 0;

double Skills::x = -1;
double Skills::y = -1;
double Skills::h = -1;


int Skills::_skillsFunc() {
    while(!isActive) {
        vex::this_thread::sleep_for(100);
    }
    SkillsStartingPosition start = Skills::StartingPosistions[Skills::skillsPositionSelect];


    if(!Bot::GpsF.installed() || !Bot::GpsL.installed() || !Bot::GpsR.installed() || !Bot::GpsB.installed()) return 1;

    while(true) {
        x = (Bot::GpsF.xPosition() + Bot::GpsL.xPosition() + Bot::GpsR.xPosition() + Bot::GpsB.xPosition())/4;
        y = (Bot::GpsF.yPosition() + Bot::GpsL.yPosition() + Bot::GpsR.yPosition() + Bot::GpsB.yPosition())/4;
        h = (Bot::GpsF.heading() + Bot::GpsL.heading() + Bot::GpsR.heading() + Bot::GpsB.heading())/4;






    }

}

bool Skills::isSkillsActive() {
    return Skills::isActive;
}

void Skills::deactiveSkills() {

}

void Skills::runSkills(int p) {
    //Stop Controller Input and Notification System For Now.
    Bot::IgnoreMain = true;
    Bot::IgnoreDisplay = true;
    Bot::IgnoreVision = true;

    //Change UI odo map on bot to recieve information from gps sensors
    UISystem::odoMap.elements.clear();
    UISystem::odoMap.xref = &x;
    UISystem::odoMap.yref = &y;
    UISystem::odoMap.headingref = &h;

    isActive == true;
}

