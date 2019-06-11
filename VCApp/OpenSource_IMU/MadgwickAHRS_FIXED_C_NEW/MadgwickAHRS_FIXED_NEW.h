
//=====================================================================================================
// MadgwickAHRS.h
//=====================================================================================================
//
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================
#ifndef MadgwickAHRS_FIXED_C_NEW_h
#define MadgwickAHRS_FIXED_C_NEW_h
#include <MatConv.h>
typedef struct MadgAHRSData_s
{
   double beta;                                                        // 2 * proportional gain (Kp)
   Vec4D _q; // quaternion of sensor frame relative to auxiliary frame
}MadgAHRSData_t;

void InitMadgAHRS(MadgAHRSData_t *pMadg);
extern MadgAHRSData_t gMadgAHRSNew;
//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate_FixedNew(double dTime, Vec3D _g, Vec3D _a=NullVec3D, Vec3D _m=NullVec3D);


#endif
//=====================================================================================================
// End of file
//=====================================================================================================
