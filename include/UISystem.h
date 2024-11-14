#include "vex.h"
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
    static int renderLoop();
    static void toggleUI();
};

