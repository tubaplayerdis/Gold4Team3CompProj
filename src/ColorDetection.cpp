#include "ColorDetection.h"
#include "Bot.h"

bool ColorDetection::isEnabled = false;

int ColorDetection::visionTask() {
    Bot::ColorSensor.setLightPower(50, vex::percent);
    while(true)
    {
        if(!Bot::ColorSensor.installed()) isEnabled = false;

        double brightness = Bot::ColorSensor.brightness(false);
        double hue = Bot::ColorSensor.hue();
        //Bot::Brain.Screen.printAt(130,0, "hue: %f brightness: %f", hue, brightness);

        double GV = Bot::ColorSensor.hue();
        if(GV < BLUE_HUE_HIGH && GV > BLUE_HEU_LOW && Bot::DistanceC.objectDistance(vex::mm) < 30) {
            //Blue Ring
            //Bot::Brain.Screen.printAt(0,190, "Blue Ring Detected!");
            if(!isEnabled) continue;
            //Blue Ring Detected
            if(Bot::Aliance != aliance::Blue) {
                vex::this_thread::sleep_for(80);
                Bot::IgnoreIntake = true;
                Bot::Conveyor.stop();
                Bot::Conveyor.spinFor(vex::reverse, 0.1, vex::seconds);
                Bot::Conveyor.stop();
                Bot::Conveyor.spin(vex::forward);
                vex::this_thread::sleep_for(80);
                if(Bot::Comp.isAutonomous()) {
                    Bot::Conveyor.setMaxTorque(100, vex::percent);
                    Bot::Conveyor.setVelocity(100, vex::percent);
                    Bot::Conveyor.spin(vex::forward);
                }
                Bot::IgnoreIntake = false;
                //vex::this_thread::sleep_for(200);
            }
        } else if( GV < RED_HUE_HIGH && GV > RED_HUE_LOW && Bot::DistanceC.objectDistance(vex::mm) < 30) {
            //Red Ring
            //Bot::Brain.Screen.printAt(0,190, "Red Ring Detected!");
            if(!isEnabled) continue;
            //Red Ring Detected
            if(Bot::Aliance != aliance::Red) {
                vex::this_thread::sleep_for(80);
                Bot::IgnoreIntake = true;
                Bot::Conveyor.stop();
                Bot::Conveyor.spinFor(vex::reverse, 0.1, vex::seconds);
                Bot::Conveyor.stop();
                Bot::Conveyor.spin(vex::forward);
                vex::this_thread::sleep_for(80);
                if(Bot::Comp.isAutonomous()) {
                    Bot::Conveyor.setMaxTorque(100, vex::percent);
                    Bot::Conveyor.setVelocity(100, vex::percent);
                    Bot::Conveyor.spin(vex::forward);
                }
                Bot::IgnoreIntake = false;
                //vex::this_thread::sleep_for(200);
            }
        }
        
    }
    return 0;
}


void ColorDetection::toggleEnabled() {
    isEnabled = !isEnabled;
}