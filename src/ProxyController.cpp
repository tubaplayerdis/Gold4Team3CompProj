#include "ProxyController.h"
#include "vex.h"
#include "fstream"
#include "string"
#include "vector"

Button::Button(_V5_ControllerIndex keycode) {
    code = keycode;
    pressedCallBack = nullptr;
    releasedCallBack = nullptr;
    pressedTask = vex::thread();
    releasedTask = vex::thread();
}

void Button::pressed(void (* CallBack)(void)) {
    pressedCallBack = CallBack;
    //pressedTask = vex::thread(pressedCallBack);
}

void Button::released(void (* CallBack)(void)) {
    releasedCallBack = CallBack;
    //pressedTask = vex::thread(pressedCallBack);
}

bool Button::pressing() {
    switch (code)
        {
            case ButtonA:
                return ProxyController::playback[ProxyController::currentTime].ButtonA;

            case ButtonB:
                return ProxyController::playback[ProxyController::currentTime].ButtonB;

            case ButtonX:
                return ProxyController::playback[ProxyController::currentTime].ButtonX;

            case ButtonY:
                return ProxyController::playback[ProxyController::currentTime].ButtonY;

            case ButtonUp:
                return ProxyController::playback[ProxyController::currentTime].ButtonUp;

            case ButtonDown:
                return ProxyController::playback[ProxyController::currentTime].ButtonDown;

            case ButtonLeft:
                return ProxyController::playback[ProxyController::currentTime].ButtonLeft;

            case ButtonRight:
                return ProxyController::playback[ProxyController::currentTime].ButtonRight;

            case ButtonR1:
                return ProxyController::playback[ProxyController::currentTime].ButtonR1;

            case ButtonR2:
                return ProxyController::playback[ProxyController::currentTime].ButtonR2;

            case ButtonL1:
                return ProxyController::playback[ProxyController::currentTime].ButtonL1;

            case ButtonL2:
                return ProxyController::playback[ProxyController::currentTime].ButtonL2;
            
            default:
                return false; 
        }
}

bool Button::isRegistered(bool por) {
    if(por) {
        return (pressedCallBack == nullptr) ? false : true;
    } else {
        return (releasedCallBack == nullptr) ? false : true;
    }
}

bool Button::isRunning(bool por) {
    if(por) {
        if(!isRegistered(por)) return false;

    } else {
        if(!isRegistered(por)) return false;
    }
    return false;
}

PlaybackState getControllerStates() {
    PlaybackState temp;
    temp.Axis1 = vexControllerGet(kControllerMaster, V5_ControllerIndex::Axis1);
    temp.Axis2 = vexControllerGet(kControllerMaster, V5_ControllerIndex::Axis2);
    temp.Axis3 = vexControllerGet(kControllerMaster, V5_ControllerIndex::Axis1);
    temp.Axis4 = vexControllerGet(kControllerMaster, V5_ControllerIndex::Axis3);
    temp.ButtonA = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonA);
    temp.ButtonB = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonB);
    temp.ButtonX = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonX);
    temp.ButtonY = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonY);
    temp.ButtonUp = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonUp);
    temp.ButtonDown = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonDown);
    temp.ButtonLeft = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonLeft);
    temp.ButtonRight = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonRight);
    temp.ButtonL1 = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonL1);
    temp.ButtonL2 = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonL2);
    temp.ButtonR1 = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonR1);
    temp.ButtonR2 = vexControllerGet(kControllerMaster, V5_ControllerIndex::ButtonR2);
    return temp;
}

bool isPlaybackStateEmpty(PlaybackState state) {
    if(state.Axis1 == (signed char)-127) return true;
    return false;
}

  

int ProxyController::_workerFunction() {
    while(1) {
        vex::this_thread::sleep_for(10);
        switch (ProxyController::status)
        {
            case IDLE:
                vex::this_thread::sleep_for(10);
                break;
        
            case RECORD: {
                if(vexControllerConnectionStatusGet(kControllerMaster) == kV5ControllerOffline) continue;
                for(unsigned short i; i < MAXIMUM_PLAYBACKS; i+=quality) {
                    currentTime = i;
                    PlaybackState state = getControllerStates();
                    ProxyController::playback[i] = state;
                    vex::this_thread::sleep_for(quality);
                }
            }

            case PLAY: {

            }
        }
    }
    return 0;
}

ProxyControllerStatus ProxyController::status = IDLE;
vex::task ProxyController::worker = vex::task(ProxyController::_workerFunction);
std::string ProxyController::file = "";
unsigned char ProxyController::quality = 1;
unsigned short ProxyController::currentTime = 0;
PlaybackState ProxyController::playback[MAXIMUM_PLAYBACKS_WBUFFER] = {};

void ProxyController::Initalize(std::string filename) {
    file = filename;
    for(int i = 0; i < MAXIMUM_PLAYBACKS_WBUFFER; i++) {
        PlaybackState non;
        non.Axis1 = -127; //custom value to check if invalid. -127 is out of the domain of the controller axies.
        playback[i] = non;
    }
}

int ProxyController::recordAndWrite(double quality_) {
    if(vexControllerConnectionStatusGet(kControllerMaster) == kV5ControllerOffline) return 1;
    quality = 101-quality_;
    if(quality > 100 ) quality = 100;
    if(quality < 1) quality = 1;
    status = RECORD;
    return 0;
}

int ProxyController::play() {
    return 0;
}
