#include "ColorDetection.h"
#include "Bot.h"

bool ColorDetection::isEnabled = false;

int ColorDetection::visionTask() {
    while(true) 
    {
        if(!Bot::ColorSensor.installed()) isEnabled = false;

        double brightness = Bot::ColorSensor.brightness(false);
        double hue = Bot::ColorSensor.hue();
        //Bot::Brain.Screen.printAt(130,0, "hue: %f brightness: %f", hue, brightness);

        double GV = Bot::ColorSensor.hue();
        if(GV < BLUE_HUE_HIGH && GV > BLUE_HEU_LOW) {
            //Blue Ring
            //Bot::Brain.Screen.printAt(0,190, "Blue Ring Detected!");
            if(!isEnabled) continue;
            //Blue Ring Detected
            if(Bot::Aliance != aliance::Blue) {
                Bot::IgnoreIntake = true;
                Bot::Intake.stop();
                vex::this_thread::sleep_for(50);
                Bot::IgnoreIntake = false;
            }
        } else if( GV < RED_HUE_EXCLUSION_LOW && GV > RED_HUE_EXCLUSION_HIGH) {
            //Red Ring
            //Bot::Brain.Screen.printAt(0,190, "Red Ring Detected!");
            if(!isEnabled) continue;
            //Red Ring Detected
            if(Bot::Aliance != aliance::Red) {
                Bot::IgnoreIntake = true;
                Bot::Intake.stop();
                vex::this_thread::sleep_for(50);
                Bot::IgnoreIntake = false;
            }
        }
        
    }
    return 0;
}


void ColorDetection::toggleEnabled() {
    isEnabled = !isEnabled;
}