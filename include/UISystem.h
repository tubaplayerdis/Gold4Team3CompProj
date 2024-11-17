#include "vex.h"
#include "api/vexui.h"
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

        //Functions and list
        static void setup();
        static int renderLoop();
        static void toggleUI();
};

