# STJS (Skills Task JSON System) Documentation

Created By: Aaron Wilk - https://github.com/tubaplayerdis, 38535B

## Overview

STJS uses json instead of literal source code to develop autonomus routines. The "Tasks" are stored on the micro sd card of the vex robotics v5 brain. Major advantages of this system include: 

1. No Need to Recompile
2. Data is stored Externally
3. Hot Swapping data during program execution
4. Quick Developement of autonomus routines

## Basics

STJS uses a standard format for tasks. An Example of this would look like:

```
{
    "tasks": [
        {
            "name": "Drive To 200, 200",
            "stype": "Driving",
            "destx": 200,
            "desty": 200,
            "desth": 30,
            "tp": 100,
            "vp": 100
        },
        {
            "name": "Drive Forward",
            "stype": "Segmenting",
            "segDist": 90,
            "vp": 100
        },
        {
            "name": "Turn 90 Degrees",
            "stype": "Turning",
            "turnBy": 90,
            "tp": 100
        },
        {
            "name": "Set mogo true",
            "stype": "Mogo",
            "setTo": true
        }
    ]
    
}
```

### Format

1. One array in the json file that is called tasks. (Name is important)
2. Objects in the array have no name

### Creating Objects

Objects have 2 very important data members.

1. name - Name of the Task. Displayed on the controller when running. Characters above index 17 wont be showed
2. stype - Type of the Task. Tells the engine to what values to use when interpreting

# stype - Driving

Use this stype when driving to a location. takes in coordinates and a heading value to turn to at the end. use -1 to ignore this behavior. Also takes in values tp and vp for turn velocity percentage and drive velocity percentage

### Stype Driving Specific Members

- destx - destination x value (number)
- desty - destination y value (number)
- desth - destination heading value (number)
- tp - turn velocity percentage (integer)
- vp - driving velocity percentage (integer)

### Stype Driving Example

```
{
    "name": "Drive To 200, 200",
    "stype": "Driving",
    "destx": 200,
    "desty": 200,
    "desth": 30,
    "tp": 100,
    "vp": 100
}
```
