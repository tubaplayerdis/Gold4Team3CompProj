#pragma once
#include "v5_api.h"
#include <string>


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
};

typedef enum {
    None = 0 //Complete More and add method to convert.
} DeviceType;