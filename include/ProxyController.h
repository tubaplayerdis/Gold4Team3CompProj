#include "vex.h"
#include "fstream"
#include "string"
#include "vector"
#include "functional"

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
1% of memory usage of the c++ enviroment
*/


#define MAXIMUM_PLAYBACKS 65000
#define MAXIMUM_PLAYBACKS_WBUFFER 65000 + 500

#pragma pack(push, 1)
struct PlaybackStateEventReigsters {
    //event registers
    bool ButtonAPressed = false;
    bool ButtonAReleased = false;
    bool ButtonBPressed = false;
    bool ButtonBReleased = false;
    bool ButtonXPressed = false;
    bool ButtonXReleased = false;
    bool ButtonYPressed = false;
    bool ButtonYReleased = false;
    bool ButtonUpPressed = false;
    bool ButtonUpReleased = false;
    bool ButtonDownPressed = false;
    bool ButtonDownReleased = false;
    bool ButtonLeftPressed = false;
    bool ButtonLeftReleased = false;
    bool ButtonRightPressed = false;
    bool ButtonRightReleased = false;
    bool ButtonL1Pressed = false;
    bool ButtonL1Released = false;
    bool ButtonL2Pressed = false;
    bool ButtonL2Released = false;
    bool ButtonR1Pressed = false;
    bool ButtonR1Released = false;
    bool ButtonR2Pressed = false;
    bool ButtonR2Released = false;

    bool ButtonAxis1Changed = false;
    bool ButtonAxis2Changed = false;
    bool ButtonAxis3Changed = false;
    bool ButtonAxis4Changed = false;
    
    
};

struct PlaybackState {
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
    signed char Axis1 = 0; //-127 to 127 domain for unsigned chars
    signed char Axis2 = 0;
    signed char Axis3 = 0;
    signed char Axis4 = 0;

    //event registers
    PlaybackStateEventReigsters registers;

};
#pragma pack(pop)

enum ProxyControllerStatus {
    RECORD,
    PLAY
};

//None of these functions do nothing. they are meant to allow combatability beetween actual vex::controllers.
//I might have them do thier functions but not now.
class FScreen {
    public:
        FScreen();
        void     setCursor( int32_t row, int32_t col );
        int32_t  column();
        int32_t  row();
        template <class T>
        void     print( T value ) {}
        void     print( const char *format, ... );
        void     print( char *format, ... ); 
        void     clearScreen( void );
        void     clearLine( int number );
        void     clearLine( void );
        void     newLine( void );  
};

class Axis;
class ProxyController;

class Button {
    friend ProxyController;
    private:
        _V5_ControllerIndex code;
        void (* pressedCallBack)(void);
        void  (* releasedCallBack)(void);

        ProxyController* parentPointer;

    protected:
        //pressed is true and false is released
        bool isRegistered(bool por);

    public:
        Button(_V5_ControllerIndex keycode, ProxyController* parent);

        void pressed(void (* CallBack)(void));

        void released(void (* CallBack)(void));

        bool pressing();

};

class Axis {
    friend ProxyController;
    private:
        _V5_ControllerIndex code;
        void (* changedCallBack)(void);

        ProxyController* parentPointer;

    protected:
        bool isRegistered();

    public:
        Axis(_V5_ControllerIndex keycode, ProxyController* parent);

        void changed(void (* CallBack)(void));

        int position();

};

class ProxyController {
    friend Button;
    friend Axis;

    private:
        //static worker vars
        static ProxyControllerStatus sstatus;
        static PlaybackState* sarrp;
        static unsigned char squality;
        static ProxyController* splayPointer;


        static void _initWorker(ProxyControllerStatus status, PlaybackState *arrp, unsigned char quality, ProxyController* playPointer);
        static int _workerFunction();

        vex::task worker;
        std::string file;
        unsigned char quality; //real quality is reversed to simplify loop logic. so a value of 1 is highest quality and 100 is lowest
        PlaybackState playback[MAXIMUM_PLAYBACKS_WBUFFER]; //Add extra for saftey.
        unsigned short currentTime;

    public:
        FScreen Screen = FScreen();

        Axis Axis1 = Axis(_V5_ControllerIndex::Axis1, this);
        Axis Axis2 = Axis(_V5_ControllerIndex::Axis2, this);
        Axis Axis3 = Axis(_V5_ControllerIndex::Axis3, this);
        Axis Axis4 = Axis(_V5_ControllerIndex::Axis4, this);

        Button ButtonA = Button(_V5_ControllerIndex::ButtonA, this);
        Button ButtonB = Button(_V5_ControllerIndex::ButtonB, this);
        Button ButtonX = Button(_V5_ControllerIndex::ButtonX, this);
        Button ButtonY = Button(_V5_ControllerIndex::ButtonY, this);
        Button ButtonUp = Button(_V5_ControllerIndex::ButtonUp, this);
        Button ButtonDown = Button(_V5_ControllerIndex::ButtonDown, this);
        Button ButtonLeft = Button(_V5_ControllerIndex::ButtonLeft, this);
        Button ButtonRight = Button(_V5_ControllerIndex::ButtonRight, this);
        Button ButtonL1 = Button(_V5_ControllerIndex::ButtonL1, this);
        Button ButtonL2 = Button(_V5_ControllerIndex::ButtonL2, this);
        Button ButtonR1 = Button(_V5_ControllerIndex::ButtonR1, this);
        Button ButtonR2 = Button(_V5_ControllerIndex::ButtonR2, this);


        ProxyController(std::string filename);

        int recordAndWrite(double quality);//0-100 for quality. will be converted to real quality before flipping to record.
        int play(/*add a v5 controller to imitate?*/);
        void rumble(const char* pattern);

};


