#include "vex.h"
#include "fstream"
#include "string"
#include "vector"

/*
MEMORY CALCULATIONS.

1mb = 10,00,000 bytes

Playback State struct memory usage per: 
2 bytes - time
1 byte - button enum
1 byte - representation of value signed char
4 bytes total.

MAXIMUM SIZE OF PLAYBACKS: 65,000

65,000 * 4 = 260,000 bytes 

std::string is about 32 bytes

PLAYBACK SYSTEM MEMORY FOOTPRINT:
Class and metadata: 100 bytes (overestimate)
playback - 260,000 bytes 

Total: 260,100 bytes
----------------------------------------------------------
TOTAL IN MB: 0.2601mb ~~ 0.3 MB.
----------------------------------------------------------
That is infact, memory efficency 
*/


#define MAXIMUM_PLAYBACKS 65000

enum ButtonTypes : unsigned char {
    Axis1,
    Axis2,
    Axis3,
    Axis4,
    ButtonA,
    ButtonB,
    ButtonX,
    ButtonY,
    ButtonUp,
    ButtonDown,
    ButtonLeft,
    ButtonRight,
    ButtonL1,
    ButtonL2,
    ButtonR1,
    ButtonR2
};

struct PlaybackState {
    unsigned short time; //Skills matches are only 60 seconds (60,000) milliseconds long. 65,535 is the maximum value of a unsigned short, therefore the therorietical limit is 65.535 seconds of playback. Will be capped at 65.
    ButtonTypes btype;
    signed char rep; //0-1 if interpreted as a bool and -100 to 100 if Axis Playback
};

enum ProxyControllerStatus {
    IDLE,
    RECORD,
    PLAY
};

class ProxyController {
    private:
        static ProxyControllerStatus status;
        static int _workerFunction();

        static vex::task worker;
        static std::string file;
        static PlaybackState playback[MAXIMUM_PLAYBACKS + 500]; //Add extra for saftey.

    public:
        static void Initalize(std::string filename);

        static int recordAndWrite(double quality);
        static int play(/*add a v5 controller to imitate?*/);
};
