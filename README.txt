todo:
    Implement Throwing mechanism of color detection;
    
CONTROLS:
    Joystick Left - Forward/backward
    Joystick Right - Left/Right
    L2 - Intake In
    R2 - Intake Out
    A - Clamp Down
    B - Clamp up
    Y - Cycle Aliiance
    X - Drive to nearest wall stake
    Up - Arm Up
    Down - Arm Down
    Left - Toggle Color Sort



IMPORTANT(tldr):

This project requires the vex robotics extension for vs code to function.
This project uses irregular naming conventions due to vex's nameing conventions.
This projects structure is irregular to a standard c++ project.

NAMING CONVENTIONS:

1. All classes and files use uppercase for the first letter unless rule 4.
2. Public varaiables have uppercase first letters and lowercase letters after except if rule 4.
3. Private varaibles use underscores _ before the varaible name and are all lowercase except if rule 4.
4. Varaible and Class abbreviations have all uppercase letters.
5. Classes and files share names. struct's do not need to follow this rule

FILE STRUCTURE:

1. Source files(cpp) are placed under the src directory.
2. Header files(h or hpp) are placed under the include directory.

PROGRAMMING CONVENTIONS:

1. Avoid namespace pollution. Ie: include namespace std; , include namespace vex;. use of vex namepsace allowed only in the main.cpp file.
2. Comprehensive variable names, arrbreviations need a comment with full form.
3. Avoid use of extern varaibles and instead use static classes for strucutre. This allows for better comprehension
4. Comments are useful, but not required.

BASIC UNDERSTANDING:

1. All electronics are under Bot.
2. Drivetrain is well, drivetrain code.
