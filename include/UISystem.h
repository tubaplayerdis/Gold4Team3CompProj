#include "vex.h"
#include "api/vexui.h"
#include <string>
#include "Bot.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Drivetrain.h"
#include "Notifications.h"

class UISystem
{
    private:
        static bool doRender;
    public:
        //Starting Positions
        static vexui::StartingPosition Blue_Left;
        static vexui::StartingPosition Blue_Right;
        static vexui::StartingPosition Red_Left;
        static vexui::StartingPosition Red_Right;
        static vexui::StartingPosition Center;

        static std::vector<vexui::StartingPosition> positions;
        static int SelectedPosition;


        //callbacks
        static void mainTabButton_Press();
        static void odometryTabButton_Press();
        static void consoleTabButton_Press();
        //UI Elements
        static vexui::Button mainTabButton;
        static vexui::Button odometryTabButton;
        static vexui::Button consoleTabButton;

        static vexui::Panel mainPanel;
        static vexui::Panel odometryPanel;
        static vexui::Panel consolePanel;

        static vexui::Label watermark;

        //Main Panel Elements
        static vexui::Label labm;
        static vexui::Label redRingNumLabel;
        static vexui::Label blueRingNumLabel;
        static vexui::Label MobileGoalNumLabel;

        //Odometry Panel Elements
        static vexui::Label labo;
        static vexui::OdometryMap odoMap;
        static vexui::Label calibrationSelectLabel;
        static vexui::Button calibrationPositionBackButton;
        static vexui::Button calibrationPositionForwardButton;
        static vexui::Label calibrationWarningLabel;

        //Console Panel Elements
        static vexui::Label labc;

        //Functions and list
        static void setup();
        static int renderLoop();
        static void toggleUI();
};

