#pragma once
#include "v5_api.h"
#include <string>


class Device {
    public:
        std::string name;
        int index;
        V5_DeviceT deviceref;
        Device(std::string _name, int _index, V5_DeviceT _deviceref);
};
