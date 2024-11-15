#include "vex.h"
#include "Bot.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Drivetrain.h"
#include "Notifications.h"
#include "api/vexui.h"


class UISystem
{
    private:
        static bool doRender;

        //callbacks
        static void mainTabButton_Press();
        static void odometryTabButton_Press();
        static void consoleTabButton_Press();
    public:
        //UI Elements
        static vexui::Button mainTabButton;
        static vexui::Button odometryTabButton;
        static vexui::Button consoleTabButton;

        static vexui::Panel mainPanel;
        static vexui::Panel odometryPanel;
        static vexui::Panel consolePanel;

        //Functions and list
        static void setup();
        static int renderLoop();
        static void toggleUI();
};

