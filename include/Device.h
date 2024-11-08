#pragma once
#include "v5_api.h"
#include "vex.h"
#include <string>

struct DeviceInstall {
    std::string Name;
    bool isInstalled;
};

class Device {
    public:
        std::string name;
        int index;
        V5_DeviceT deviceref;

        //Constructors
        Device(std::string _name, int _index, V5_DeviceT _deviceref);
        Device(int _type, int _index, V5_DeviceT _deviceref);

        std::string toString();

    public:
        static std::string getDeviceTypeString(int type);
        static V5_DeviceT getInternalDevicePointer(vex::device device);
};