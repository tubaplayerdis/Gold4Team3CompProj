#include <vector>
#include <vex.h>

enum SkillsTaskType {
    Driving = 0,
    Segmenting = 1,
    Turning = 2,
    Mogo = 3,
    Arm = 4,
    EndGame = 5,
    Error = 6
};


struct SkillsTask {
    std::string name;
    SkillsTaskType stype = SkillsTaskType::Driving;

    //ALL VALUES OF DISTANCE ARE MM, ALL VALUES OF TURNING ARE DEGREES.

    //For Driving, Segmenting and Turning
    int tp = 100;
    int vp = 100;

    //If Driving.
    double destx = -1;
    double desty = -1;
    double desth = -1;

    //If Segmenting
    double segAmount = -1;

    //If Only Turning
    double turnBy = -1;

    //If Actuating Mogo
    bool toggleMogoTo = false;

    //If Actuating Arm
    double turnArmTo = -1;

    //Actuate Endgame. To Use It Drive Forward OR BackWard
    bool togglePTUTo;

    bool isComplete = false;
};

class SkillsEngine {
    public:
        static void init(std::vector<SkillsTask> _tasks);

        //Main Functions
        static void execute();
        static void pause();
        static void restart();

        //Helpers
        static void addTask(SkillsTask task);
        static int currentTaskIndex();
        static SkillsTask currentTask();

        static std::vector<SkillsTask> tasks;
    private:
        static int onTaskIndex;
        static int _worker();
        static vex::task executor;
};

class Skills {
    public:
        static bool isActive;
        static double x,y,h;
        static double ix,iy;

        static bool isSkillsActive();
        static void deactiveSkills();
        static void runSkills(int positionIndex);
};