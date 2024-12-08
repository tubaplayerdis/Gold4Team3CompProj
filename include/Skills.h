#include <vector>
#include <vex.h>

struct SkillsStartingPosition {
    int x;
    int y;
};

class Skills {
    private:
        static bool isActive;
        static int skillsPositionSelect;
        static vex::task skillsTask;
        static int _skillsFunc();
    public:

        static double x,y,h;

        static std::vector<SkillsStartingPosition> StartingPosistions;
        static bool isSkillsActive();
        static void deactiveSkills();
        static void runSkills(int positionIndex);
};