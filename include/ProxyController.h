#include "vex.h"
#include "fstream"
#include "string"
#include "vector"

/*
MEMORY CALCULATIONS.

1mb = 10,00,000 bytes

Playback State struct memory usage per: 
12 bits for bools
32 bits for axis
44 bits
11 bytes

MAXIMUM SIZE OF PLAYBACKS: 65,000

65,000 * 11 = 715,000 bytes 

std::string is about 32 bytes

PLAYBACK SYSTEM MEMORY FOOTPRINT:
Class and metadata: 100 bytes (overestimate)
playback - 260,000 bytes 

Total: 715,100 bytes
----------------------------------------------------------
TOTAL IN MB: 0.7101mb ~~ 0.8 MB.
----------------------------------------------------------
That is infact, memory efficency 
*/


#define MAXIMUM_PLAYBACKS 65000
#define MAXIMUM_PLAYBACKS_WBUFFER 65000 + 500

enum ButtonEventType : bool {
    PRESSED,
    RELEASED
};

struct PlaybackState { // 20 bytes
    unsigned short time; //Skills matches are only 60 seconds (60,000) milliseconds long. 65,535 is the maximum value of a unsigned short, therefore the therorietical limit is 65.535 seconds of playback. Will be capped at 65.
    bool ButtonA = false;
    bool ButtonB = false;
    bool ButtonX = false;
    bool ButtonY = false;
    bool ButtonUp = false;
    bool ButtonDown = false;
    bool ButtonLeft = false;
    bool ButtonRight = false;
    bool ButtonL1 = false;
    bool ButtonL2 = false;
    bool ButtonR1 = false;
    bool ButtonR2 = false;
    signed char Axis1 = 0;
    signed char Axis2 = 0;
    signed char Axis3 = 0;
    signed char Axis4 = 0;
};

enum ProxyControllerStatus {
    IDLE,
    RECORD,
    PLAY
};

class Button {
    private:
        _V5_ControllerIndex code;
        void (* pressedCallBack)(void);
        void  (* releasedCallBack)(void);
        vex::thread pressedTask;
        vex::thread releasedTask;

    protected:
        bool isRegistered(bool por);
        bool isRunning(bool por);

    public:
        Button(_V5_ControllerIndex keycode);

        void pressed(void (* CallBack)(void));

        void released(void (* CallBack)(void));

        bool pressing();

};

class ProxyController {
    friend Button;

    private:
        static ProxyControllerStatus status;
        static int _workerFunction();

        static vex::task worker;
        static std::string file;
        static unsigned char quality; //real quality is reversed to simplify loop logic. so a value of 1 is highest quality and 100 is lowest
        static PlaybackState playback[MAXIMUM_PLAYBACKS_WBUFFER]; //Add extra for saftey.
        static unsigned short currentTime;

    public:
        static void Initalize(std::string filename);

        static int recordAndWrite(double quality);//0-100 for quality. will be converted to real quality before flipping to record.
        static int play(/*add a v5 controller to imitate?*/);


        

};


