#include "UISystem.h"
#include <string>
#include <cmath>
#include <sstream>
#include "vex.h"
#include "api/vexui.h"
#include "Bot.h"
#include <cstdlib>
#include <cstdio>
#include <sys/resource.h>
#include <errno.h>


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
vexui::Button UISystem::diagnosticsTabButton = vexui::Button(200,0,100,40, "Info");

vexui::Panel UISystem::mainPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::odometryPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::diagnosticsPanel = vexui::Panel(0,40,480,200);

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

vexui::Panel UISystem::motorTempVisualPanel = vexui::Panel(200,10, 250, 110);
vexui::Rectangle UISystem::LeftATempRec = vexui::Rectangle(0,60,50,110);
vexui::Rectangle UISystem::LeftBTempRec = vexui::Rectangle(55,20,105,70);
vexui::Rectangle UISystem::LeftCTempRec = vexui::Rectangle(0,0,50,50);
vexui::Rectangle UISystem::RightATempRec = vexui::Rectangle(200,60,250,110);
vexui::Rectangle UISystem::RightBTempRec = vexui::Rectangle(145,20,195,70);
vexui::Rectangle UISystem::RightCTempRec = vexui::Rectangle(200,0,250,50);

//Console Panel Elements
vexui::Label UISystem::labc = vexui::Label(10,10, "Diagnostics Panel");
vexui::DropdownS UISystem::diagDropdown = vexui::DropdownS(5,5, 180, 30, "Notifications", false);
vexui::Label UISystem::memFreeLabel = vexui::Label(0,0," ");
 

void UISystem::mainTabButton_Press() {
    mainPanel.dorender = true;
    odometryPanel.dorender = false;
    diagnosticsPanel.dorender = false;
}

void UISystem::odometryTabButton_Press() {
    mainPanel.dorender = false;
    odometryPanel.dorender = true;
    diagnosticsPanel.dorender = false;
}

void UISystem::diagnosticsTabButton_Press() {
    mainPanel.dorender = false;
    odometryPanel.dorender = false;
    diagnosticsPanel.dorender = true;
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

void toggleDiagDropdown() {
    UISystem::diagDropdown.toggle();
}

void UISystem::setup() {

    vexDisplayFontNamedSet("mono12");

    mainTabButton.pressEvent.addListener(UISystem::mainTabButton_Press);
    odometryTabButton.pressEvent.addListener(UISystem::odometryTabButton_Press);
    diagnosticsTabButton.pressEvent.addListener(UISystem::diagnosticsTabButton_Press);
    mainPanel.dorender = false;
    odometryPanel.dorender = true;
    diagnosticsPanel.dorender = false;

    labm.bgcolor = mainPanel.color;
    labo.bgcolor = odometryPanel.color;
    labc.bgcolor = diagnosticsPanel.color;

    labo.txsize = vexui::LARGE;
    motorTempLabel.txsize = vexui::SMALL;
    calibrationSelectLabel.bgcolor = odometryPanel.color;
    calibrationPositionBackButton.pressEvent.addListener(DecreaseSelectedPosisiton);
    calibrationPositionForwardButton.pressEvent.addListener(IncreaseSelectedPosisiton);
    calibrationWarningLabel.txsize = vexui::SMALL;
    calibrationWarningLabel.bgcolor = odometryPanel.color;
    calibrationWarningLabel.color.mset(153, 42, 28);

    diagnosticsPanel.addElement(&motorTempVisualPanel);

    mainPanel.addElement(&labm);
    mainPanel.addElement(&redRingNumLabel);
    mainPanel.addElement(&blueRingNumLabel);
    mainPanel.addElement(&MobileGoalNumLabel);
    mainPanel.addElement(&motorTempLabel);


    motorTempVisualPanel.addElement(&LeftATempRec);
    motorTempVisualPanel.addElement(&LeftBTempRec);
    motorTempVisualPanel.addElement(&LeftCTempRec);
    motorTempVisualPanel.addElement(&RightATempRec);
    motorTempVisualPanel.addElement(&RightBTempRec);
    motorTempVisualPanel.addElement(&RightCTempRec);
    motorTempVisualPanel.color = diagnosticsPanel.color;

    LeftATempRec.showText = true;
    LeftATempRec.hasBorder = true;
    LeftATempRec.width = motorTempVisualPanel.x + LeftATempRec.width;
    LeftATempRec.height = motorTempVisualPanel.y + LeftATempRec.height;

    LeftBTempRec.showText = true;
    LeftBTempRec.hasBorder = true;
    LeftBTempRec.width = motorTempVisualPanel.x + LeftBTempRec.width;
    LeftBTempRec.height = motorTempVisualPanel.y + LeftBTempRec.height;

    LeftCTempRec.showText = true;
    LeftCTempRec.hasBorder = true;
    LeftCTempRec.width = motorTempVisualPanel.x + LeftCTempRec.width;
    LeftCTempRec.height = motorTempVisualPanel.y + LeftCTempRec.height;

    RightATempRec.showText = true;
    RightATempRec.hasBorder = true;
    RightATempRec.width = motorTempVisualPanel.x + RightATempRec.width;
    RightATempRec.height = motorTempVisualPanel.y + RightATempRec.height;

    RightBTempRec.showText = true;
    RightBTempRec.hasBorder = true;
    RightBTempRec.width = motorTempVisualPanel.x + RightBTempRec.width;
    RightBTempRec.height = motorTempVisualPanel.y + RightBTempRec.height;

    RightCTempRec.showText = true;
    RightCTempRec.hasBorder = true;
    RightCTempRec.width = motorTempVisualPanel.x + RightCTempRec.width;
    RightCTempRec.height = motorTempVisualPanel.y + RightCTempRec.height;



    memFreeLabel.bgcolor = diagDropdown.bgcolor;
    memFreeLabel.color.mset(0,0,0);
    //I hate this workaround so much
    diagDropdown.pressEvent.addListener(toggleDiagDropdown);
    //diagDropdown.addElement(&memFreeLabel);
    //diagnosticsPanel.addElement(&labc);
    diagnosticsPanel.addElement(&diagDropdown);


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

double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

vexui::Color calculateColorFromTemperature(double temperature) {
    double minTemp = 30;
    double maxTemp = 140;

    double t = (temperature - minTemp) / (maxTemp - minTemp);

    if (temperature < minTemp) temperature = minTemp;
    if (temperature > maxTemp) temperature = maxTemp;

    std::array<double, 3> colorStart = {173, 216, 230}; // Baby blue (RGB)
    std::array<double, 3> colorEnd = {255, 0, 0};       // Red (RGB)

    // Interpolate RGB values
    int r = static_cast<int>(std::round(lerp(colorStart[0], colorEnd[0], t)));
    int g = static_cast<int>(std::round(lerp(colorStart[1], colorEnd[1], t)));
    int b = static_cast<int>(std::round(lerp(colorStart[2], colorEnd[2], t)));

    return vexui::Color(r,g,b,false);
}

int UISystem::renderLoop() {

    while(true) {
        vexDisplayErase();
        
        redRingNumLabel.setText("Red Rings: "+to_string_int_f(Bot::redRingNum));
        blueRingNumLabel.setText("Blue Rings: "+to_string_int_f(Bot::redRingNum));
        MobileGoalNumLabel.setText("Mobile Goals: "+to_string_int_f(Bot::redRingNum));
        

        if(diagnosticsPanel.dorender) {

            if(!Bot::LeftA.installed()) {
                LeftATempRec.text = "LA";
                LeftATempRec.extext = to_string_double_f(129);
                LeftBTempRec.text = "LB";
                LeftBTempRec.extext = to_string_double_f(129);
                LeftCTempRec.text = "LC";
                LeftCTempRec.extext = to_string_double_f(129);
                RightATempRec.text = "RA";
                RightATempRec.extext = to_string_double_f(129);
                RightBTempRec.text = "RB";
                RightBTempRec.extext = to_string_double_f(129);
                RightCTempRec.text = "RC";
                RightCTempRec.extext = to_string_double_f(129);
                LeftATempRec.color = calculateColorFromTemperature(129);
                LeftBTempRec.color = calculateColorFromTemperature(129);
                LeftCTempRec.color = calculateColorFromTemperature(129);
                RightATempRec.color = calculateColorFromTemperature(129);
                RightBTempRec.color = calculateColorFromTemperature(129);
                RightCTempRec.color = calculateColorFromTemperature(129);
            } else {
                LeftATempRec.text = "LA";
                LeftATempRec.extext = to_string_double_f(Bot::LeftA.temperature());
                LeftBTempRec.text = "LB";
                LeftBTempRec.extext = to_string_double_f(Bot::LeftB.temperature());
                LeftCTempRec.text = "LC";
                LeftCTempRec.extext = to_string_double_f(Bot::LeftC.temperature());
                RightATempRec.text = "RA";
                RightATempRec.extext = to_string_double_f(Bot::RightA.temperature());
                RightBTempRec.text = "RB";
                RightBTempRec.extext = to_string_double_f(Bot::RightA.temperature());
                RightCTempRec.text = "RC";
                RightCTempRec.extext = to_string_double_f(Bot::RightC.temperature());
                LeftATempRec.color = calculateColorFromTemperature(Bot::LeftA.temperature());
                LeftBTempRec.color = calculateColorFromTemperature(Bot::LeftB.temperature());
                LeftCTempRec.color = calculateColorFromTemperature(Bot::LeftC.temperature());
                RightATempRec.color = calculateColorFromTemperature(Bot::RightA.temperature());
                RightBTempRec.color = calculateColorFromTemperature(Bot::RightB.temperature());
                RightCTempRec.color = calculateColorFromTemperature(Bot::RightC.temperature());
            }

            

        }

        if(!doRender) continue;
        //Bot::Brain.Screen.printAt(200,200, "ummm");
        UISystem::mainTabButton.render();
        UISystem::odometryTabButton.render();
        UISystem::diagnosticsTabButton.render();
        UISystem::mainPanel.render();
        UISystem::odometryPanel.render();
        UISystem::diagnosticsPanel.render();
        UISystem::watermark.render();
        vexDisplayRender(1,1);
        

        vex::this_thread::sleep_for(20);
    }
    return 1;
}