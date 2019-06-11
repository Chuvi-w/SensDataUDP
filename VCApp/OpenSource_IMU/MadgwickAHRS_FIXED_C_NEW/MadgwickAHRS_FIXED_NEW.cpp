//=====================================================================================================
// MadgwickAHRS.c
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalised
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "MadgwickAHRS_FIXED_NEW.h"
#include <math.h>
#include <stdbool.h>


//---------------------------------------------------------------------------------------------------
// Definitions

//#define sampleFreq 512.0f // sample frequency in Hz
#define betaDef 0.1f      // 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions






void InitMadgAHRS(MadgAHRSData_t *pMadg)
{
   pMadg->beta = betaDef;     
   pMadg->_q = Vec4D(0.0, 0.0, 0.0, 1.0);
  
}

//---------------------------------------------------------------------------------------------------
// Function declarations


MadgAHRSData_t gMadgAHRSNew;
extern "C" double invSqrt(double x);

//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void MadgwickAHRSupdate_FixedNew(double dTime,Vec3D _g, Vec3D _a, Vec3D _m)
{
   double recipNorm;
   Vec4D _s;
   Vec4D _qDot;
  // double qDot1, qDot2, qDot3, qDot4;
   double hx, hy;
   double _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _8bx, _8bz,_2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
   Vec4D _2q;

   bool bHaveMag = true;
   MadgAHRSData_t *pMadgAHRSData = &gMadgAHRSNew;
   // Rate of change of quaternion from gyroscope
   _qDot.w = 0.5f * (-pMadgAHRSData->_q.x * _g.x - pMadgAHRSData->_q.y * _g.y - pMadgAHRSData->_q.z * _g.z);
   _qDot.x = 0.5f * (pMadgAHRSData->_q.w * _g.x + pMadgAHRSData->_q.y * _g.z - pMadgAHRSData->_q.z * _g.y);
   _qDot.y = 0.5f * (pMadgAHRSData->_q.w * _g.y - pMadgAHRSData->_q.x * _g.z + pMadgAHRSData->_q.z * _g.x);
   _qDot.z = 0.5f * (pMadgAHRSData->_q.w * _g.z + pMadgAHRSData->_q.x * _g.y - pMadgAHRSData->_q.y * _g.x);

   if((_m.x == 0.0f) && (_m.y == 0.0f) && (_m.z == 0.0f))
   {
      bHaveMag = false;
   }

   // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
   if(!((_a.x == 0.0f) && (_a.y == 0.0f) && (_a.z == 0.0f)))
   {

      // Normalise accelerometer measurement
      _a.SetNormalized();

      // Auxiliary variables to avoid repeated arithmetic
      _2q = pMadgAHRSData->_q*2.0;
      
      q0q0 = pMadgAHRSData->_q.w * pMadgAHRSData->_q.w;
      q1q1 = pMadgAHRSData->_q.x * pMadgAHRSData->_q.x;
      q2q2 = pMadgAHRSData->_q.y * pMadgAHRSData->_q.y;
      q3q3 = pMadgAHRSData->_q.z * pMadgAHRSData->_q.z;


      if((_m.x == 0.0f) && (_m.y == 0.0f) && (_m.z == 0.0f))
      {
         // Auxiliary variables to avoid repeated arithmetic
        
         // Gradient decent algorithm corrective step
         _s.w = (4.0f * pMadgAHRSData->_q.w) * q2q2 + _2q.y * _a.x + (4.0f * pMadgAHRSData->_q.w) * q1q1 - _2q.x * _a.y;
         _s.x = (4.0f * pMadgAHRSData->_q.x) * q3q3 - _2q.z * _a.x + 4.0f * q0q0 * pMadgAHRSData->_q.x - _2q.w * _a.y - (4.0f * pMadgAHRSData->_q.x) + (8.0f * pMadgAHRSData->_q.x) * q1q1 + (8.0f * pMadgAHRSData->_q.x) * q2q2 + (4.0f * pMadgAHRSData->_q.x) * _a.z;
         _s.y = 4.0f * q0q0 * pMadgAHRSData->_q.y + _2q.w * _a.x + (4.0f * pMadgAHRSData->_q.y) * q3q3 - _2q.z * _a.y - (4.0f * pMadgAHRSData->_q.y) + (8.0f * pMadgAHRSData->_q.y) * q1q1 + (8.0f * pMadgAHRSData->_q.y) * q2q2 + (4.0f * pMadgAHRSData->_q.y) * _a.z;
         _s.z = 4.0f * q1q1 * pMadgAHRSData->_q.z - _2q.x * _a.x + 4.0f * q2q2 * pMadgAHRSData->_q.z - _2q.y * _a.y;
      }
      else
      {
         // Normalise magnetometer measurement
         _m.SetNormalized();
        
         // Auxiliary variables to avoid repeated arithmetic
         _2q0mx = 2.0f * pMadgAHRSData->_q.w * _m.x;
         _2q0my = 2.0f * pMadgAHRSData->_q.w * _m.y;
         _2q0mz = 2.0f * pMadgAHRSData->_q.w * _m.z;
         _2q1mx = 2.0f * pMadgAHRSData->_q.x * _m.x;
         _2q0q2 = 2.0f * pMadgAHRSData->_q.w * pMadgAHRSData->_q.y;
         _2q2q3 = 2.0f * pMadgAHRSData->_q.y * pMadgAHRSData->_q.z;
         q0q1 = pMadgAHRSData->_q.w * pMadgAHRSData->_q.x;
         q0q2 = pMadgAHRSData->_q.w * pMadgAHRSData->_q.y;
         q0q3 = pMadgAHRSData->_q.w * pMadgAHRSData->_q.z;
         q1q2 = pMadgAHRSData->_q.x * pMadgAHRSData->_q.y;
         q1q3 = pMadgAHRSData->_q.x * pMadgAHRSData->_q.z;
         q2q3 = pMadgAHRSData->_q.y * pMadgAHRSData->_q.z;
         // Reference direction of Earth's magnetic field
         hx = _m.x * q0q0 - _2q0my * pMadgAHRSData->_q.z + _2q0mz * pMadgAHRSData->_q.y + _m.x * q1q1 + _2q.x * _m.y * pMadgAHRSData->_q.y + _2q.x * _m.z * pMadgAHRSData->_q.z - _m.x * q2q2 - _m.x * q3q3;
         hy = _2q0mx * pMadgAHRSData->_q.z + _m.y * q0q0 - _2q0mz * pMadgAHRSData->_q.x + _2q1mx * pMadgAHRSData->_q.y - _m.y * q1q1 + _m.y * q2q2 + _2q.y * _m.z * pMadgAHRSData->_q.z - _m.y * q3q3;
         _2bx = sqrt(hx * hx + hy * hy);
         _2bz = -_2q0mx * pMadgAHRSData->_q.y + _2q0my * pMadgAHRSData->_q.x + _m.z * q0q0 + _2q1mx * pMadgAHRSData->_q.z - _m.z * q1q1 + _2q.y * _m.y * pMadgAHRSData->_q.z - _m.z * q2q2 + _m.z * q3q3;
         _4bx = 2.0f * _2bx;
         _4bz = 2.0f * _2bz;
         _8bx = 2.0f * _4bx;
         _8bz = 2.0f * _4bz;

         // Gradient decent algorithm corrective step
         _s.w = -_2q.y * (2.0f * (q1q3 - q0q2) - _a.x) + _2q.x * (2.0f * (q0q1 + q2q3) - _a.y) + -_4bz * pMadgAHRSData->_q.y * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - _m.x) + (-_4bx * pMadgAHRSData->_q.z + _4bz * pMadgAHRSData->_q.x) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - _m.y) + _4bx * pMadgAHRSData->_q.y * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - _m.z);
         _s.x = _2q.z * (2.0f * (q1q3 - q0q2) - _a.x) + _2q.w * (2.0f * (q0q1 + q2q3) - _a.y) + -4.0f * pMadgAHRSData->_q.x * (2.0f * (0.5 - q1q1 - q2q2) - _a.z) + _4bz * pMadgAHRSData->_q.z * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - _m.x) + (_4bx * pMadgAHRSData->_q.y + _4bz * pMadgAHRSData->_q.w) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - _m.y) + (_4bx * pMadgAHRSData->_q.z - _8bz * pMadgAHRSData->_q.x) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - _m.z);
         _s.y = -_2q.w * (2.0f * (q1q3 - q0q2) - _a.x) + _2q.z * (2.0f * (q0q1 + q2q3) - _a.y) + (-4.0f * pMadgAHRSData->_q.y) * (2.0f * (0.5 - q1q1 - q2q2) - _a.z) + (-_8bx * pMadgAHRSData->_q.y - _4bz * pMadgAHRSData->_q.w) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - _m.x) + (_4bx * pMadgAHRSData->_q.x + _4bz * pMadgAHRSData->_q.z) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - _m.y) + (_4bx * pMadgAHRSData->_q.w - _8bz * pMadgAHRSData->_q.y) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - _m.z);
         _s.z = _2q.x * (2.0f * (q1q3 - q0q2) - _a.x) + _2q.y * (2.0f * (q0q1 + q2q3) - _a.y) + (-_8bx * pMadgAHRSData->_q.z + _4bz * pMadgAHRSData->_q.x) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - _m.x) + (-_4bx * pMadgAHRSData->_q.w + _4bz * pMadgAHRSData->_q.y) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - _m.y) + (_4bx * pMadgAHRSData->_q.x) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - _m.z);
      }
      _s.SetNormalized();

      // Apply feedback step
      _qDot -= (_s*pMadgAHRSData->beta);
   }

   // Integrate rate of change of quaternion to yield quaternion
   pMadgAHRSData->_q += _qDot * dTime;
   // Normalize quaternion
   pMadgAHRSData->_q.SetNormalized();
}
