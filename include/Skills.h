#include <vector>
#include <vex.h>


struct SkillsTask {
    double destx;
    double desty;
    double desth;
    const char name[17];
    bool doDrive;
    bool doSpecial;
    bool toggleMogo;
    int (*special)(int); //function pointer that takes an int as input and output. used for a special implementation for the task.
    bool isComplete = false;
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