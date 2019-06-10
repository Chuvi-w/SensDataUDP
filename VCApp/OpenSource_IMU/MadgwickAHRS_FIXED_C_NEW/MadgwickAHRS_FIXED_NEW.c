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

#define sampleFreq 512.0f // sample frequency in Hz
#define betaDef 0.1f      // 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions






void InitMadgAHRS(MadgAHRSData_t *pMadg)
{
   pMadg->beta = betaDef;                                                      
   pMadg->q0 = 1.0f;
   pMadg->q1 = 0.0f;
   pMadg->q2 = 0.0f;
   pMadg->q3 = 0.0f;
}

//---------------------------------------------------------------------------------------------------
// Function declarations


MadgAHRSData_t gMadgAHRSNew;
double invSqrt(double x);

//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void MadgwickAHRSupdate_FixedNew(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz)
{
   double recipNorm;
   double s0, s1, s2, s3;
   double qDot1, qDot2, qDot3, qDot4;
   double hx, hy;
   double _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _8bx, _8bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;


   bool bHaveMag = true;
   MadgAHRSData_t *pMadgAHRSData = &gMadgAHRSNew;
   // Rate of change of quaternion from gyroscope
   qDot1 = 0.5f * (-pMadgAHRSData->q1 * gx - pMadgAHRSData->q2 * gy - pMadgAHRSData->q3 * gz);
   qDot2 = 0.5f * (pMadgAHRSData->q0 * gx + pMadgAHRSData->q2 * gz - pMadgAHRSData->q3 * gy);
   qDot3 = 0.5f * (pMadgAHRSData->q0 * gy - pMadgAHRSData->q1 * gz + pMadgAHRSData->q3 * gx);
   qDot4 = 0.5f * (pMadgAHRSData->q0 * gz + pMadgAHRSData->q1 * gy - pMadgAHRSData->q2 * gx);

   if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
   {
      bHaveMag = false;
   }

   // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
   if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
   {

      // Normalise accelerometer measurement
      recipNorm = invSqrt(ax * ax + ay * ay + az * az);
      ax *= recipNorm;
      ay *= recipNorm;
      az *= recipNorm;

      // Auxiliary variables to avoid repeated arithmetic
      _2q0 = (2.0f * pMadgAHRSData->q0);
      _2q1 = (2.0f * pMadgAHRSData->q1);
      _2q2 = (2.0f * pMadgAHRSData->q2);
      _2q3 = (2.0f * pMadgAHRSData->q3);
      q0q0 = pMadgAHRSData->q0 * pMadgAHRSData->q0;
      q1q1 = pMadgAHRSData->q1 * pMadgAHRSData->q1;
      q2q2 = pMadgAHRSData->q2 * pMadgAHRSData->q2;
      q3q3 = pMadgAHRSData->q3 * pMadgAHRSData->q3;


      if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
      {
         // Auxiliary variables to avoid repeated arithmetic
        
         // Gradient decent algorithm corrective step
         s0 = (4.0f * pMadgAHRSData->q0) * q2q2 + _2q2 * ax + (4.0f * pMadgAHRSData->q0) * q1q1 - _2q1 * ay;
         s1 = (4.0f * pMadgAHRSData->q1) * q3q3 - _2q3 * ax + 4.0f * q0q0 * pMadgAHRSData->q1 - _2q0 * ay - (4.0f * pMadgAHRSData->q1) + (8.0f * pMadgAHRSData->q1) * q1q1 + (8.0f * pMadgAHRSData->q1) * q2q2 + (4.0f * pMadgAHRSData->q1) * az;
         s2 = 4.0f * q0q0 * pMadgAHRSData->q2 + _2q0 * ax + (4.0f * pMadgAHRSData->q2) * q3q3 - _2q3 * ay - (4.0f * pMadgAHRSData->q2) + (8.0f * pMadgAHRSData->q2) * q1q1 + (8.0f * pMadgAHRSData->q2) * q2q2 + (4.0f * pMadgAHRSData->q2) * az;
         s3 = 4.0f * q1q1 * pMadgAHRSData->q3 - _2q1 * ax + 4.0f * q2q2 * pMadgAHRSData->q3 - _2q2 * ay;
      }
      else
      {
         // Normalise magnetometer measurement
         recipNorm = invSqrt(mx * mx + my * my + mz * mz);
         mx *= recipNorm;
         my *= recipNorm;
         mz *= recipNorm;

         // Auxiliary variables to avoid repeated arithmetic
         _2q0mx = 2.0f * pMadgAHRSData->q0 * mx;
         _2q0my = 2.0f * pMadgAHRSData->q0 * my;
         _2q0mz = 2.0f * pMadgAHRSData->q0 * mz;
         _2q1mx = 2.0f * pMadgAHRSData->q1 * mx;
         _2q0q2 = 2.0f * pMadgAHRSData->q0 * pMadgAHRSData->q2;
         _2q2q3 = 2.0f * pMadgAHRSData->q2 * pMadgAHRSData->q3;
         q0q1 = pMadgAHRSData->q0 * pMadgAHRSData->q1;
         q0q2 = pMadgAHRSData->q0 * pMadgAHRSData->q2;
         q0q3 = pMadgAHRSData->q0 * pMadgAHRSData->q3;
         q1q2 = pMadgAHRSData->q1 * pMadgAHRSData->q2;
         q1q3 = pMadgAHRSData->q1 * pMadgAHRSData->q3;
         q2q3 = pMadgAHRSData->q2 * pMadgAHRSData->q3;
         // Reference direction of Earth's magnetic field
         hx = mx * q0q0 - _2q0my * pMadgAHRSData->q3 + _2q0mz * pMadgAHRSData->q2 + mx * q1q1 + _2q1 * my * pMadgAHRSData->q2 + _2q1 * mz * pMadgAHRSData->q3 - mx * q2q2 - mx * q3q3;
         hy = _2q0mx * pMadgAHRSData->q3 + my * q0q0 - _2q0mz * pMadgAHRSData->q1 + _2q1mx * pMadgAHRSData->q2 - my * q1q1 + my * q2q2 + _2q2 * mz * pMadgAHRSData->q3 - my * q3q3;
         _2bx = sqrt(hx * hx + hy * hy);
         _2bz = -_2q0mx * pMadgAHRSData->q2 + _2q0my * pMadgAHRSData->q1 + mz * q0q0 + _2q1mx * pMadgAHRSData->q3 - mz * q1q1 + _2q2 * my * pMadgAHRSData->q3 - mz * q2q2 + mz * q3q3;
         _4bx = 2.0f * _2bx;
         _4bz = 2.0f * _2bz;
         _8bx = 2.0f * _4bx;
         _8bz = 2.0f * _4bz;

         // Gradient decent algorithm corrective step
         s0 = -_2q2 * (2.0f * (q1q3 - q0q2) - ax) + _2q1 * (2.0f * (q0q1 + q2q3) - ay) + -_4bz * pMadgAHRSData->q2 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (-_4bx * pMadgAHRSData->q3 + _4bz * pMadgAHRSData->q1) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + _4bx * pMadgAHRSData->q2 * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
         s1 = _2q3 * (2.0f * (q1q3 - q0q2) - ax) + _2q0 * (2.0f * (q0q1 + q2q3) - ay) + -4.0f * pMadgAHRSData->q1 * (2.0f * (0.5 - q1q1 - q2q2) - az) + _4bz * pMadgAHRSData->q3 * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (_4bx * pMadgAHRSData->q2 + _4bz * pMadgAHRSData->q0) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * pMadgAHRSData->q3 - _8bz * pMadgAHRSData->q1) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
         s2 = -_2q0 * (2.0f * (q1q3 - q0q2) - ax) + _2q3 * (2.0f * (q0q1 + q2q3) - ay) + (-4.0f * pMadgAHRSData->q2) * (2.0f * (0.5 - q1q1 - q2q2) - az) + (-_8bx * pMadgAHRSData->q2 - _4bz * pMadgAHRSData->q0) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (_4bx * pMadgAHRSData->q1 + _4bz * pMadgAHRSData->q3) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * pMadgAHRSData->q0 - _8bz * pMadgAHRSData->q2) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
         s3 = _2q1 * (2.0f * (q1q3 - q0q2) - ax) + _2q2 * (2.0f * (q0q1 + q2q3) - ay) + (-_8bx * pMadgAHRSData->q3 + _4bz * pMadgAHRSData->q1) * (_4bx * (0.5 - q2q2 - q3q3) + _4bz * (q1q3 - q0q2) - mx) + (-_4bx * pMadgAHRSData->q0 + _4bz * pMadgAHRSData->q2) * (_4bx * (q1q2 - q0q3) + _4bz * (q0q1 + q2q3) - my) + (_4bx * pMadgAHRSData->q1) * (_4bx * (q0q2 + q1q3) + _4bz * (0.5 - q1q1 - q2q2) - mz);
      }
      recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
      s0 *= recipNorm;
      s1 *= recipNorm;
      s2 *= recipNorm;
      s3 *= recipNorm;

      // Apply feedback step
      qDot1 -= pMadgAHRSData->beta * s0;
      qDot2 -= pMadgAHRSData->beta * s1;
      qDot3 -= pMadgAHRSData->beta * s2;
      qDot4 -= pMadgAHRSData->beta * s3;
   }

   // Integrate rate of change of quaternion to yield quaternion
   pMadgAHRSData->q0 += qDot1 * (1.0f / sampleFreq);
   pMadgAHRSData->q1 += qDot2 * (1.0f / sampleFreq);
   pMadgAHRSData->q2 += qDot3 * (1.0f / sampleFreq);
   pMadgAHRSData->q3 += qDot4 * (1.0f / sampleFreq);

   // Normalise quaternion
   recipNorm = invSqrt(pMadgAHRSData->q0 * pMadgAHRSData->q0 + pMadgAHRSData->q1 * pMadgAHRSData->q1 + pMadgAHRSData->q2 * pMadgAHRSData->q2 + pMadgAHRSData->q3 * pMadgAHRSData->q3);
   pMadgAHRSData->q0 *= recipNorm;
   pMadgAHRSData->q1 *= recipNorm;
   pMadgAHRSData->q2 *= recipNorm;
   pMadgAHRSData->q3 *= recipNorm;
}
