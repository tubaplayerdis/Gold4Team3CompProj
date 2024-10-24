#include "Device.h"
#include <string>
#include <sstream>

std::string to_string_ints(int x){
  std::stringstream s;
  s << x;
  return s.str();
}

Device::Device(std::string _name, int _index, V5_DeviceT _deviceref) {    name = _name;
  index = _index;
  deviceref = _deviceref;
}

std::string Device::toString() {
  return name + "Index: " + to_string_ints(index);
}

/*
  kDeviceTypeNoSensor        = 0,
  kDeviceTypeMotorSensor     = 2,
  kDeviceTypeLedSensor       = 3,
  kDeviceTypeAbsEncSensor    = 4,
  kDeviceTypeCrMotorSensor   = 5,
  kDeviceTypeImuSensor       = 6,
  kDeviceTypeRangeSensor     = 7, // obsolete
  kDeviceTypeDistanceSensor  = 7,
  kDeviceTypeRadioSensor     = 8,
  kDeviceTypeTetherSensor    = 9,
  kDeviceTypeBrainSensor     = 10,
  kDeviceTypeVisionSensor    = 11,
  kDeviceTypeAdiSensor       = 12,
  kDeviceTypeRes1Sensor      = 13,
  kDeviceTypeRes2Sensor      = 14,
  kDeviceTypeRes3Sensor      = 15,
  kDeviceTypeOpticalSensor   = 16,
  kDeviceTypeMagnetSensor    = 17,
  kDeviceTypeBumperSensor    = 0x40,
  kDeviceTypeGyroSensor      = 0x46,
  kDeviceTypeSonarSensor     = 0x47,
  kDeviceTypeGenericSensor   = 128,
  kDeviceTypeGenericSerial   = 129,
  kDeviceTypeUndefinedSensor = 255
*/
std::string Device::getDeviceTypeString(int type) {
    switch (type)
    {
        case 0:
        return std::string("kDeviceTypeNoSensor");
        case 1:
        return std::string("Unsupported");  // This case was not defined above; keep or modify as needed.
        case 2:
        return std::string("kDeviceTypeMotorSensor");
        case 3:
        return std::string("kDeviceTypeLedSensor");
        case 4:
        return std::string("kDeviceTypeAbsEncSensor");
        case 5:
        return std::string("kDeviceTypeCrMotorSensor");
        case 6:
        return std::string("kDeviceTypeImuSensor");
        case 7:
        return std::string("kDeviceTypeDistanceSensor");  // Note: kDeviceTypeRangeSensor is obsolete.
        case 8:
        return std::string("kDeviceTypeRadioSensor");
        case 9:
        return std::string("kDeviceTypeTetherSensor");
        case 10:
        return std::string("kDeviceTypeBrainSensor");
        case 11:
        return std::string("kDeviceTypeVisionSensor");
        case 12:
        return std::string("kDeviceTypeAdiSensor");
        case 13:
        return std::string("kDeviceTypeRes1Sensor");
        case 14:
        return std::string("kDeviceTypeRes2Sensor");
        case 15:
        return std::string("kDeviceTypeRes3Sensor");
        case 16:
        return std::string("kDeviceTypeOpticalSensor");
        case 17:
        return std::string("kDeviceTypeMagnetSensor");
        case 0x40:
        return std::string("kDeviceTypeBumperSensor");
        case 0x46:
        return std::string("kDeviceTypeGyroSensor");
        case 0x47:
        return std::string("kDeviceTypeSonarSensor");
        case 128:
        return std::string("kDeviceTypeGenericSensor");
        case 129:
        return std::string("kDeviceTypeGenericSerial");
        case 255:
        return std::string("kDeviceTypeUndefinedSensor");
        default:
        return std::string("Unsupported");
    }
}

V5_DeviceT Device::getInternalDevicePointer(vex::device device) {
  return vexDeviceGetByIndex(device.index());
}