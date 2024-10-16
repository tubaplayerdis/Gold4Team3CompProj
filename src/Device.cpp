#include "Device.h"

Device::Device(std::string _name, int _index, V5_DeviceT _deviceref) {
    name = _name;
    index = _index;
    deviceref = _deviceref;
}