#include "ProxyController.h"
#include "vex.h"
#include "fstream"
#include "istream"
#include "string"
#include "vector"
#include "functional"

FScreen::FScreen() {
    return;
}

void FScreen::setCursor( int32_t row, int32_t col ) { return; }
int32_t  FScreen::column() { return -1; }
int32_t  FScreen::row() { return -1; }

//template <class T>
//void  FScreen::print( T value ) { return; }
void  FScreen::print( const char *format, ... ) { return; }
void  FScreen::print( char *format, ... ) { return; }
void  FScreen::clearScreen( void ) { return; }
void  FScreen::clearLine( int number ) { return; }
void  FScreen::clearLine( void ) { return; }
void  FScreen::newLine( void ) { return; } 



Button::Button(_V5_ControllerIndex keycode, ProxyController* parent) {
    code = keycode;
    pressedCallBack = nullptr;
    releasedCallBack = nullptr;
    parentPointer = parent;

}

void Button::pressed(void (* CallBack)(void)) {
    pressedCallBack = CallBack;
}

void Button::released(void (* CallBack)(void)) {
    releasedCallBack = CallBack;
}

bool Button::pressing() {
    switch (code)
        {
            case ButtonA:
                return parentPointer->playback[parentPointer->currentTime].ButtonA;

            case ButtonB:
                return parentPointer->playback[parentPointer->currentTime].ButtonB;

            case ButtonX:
                return parentPointer->playback[parentPointer->currentTime].ButtonX;

            case ButtonY:
                return parentPointer->playback[parentPointer->currentTime].ButtonY;

            case ButtonUp:
                return parentPointer->playback[parentPointer->currentTime].ButtonUp;

            case ButtonDown:
                return parentPointer->playback[parentPointer->currentTime].ButtonDown;

            case ButtonLeft:
                return parentPointer->playback[parentPointer->currentTime].ButtonLeft;

            case ButtonRight:
                return parentPointer->playback[parentPointer->currentTime].ButtonRight;

            case ButtonR1:
                return parentPointer->playback[parentPointer->currentTime].ButtonR1;

            case ButtonR2:
                return parentPointer->playback[parentPointer->currentTime].ButtonR2;

            case ButtonL1:
                return parentPointer->playback[parentPointer->currentTime].ButtonL1;

            case ButtonL2:
                return parentPointer->playback[parentPointer->currentTime].ButtonL2;
            
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

Axis::Axis(_V5_ControllerIndex keycode, ProxyController* parent) {
    code = keycode;
    changedCallBack = nullptr;
    parentPointer = parent;
}

bool Axis::isRegistered() {
    return (changedCallBack == nullptr) ? false : true;
}

void Axis::changed(void (* CallBack)(void)) {
    changedCallBack = CallBack;
}

int Axis::position() {
    switch (code)
    {
        case Axis1:
            return parentPointer->playback[parentPointer->currentTime].Axis1;

        case Axis2:
            return parentPointer->playback[parentPointer->currentTime].Axis2;

        case Axis3:
            return parentPointer->playback[parentPointer->currentTime].Axis3;

        case Axis4:
            return parentPointer->playback[parentPointer->currentTime].Axis4;

        default:
            return false;
    }
}

PlaybackState getControllerStates(unsigned short time) {
    PlaybackState temp;
    temp.time = time;
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

void ProxyController::_initWorker(ProxyControllerStatus status, PlaybackState *arrp, unsigned char quality, ProxyController* playPointer) {
    sstatus = status;
    sarrp = arrp;
    squality = quality;
    splayPointer = playPointer;
}

ProxyControllerStatus ProxyController::sstatus = RECORD;
PlaybackState* ProxyController::sarrp = nullptr;
unsigned char ProxyController::squality = 1;
ProxyController* ProxyController::splayPointer = nullptr;


int ProxyController::_workerFunction() {
    ProxyControllerStatus status = sstatus;
    PlaybackState *arrp = sarrp;
    unsigned char quality = squality;
    ProxyController* playPointer = splayPointer;


    switch (status)
    {
        case RECORD: {
            if(vexControllerConnectionStatusGet(kControllerMaster) == kV5ControllerOffline) return 1;
            for(unsigned short i = 0; i < MAXIMUM_PLAYBACKS; i+=quality) {
                PlaybackState state = getControllerStates(i);
                arrp[i] = state;
                vex::this_thread::sleep_for(quality);
            }
        }

        case PLAY: {
            for(unsigned short i = 0; i < MAXIMUM_PLAYBACKS; i+=quality) {
                playPointer->currentTime = i;
                PlaybackStateEventReigsters reg = arrp[i].registers;

                if(reg.ButtonAxis1Changed && playPointer->Axis1.isRegistered()) vex::thread(playPointer->Axis1.changedCallBack);
                if(reg.ButtonAxis2Changed && playPointer->Axis2.isRegistered()) vex::thread(playPointer->Axis2.changedCallBack);
                if(reg.ButtonAxis3Changed && playPointer->Axis3.isRegistered()) vex::thread(playPointer->Axis3.changedCallBack);
                if(reg.ButtonAxis4Changed && playPointer->Axis4.isRegistered()) vex::thread(playPointer->Axis4.changedCallBack);

                if(reg.ButtonAPressed && playPointer->ButtonA.isRegistered(true) ) vex::thread(playPointer->ButtonA.pressedCallBack);
                if(reg.ButtonAPressed && playPointer->ButtonA.isRegistered(true) ) vex::thread(playPointer->ButtonA.pressedCallBack);

                if(reg.ButtonBPressed && playPointer->ButtonB.isRegistered(true) ) vex::thread(playPointer->ButtonB.pressedCallBack);
                if(reg.ButtonBPressed && playPointer->ButtonB.isRegistered(true) ) vex::thread(playPointer->ButtonB.pressedCallBack);

                if(reg.ButtonXPressed && playPointer->ButtonX.isRegistered(true) ) vex::thread(playPointer->ButtonX.pressedCallBack);
                if(reg.ButtonXPressed && playPointer->ButtonX.isRegistered(true) ) vex::thread(playPointer->ButtonX.pressedCallBack);

                if(reg.ButtonYPressed && playPointer->ButtonY.isRegistered(true) ) vex::thread(playPointer->ButtonY.pressedCallBack);
                if(reg.ButtonYPressed && playPointer->ButtonY.isRegistered(true) ) vex::thread(playPointer->ButtonY.pressedCallBack);

                if(reg.ButtonUpPressed && playPointer->ButtonUp.isRegistered(true) ) vex::thread(playPointer->ButtonUp.pressedCallBack);
                if(reg.ButtonUpPressed && playPointer->ButtonUp.isRegistered(true) ) vex::thread(playPointer->ButtonUp.pressedCallBack);

                if(reg.ButtonDownPressed && playPointer->ButtonDown.isRegistered(true) ) vex::thread(playPointer->ButtonDown.pressedCallBack);
                if(reg.ButtonDownPressed && playPointer->ButtonDown.isRegistered(true) ) vex::thread(playPointer->ButtonDown.pressedCallBack);

                if(reg.ButtonLeftPressed && playPointer->ButtonLeft.isRegistered(true) ) vex::thread(playPointer->ButtonLeft.pressedCallBack);
                if(reg.ButtonLeftPressed && playPointer->ButtonLeft.isRegistered(true) ) vex::thread(playPointer->ButtonLeft.pressedCallBack);

                if(reg.ButtonRightPressed && playPointer->ButtonRight.isRegistered(true) ) vex::thread(playPointer->ButtonRight.pressedCallBack);
                if(reg.ButtonRightPressed && playPointer->ButtonRight .isRegistered(true) ) vex::thread(playPointer->ButtonRight.pressedCallBack);

                if(reg.ButtonL1Pressed && playPointer->ButtonL1.isRegistered(true) ) vex::thread(playPointer->ButtonL1.pressedCallBack);
                if(reg.ButtonL1Pressed && playPointer->ButtonL1.isRegistered(true) ) vex::thread(playPointer->ButtonL1.pressedCallBack);

                if(reg.ButtonL2Pressed && playPointer->ButtonL2.isRegistered(true) ) vex::thread(playPointer->ButtonL2.pressedCallBack);
                if(reg.ButtonL2Pressed && playPointer->ButtonL2.isRegistered(true) ) vex::thread(playPointer->ButtonL2.pressedCallBack);

                if(reg.ButtonR1Pressed && playPointer->ButtonR1.isRegistered(true) ) vex::thread(playPointer->ButtonR1.pressedCallBack);
                if(reg.ButtonR1Pressed && playPointer->ButtonR1.isRegistered(true) ) vex::thread(playPointer->ButtonR1.pressedCallBack);

                if(reg.ButtonR2Pressed && playPointer->ButtonR2.isRegistered(true) ) vex::thread(playPointer->ButtonR2.pressedCallBack);
                if(reg.ButtonR2Pressed && playPointer->ButtonR2.isRegistered(true) ) vex::thread(playPointer->ButtonR2.pressedCallBack);

                vex::this_thread::sleep_for(quality);
            }
            return 0;
        }
    }

    //Static anylsis after recording finishes
    for(unsigned short i = 0; i < MAXIMUM_PLAYBACKS; i+=quality) { 
        if(i != 0) continue;

        if(arrp[i].Axis1 != arrp[i-1].Axis1) arrp[i].registers.ButtonAxis1Changed = true;
        if(arrp[i].Axis2 != arrp[i-1].Axis2) arrp[i].registers.ButtonAxis2Changed = true;
        if(arrp[i].Axis3 != arrp[i-1].Axis3) arrp[i].registers.ButtonAxis3Changed = true;
        if(arrp[i].Axis4 != arrp[i-1].Axis4) arrp[i].registers.ButtonAxis4Changed = true;

        if (!arrp[i - 1].ButtonA && arrp[i].ButtonA) arrp[i].registers.ButtonAPressed = true;
        else if (arrp[i - 1].ButtonA && !arrp[i].ButtonA) arrp[i].registers.ButtonAReleased = true;

        if (!arrp[i - 1].ButtonB && arrp[i].ButtonB) arrp[i].registers.ButtonBPressed = true;
        else if (arrp[i - 1].ButtonB && !arrp[i].ButtonB) arrp[i].registers.ButtonBReleased = true;

        if (!arrp[i - 1].ButtonX && arrp[i].ButtonX) arrp[i].registers.ButtonXPressed = true;
        else if (arrp[i - 1].ButtonX && !arrp[i].ButtonX) arrp[i].registers.ButtonXReleased = true;

        if (!arrp[i - 1].ButtonY && arrp[i].ButtonY) arrp[i].registers.ButtonYPressed = true;
        else if (arrp[i - 1].ButtonY && !arrp[i].ButtonY) arrp[i].registers.ButtonYReleased = true;

        if (!arrp[i - 1].ButtonUp && arrp[i].ButtonUp) arrp[i].registers.ButtonUpPressed = true;
        else if (arrp[i - 1].ButtonUp && !arrp[i].ButtonUp) arrp[i].registers.ButtonUpReleased = true;

        if (!arrp[i - 1].ButtonDown && arrp[i].ButtonDown) arrp[i].registers.ButtonDownPressed = true;
        else if (arrp[i - 1].ButtonDown && !arrp[i].ButtonDown) arrp[i].registers.ButtonDownReleased = true;

        if (!arrp[i - 1].ButtonLeft && arrp[i].ButtonLeft) arrp[i].registers.ButtonLeftPressed = true;
        else if (arrp[i - 1].ButtonLeft && !arrp[i].ButtonLeft) arrp[i].registers.ButtonLeftReleased = true;

        if (!arrp[i - 1].ButtonRight && arrp[i].ButtonRight) arrp[i].registers.ButtonRightPressed = true;
        else if (arrp[i - 1].ButtonRight && !arrp[i].ButtonRight) arrp[i].registers.ButtonRightReleased = true;

        if (!arrp[i - 1].ButtonL1 && arrp[i].ButtonL1) arrp[i].registers.ButtonL1Pressed = true;
        else if (arrp[i - 1].ButtonL1 && !arrp[i].ButtonL1) arrp[i].registers.ButtonL1Released = true;

        if (!arrp[i - 1].ButtonL2 && arrp[i].ButtonL2) arrp[i].registers.ButtonL2Pressed = true;
        else if (arrp[i - 1].ButtonL2 && !arrp[i].ButtonL2) arrp[i].registers.ButtonL2Released = true;

        if (!arrp[i - 1].ButtonR1 && arrp[i].ButtonR1) arrp[i].registers.ButtonR1Pressed = true;
        else if (arrp[i - 1].ButtonR1 && !arrp[i].ButtonR1) arrp[i].registers.ButtonR1Released = true;

        if (!arrp[i - 1].ButtonR2 && arrp[i].ButtonR2) arrp[i].registers.ButtonR2Pressed = true;
        else if (arrp[i - 1].ButtonR2 && !arrp[i].ButtonR2) arrp[i].registers.ButtonR2Released = true;
    }


    //Write to file
    std::ofstream file(playPointer->file, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&arrp), sizeof(PlaybackState));
        file.close();
    } else return 1;

    return 0;
}


ProxyController::ProxyController(std::string filename) {
    file = "";
    quality = 1;
    currentTime = 0;
    playback[MAXIMUM_PLAYBACKS_WBUFFER-1] = {};
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

    ProxyController::_initWorker(RECORD, playback, quality, this);

    vex::task work(ProxyController::_workerFunction);
    
    return 0;
}

int ProxyController::play() {
    std::ifstream filep(file, std::ios::binary);
    if (filep.is_open()) {
        filep.read(reinterpret_cast<char*>(&playback), sizeof(PlaybackState));
        filep.close(); 
    }

    ProxyController::_initWorker(PLAY, playback, 1, this);

    vex::task work(ProxyController::_workerFunction);

    return 0;
}

//Ghost function for consistency;
void ProxyController::rumble(const char* pattern) {
    return;
}
