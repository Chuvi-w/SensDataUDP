
extern "C"
{
#include "../MadgwickAHRS_C/MadgwickAHRS_C.h"
#include "../MadgwickAHRS_FIXED_C/MadgwickAHRS_FIXED.h"
#include "../MahonyAHRS_C/MahonyAHRS_C.h"
}

int main()
{

   MadgwickAHRSupdateIMU(0, 0, 0, 0, 0, 0);
   return 0;
}