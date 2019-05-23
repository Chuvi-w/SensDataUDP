#pragma once
#include "EvCommon.h"

typedef enum IMUType_e
{
   TYPE_ACCELEROMETER = 1,
   TYPE_MAGNETIC_FIELD = 2,
   TYPE_GYROSCOPE = 4,
   TYPE_MAGNETIC_FIELD_UNCALIBRATED = 14,
   TYPE_GYROSCOPE_UNCALIBRATED = 16,
   TYPE_ACCELEROMETER_UNCALIBRATED = 35,
} IMUType_t;


class IIMUDevice: public std::enable_shared_from_this<IIMUDevice>
{
public:
   ~IIMUDevice()
   {

   }
   virtual uint32_t                        GetID() const = 0;
   virtual std::shared_ptr<IIMUDevice> GetShared() = 0;

};



template<IMUType_t IMUType>
class IIMUDeviceT: public IIMUDevice
{
public:
   ~IIMUDeviceT()
   {

   }

   virtual uint32_t GetID() const override
   {
      return IMUType;
   }

}