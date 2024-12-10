#include <vector>
#include <vex.h>

enum SkillsTaskType {
    Driving = 0,
    Segmenting = 1,
    Turning = 2,
    Mogo = 3,
    Arm = 4,
    EngGame = 5
};


struct SkillsTask {
    const char name[17];
    SkillsTaskType stype = SkillsTaskType::Driving;

    //ALL VALUES OF DISTANCE ARE MM, ALL VALUES OF TURNING ARE DEGREES.

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

    //Actuate Endgame
    double spinMotorsDegrees;

    bool isComplete = false;
};

class SkillsEngine {
    public:
        static int onTaskIndex;
        static void addTask(SkillsTask task);
        static SkillsTask currentTask();
};

class Skills {
    private:
        static int skillsPositionSelect;
        static vex::task skillsTask;
        static int _skillsFunc();
    public:
        static std::vector<SkillsTask> Tasks;
        static bool isActive;
        static double x,y,h;

        static bool isSkillsActive();
        static void deactiveSkills();
        static void runSkills(int positionIndex);
};