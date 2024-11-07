#include "ColorDetection.h"
#include "Bot.h"

vex::controller::button ColorDetection::ToggleButton = Bot::Controller.ButtonLeft;
bool ColorDetection::isEnabled = false;

int ColorDetection::visionTask() {
    while(true) 
    {
        double brightness = Bot::ColorSensor.brightness(false);
        double hue = Bot::ColorSensor.hue();
        Bot::Brain.Screen.printAt(130,0, "hue: %f brightness: %f", hue, brightness);


        double GV = Bot::ColorSensor.hue();
        if(GV < BLUE_HUE_HIGH && GV > BLUE_HEU_LOW) {
            //Blue Ring
            Bot::Brain.Screen.printAt(0,190, "Blue Ring Detected!");
            if(Bot::Aliance == aliance::Nuetral) continue;
            if(!isEnabled) continue;
            //Blue Ring Detected
            if(Bot::Aliance != aliance::Blue) {
                vex::this_thread::sleep_for(500);
                /*
                    IMPL of abortion mechanism throw
                */

                if(Bot::Controller.ButtonL2.pressing()) {
                    Bot::Intake.setVelocity(200, vex::rpm);
                    Bot::Intake.spin(vex::reverse);
                } else if(Bot::Controller.ButtonR2.pressing()) {
                    Bot::Intake.setVelocity(200, vex::rpm);
                    Bot::Intake.spin(vex::forward);
                }
                //Bot::Conveyor.stop();
            }
        } else if( GV < RED_HUE_EXCLUSION_LOW && GV > RED_HUE_EXCLUSION_HIGH) {
            //Red Ring
            Bot::Brain.Screen.printAt(0,190, "Red Ring Detected!");
            if(Bot::Aliance == aliance::Nuetral) continue;
            if(!isEnabled) continue;
            //Red Ring Detected
            if(Bot::Aliance != aliance::Red) {
                vex::this_thread::sleep_for(500);
                /*
                    IMPL of abortion mechanism throw
                */

                if(Bot::Controller.ButtonL2.pressing()) {
                    Bot::Intake.setVelocity(200, vex::rpm);
                    Bot::Intake.spin(vex::reverse);
                } else if(Bot::Controller.ButtonR2.pressing()) {
                    Bot::Intake.setVelocity(200, vex::rpm);
                    Bot::Intake.spin(vex::forward);
                }
                //Bot::Conveyor.stop();
            }
        }
        
    }
    return 0;
}


void ColorDetection::toggleEnabled() {
    isEnabled = !isEnabled;
}