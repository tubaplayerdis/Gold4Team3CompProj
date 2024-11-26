#include "UISystem.h"
#include <string>
#include <cmath>
#include <sstream>
#include "vex.h"
#include "api/vexui.h"
#include "Bot.h"


std::string to_string_int_f(int x){
  std::stringstream s;
  s << x;
  s << "  ";
  return s.str();
}

std::string to_string_double_f(double x){
  x = round(x);
  std::stringstream s;
  s << x;
  return s.str();
}


bool UISystem::doRender = true;

vexui::StartingPosition UISystem::Blue_Left = {vexui::OdometryPoint{-48, -60}, 0, "Blue Left"};
vexui::StartingPosition UISystem::Blue_Right = {vexui::OdometryPoint{48, -60}, 0, "Blue Right"};
vexui::StartingPosition UISystem::Red_Left = {vexui::OdometryPoint{-48, 124}, 180, "Red Left"};
vexui::StartingPosition UISystem::Red_Right = {vexui::OdometryPoint{48, 124}, 180, "Red Right"};
vexui::StartingPosition UISystem::Center = {vexui::OdometryPoint{0, 0}, 0, "Center"};
bool UISystem::showStartingGoals = true;
std::vector<vexui::StartingPosition> UISystem::positions = std::vector<vexui::StartingPosition>{UISystem::Blue_Left, UISystem::Blue_Right, UISystem::Red_Left, UISystem::Red_Right, UISystem::Center};
int UISystem::SelectedPosition = 0;
/*The Vex Brain Sceen is 480 x 240*/

vexui::Button UISystem::mainTabButton = vexui::Button(0,0,100,40, "Main");
vexui::Button UISystem::odometryTabButton = vexui::Button(100,0,100,40, "Odometry");
vexui::Button UISystem::consoleTabButton = vexui::Button(200,0,100,40, "Console");

vexui::Panel UISystem::mainPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::odometryPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::consolePanel = vexui::Panel(0,40,480,200);

vexui::Label UISystem::watermark = vexui::Label(315, 15, "GBS 38535B");

//Main Panel Elements
vexui::Label UISystem::labm = vexui::Label(10,10, "AI Vision");
vexui::Label UISystem::motorTempLabel = vexui::Label(10, 100, "Temps:");
vexui::Label UISystem::redRingNumLabel = vexui::Label(10,30, "Red Ring#:");
vexui::Label UISystem::blueRingNumLabel = vexui::Label(10,50, "Blue Ring#: ");
vexui::Label UISystem::MobileGoalNumLabel = vexui::Label(10,70, "Moblie Goal#: ");

//Odometry Panel Elements
vexui::Label UISystem::labo = vexui::Label(250,10, "Calibration");
vexui::OdometryMap UISystem::odoMap = vexui::OdometryMap(5,5, &Odometry::x, &Odometry::y, &Odometry::heading, vexui::OdometryUnits::INCHES);
vexui::Label UISystem::calibrationSelectLabel = vexui::Label(310, 60, "Select Position");
vexui::Button UISystem::calibrationPositionBackButton = vexui::Button(245, 50, 40, 40, "<");
vexui::Button UISystem::calibrationPositionForwardButton = vexui::Button(440, 50, 40, 40, ">");
vexui::Label UISystem::calibrationWarningLabel = vexui::Label(205, 100, "Will Calibrate On Position Select*!");

//Console Panel Elements
vexui::Label UISystem::labc = vexui::Label(10,10, "Console Panel");
 

void UISystem::mainTabButton_Press() {
    mainPanel.dorender = true;
    odometryPanel.dorender = false;
    consolePanel.dorender = false;
}

void UISystem::odometryTabButton_Press() {
    mainPanel.dorender = false;
    odometryPanel.dorender = true;
    consolePanel.dorender = false;
}

void UISystem::consoleTabButton_Press() {
    mainPanel.dorender = false;
    odometryPanel.dorender = false;
    consolePanel.dorender = true;
}

void IncreaseSelectedPosisiton() {
    UISystem::SelectedPosition++;
    if(UISystem::SelectedPosition > UISystem::positions.size() -1 || UISystem::SelectedPosition < 0) UISystem::SelectedPosition = 0;
    UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
    UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
    UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
    Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
    UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);
}

void DecreaseSelectedPosisiton() {
    UISystem::SelectedPosition--;
    if(UISystem::SelectedPosition > UISystem::positions.size() -1 || UISystem::SelectedPosition < 0) UISystem::SelectedPosition = 0;
    UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
    UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
    UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
    Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
    UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);
}

void UISystem::setup() {

    vexDisplayFontNamedSet("mono12");

    mainTabButton.pressEvent.addListener(UISystem::mainTabButton_Press);
    odometryTabButton.pressEvent.addListener(UISystem::odometryTabButton_Press);
    consoleTabButton.pressEvent.addListener(UISystem::consoleTabButton_Press);
    mainPanel.dorender = false;
    odometryPanel.dorender = true;
    consolePanel.dorender = false;

    labm.bgcolor = mainPanel.color;
    labo.bgcolor = odometryPanel.color;
    labc.bgcolor = consolePanel.color;

    labo.txsize = vexui::LARGE;
    motorTempLabel.txsize = vexui::SMALL;
    calibrationSelectLabel.bgcolor = odometryPanel.color;
    calibrationPositionBackButton.pressEvent.addListener(DecreaseSelectedPosisiton);
    calibrationPositionForwardButton.pressEvent.addListener(IncreaseSelectedPosisiton);
    calibrationWarningLabel.txsize = vexui::SMALL;
    calibrationWarningLabel.bgcolor = odometryPanel.color;
    calibrationWarningLabel.color.mset(153, 42, 28);

    mainPanel.addElement(&labm);
    mainPanel.addElement(&redRingNumLabel);
    mainPanel.addElement(&blueRingNumLabel);
    mainPanel.addElement(&MobileGoalNumLabel);
    mainPanel.addElement(&motorTempLabel);



    consolePanel.addElement(&labc);


    odometryPanel.addElement(&labo);
    odometryPanel.addElement(&odoMap);
    odometryPanel.addElement(&calibrationSelectLabel);
    odometryPanel.addElement(&calibrationPositionBackButton);
    odometryPanel.addElement(&calibrationPositionForwardButton);
    odometryPanel.addElement(&calibrationWarningLabel);

    UISystem::calibrationSelectLabel.setText(UISystem::positions[UISystem::SelectedPosition].name);
    UISystem::odoMap.setNewX(UISystem::positions[UISystem::SelectedPosition].pos.x, vexui::INCHES);
    UISystem::odoMap.setNewY(UISystem::positions[UISystem::SelectedPosition].pos.y, vexui::INCHES);
    Bot::Inertial.setHeading(UISystem::positions[UISystem::SelectedPosition].heading, vex::degrees);
    UISystem::odoMap.setNewH(UISystem::positions[UISystem::SelectedPosition].heading);



    UISystem::odoMap.elements.push_back(vexui::OdometryGameElement{(int)Odometry::startMobileNN.x, (int)Odometry::startMobileNN.y, Odometry::startMobileNN.objtype, UISystem::showStartingGoals});
    UISystem::odoMap.elements.push_back(vexui::OdometryGameElement{(int)Odometry::startMobileNP.x, (int)Odometry::startMobileNP.y, Odometry::startMobileNP.objtype, UISystem::showStartingGoals});
    UISystem::odoMap.elements.push_back(vexui::OdometryGameElement{(int)Odometry::startMobilePN.x, (int)Odometry::startMobilePN.y, Odometry::startMobilePN.objtype, UISystem::showStartingGoals});
    UISystem::odoMap.elements.push_back(vexui::OdometryGameElement{(int)Odometry::startMobilePP.x, (int)Odometry::startMobilePP.y, Odometry::startMobilePP.objtype, UISystem::showStartingGoals});
}

void UISystem::toggleUI() {
    doRender = !doRender;
}

int UISystem::renderLoop() {

    while(true) {
        vexDisplayErase();
        
        redRingNumLabel.setText("Red Rings: "+to_string_int_f(Bot::redRingNum));
        blueRingNumLabel.setText("Blue Rings: "+to_string_int_f(Bot::redRingNum));
        MobileGoalNumLabel.setText("Mobile Goals: "+to_string_int_f(Bot::redRingNum));

        motorTempLabel.setText("L1 "+to_string_double_f(Bot::LeftA.temperature())+" L2 "+to_string_double_f(Bot::LeftB.temperature())+" L3 "+to_string_double_f(Bot::LeftC.temperature())+" R1 "+to_string_double_f(Bot::RightA.temperature())+" R2 "+to_string_double_f(Bot::RightB.temperature())+" R3 "+to_string_double_f(Bot::RightC.temperature()));

        if(!doRender) continue;
        //Bot::Brain.Screen.printAt(200,200, "ummm");
        UISystem::mainTabButton.render();
        UISystem::odometryTabButton.render();
        UISystem::consoleTabButton.render();
        UISystem::mainPanel.render();
        UISystem::odometryPanel.render();
        UISystem::consolePanel.render();
        UISystem::watermark.render();
        vexDisplayRender(1,1);
        

        vex::this_thread::sleep_for(20);
    }
    return 1;
}