#include "UISystem.h"
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


bool UISystem::doRender = true;

/*The Vex Brain Sceen is 480 x 240*/

vexui::Button UISystem::mainTabButton = vexui::Button(0,0,100,40, "Main");
vexui::Button UISystem::odometryTabButton = vexui::Button(100,0,100,40, "Odometry");
vexui::Button UISystem::consoleTabButton = vexui::Button(200,0,100,40, "Console");

vexui::Panel UISystem::mainPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::odometryPanel = vexui::Panel(0,40,480,200);
vexui::Panel UISystem::consolePanel = vexui::Panel(0,40,480,200);

vexui::Label UISystem::watermark = vexui::Label(0, 90, "GBS 38535B");

vexui::Label UISystem::labm = vexui::Label(10,10, "Main Panel");
vexui::Label UISystem::labo = vexui::Label(50,50, "Odometry Panel");
vexui::Label UISystem::labc = vexui::Label(100,100, "Console Panel");

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

void UISystem::setup() {
    mainTabButton.pressEvent.addListener(UISystem::mainTabButton_Press);
    odometryPanel.pressEvent.addListener(UISystem::odometryTabButton_Press);
    consoleTabButton.pressEvent.addListener(UISystem::consoleTabButton_Press);
    mainPanel.dorender = true;
    odometryPanel.dorender = false;
    consolePanel.dorender = false;

    mainPanel.addElement(labm);
    odometryPanel.addElement(labo);
    consolePanel.addElement(labc);
}

void UISystem::toggleUI() {
    doRender = !doRender;
}

int UISystem::renderLoop() {
    while(true) {
        
        V5_TouchStatus stats;
        vexTouchDataGet(&stats);
        std::stringstream stream;
        //stream << "x: " << stats.lastXpos << ", y: " << stats.lastYpos << ", xv: " << mainTabButton.x << ", yv: " << mainTabButton.y << ", w: " << mainTabButton.width << ", h: " << mainTabButton.height << ", P: " << mainTabButton.isPress();
        stream << mainPanel.dorender << " " << odometryPanel.dorender << " " << consolePanel.dorender << " ";
        watermark.text = stream.str();
        
        if(!doRender) continue;
        //Bot::Brain.Screen.printAt(200,200, "ummm");
        UISystem::mainTabButton.render();
        UISystem::odometryTabButton.render();
        UISystem::consoleTabButton.render();
        UISystem::mainPanel.render();
        UISystem::odometryPanel.render();
        UISystem::consolePanel.render();
        UISystem::watermark.render();
        bool what = vexDisplayRender(1,1);
        

        vex::this_thread::sleep_for(20);
    }
    return 1;
}