#include "UISystem.h"
#include "vex.h"
#include "api/vexui.h"


bool UISystem::doRender = false;

/*The Vex Brain Sceen is 480 x 240*/

vexui::Button UISystem::mainTabButton = vexui::Button(0,0,40,20, "Main");
vexui::Button UISystem::odometryTabButton = vexui::Button(40,0,40,20, "Odometry");
vexui::Button UISystem::consoleTabButton = vexui::Button(80,0,40,20, "Console");

vexui::Panel UISystem::mainPanel = vexui::Panel(0,20,480,220);
vexui::Panel UISystem::odometryPanel = vexui::Panel(0,20,480,220);
vexui::Panel UISystem::consolePanel = vexui::Panel(0,20,480,220);

vexui::Label UISystem::watermark = vexui::Label(120, 10, "GBS 38535B");

vexui::Label UISystem::labm = vexui::Label(10,10, "Main Panel");
vexui::Label UISystem::labo = vexui::Label(10,10, "Odometry Panel");
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
        UISystem::watermark.render();
        //render Elements
        /*
        if(!doRender) continue;
        UISystem::watermark.render();
        UISystem::mainTabButton.render();
        UISystem::odometryTabButton.render();
        UISystem::consoleTabButton.render();
        UISystem::mainPanel.render();
        UISystem::odometryPanel.render();
        UISystem::consolePanel.render();
        */

        vex::this_thread::sleep_for(20);
    }
    return 1;
}