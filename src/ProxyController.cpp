#include "ProxyController.h"
#include "vex.h"
#include "fstream"
#include "string"
#include "vector"

int ProxyController::_workerFunction() {
    while(1) {
        switch (ProxyController::status)
        {
            case IDLE:
                vex::this_thread::sleep_for(10);
                break;
        
            case RECORD: {
                for(unsigned short i; i < MAXIMUM_PLAYBACKS; i++) {
                    PlaybackState temp;
                    //For some reason I though only storing if 1 button press at a time was okay.
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

void ProxyController::Initalize(std::string filename) {
    file = filename;
}

int ProxyController::recordAndWrite(double quality) {
    for(unsigned short i; i < MAXIMUM_PLAYBACKS; i++) {

    }
}
