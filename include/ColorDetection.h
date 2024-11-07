#pragma once
#include "vex.h"

/*
    OVERVIEW
    The values are tolerances for the blue and red rings. Colors can vary significantly due to brightness so the ranges are broad. Dosent matter tho cause only red and blue rings
    THE RED HAS EXCLUSION VALUES. What this means is that the range given is excluded instead of included. This is because the red and violet look similar and the color sensor dosent know the difference.
    Calculated as such:
        double GV = Bot::ColorSensor.hue();
        if(GV < BLUE_HUE_HIGH && GV > BLUE_HEU_LOW) {
            //Blue Ring
        } else if( GV < RED_HUE_EXCLUSION_LOW && GV > RED_HUE_EXCLUSION_HIGH) {
            //Red Ring
        }
*/
#define BLUE_HEU_LOW 85
#define BLUE_HUE_HIGH 271

#define RED_HUE_EXCLUSION_LOW 51
#define RED_HUE_EXCLUSION_HIGH 341


//For Throwing:
#define REVERSESECONDS 0.5//Seconds to move the conveyor in reverse at given velocity below.
#define REVERSEVELOCITY 600//(rpm because there is no percent option and 100% is 600 rpm)
#define FORWARDSECONDS 0.5//Seconds to keep moving the conveyor forward after going in reverse is done.

class ColorDetection {
    public:
        static int visionTask();
        static vex::controller::button ToggleButton;
        static void toggleEnabled();

    private:
        static bool isEnabled;

};