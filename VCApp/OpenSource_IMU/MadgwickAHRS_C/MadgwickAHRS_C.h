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
#ifndef MadgwickAHRS_C_h
#define MadgwickAHRS_C_h

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern volatile double Madg_beta;				// algorithm gain
extern volatile double Madg_q0, Madg_q1, Madg_q2, Madg_q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void MadgwickAHRSupdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz);
void MadgwickAHRSupdateIMU(double gx, double gy, double gz, double ax, double ay, double az);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
