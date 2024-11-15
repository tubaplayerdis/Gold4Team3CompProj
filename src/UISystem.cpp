#include "UISystem.h"

using namespace vexui;

bool UISystem::doRender = false;

/*The Vex Brain Sceen is 480 x 240*/

Button UISystem::mainTabButton = Button(0,0,40,20, "Main");
Button UISystem::odometryTabButton = Button(40,0,40,20, "Odometry");
Button UISystem::consoleTabButton = Button(80,0,40,20, "Console");

Panel UISystem::mainPanel = Panel(0,20,480,220);
Panel UISystem::odometryPanel = Panel(0,20,480,220);
Panel UISystem::consolePanel = Panel(0,20,480,220);

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

    mainPanel.addElement(Label(10,10,"Main Panel"));
    odometryPanel.addElement(Label(10,10,"Odometry Panel"));
    consolePanel.addElement(Label(10,10,"Console Panel"));
}

void UISystem::toggleUI() {
    doRender = !doRender;
}

int UISystem::renderLoop() {
    while(true) {
        //render Elements
        UISystem::mainTabButton.render();
        UISystem::odometryTabButton.render();
        UISystem::consoleTabButton.render();
        UISystem::mainPanel.render();
        UISystem::odometryPanel.render();
        UISystem::consolePanel.render();
    }
    return 1;
}