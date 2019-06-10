
extern "C"
{
#include "../MadgwickAHRS_C/MadgwickAHRS_C.h"
#include "../MadgwickAHRS_FIXED_C/MadgwickAHRS_FIXED.h"
#include "../MahonyAHRS_C/MahonyAHRS_C.h"

}
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

typedef struct Vec3D_s
{
   double x, y, z;
}Vec3D_t;

typedef struct TestFrame_s
{
   Vec3D_t Gyr;
   Vec3D_t Acc;
   Vec3D_t Mag;
}TestFrame_t;

typedef struct MAhonyFrameRes_s
{
   double twoKp;// 2 * proportional gain (Kp)
   double twoKi;// 2 * integral gain (Ki)
   double Mah_q0, Mah_q1, Mah_q2, Mah_q3; // quaternion of sensor frame relative to auxiliary frame
   double integralFBx, integralFBy, integralFBz; // integral error terms scaled by Ki
}MAhonyFrameRes_t;

typedef struct MagFrameRes_s
{
   double Madg_Fix_beta;                                                        // 2 * proportional gain (Kp)
   double Madg_Fix_q0, Madg_Fix_q1, Madg_Fix_q2, Madg_Fix_q3; // quaternion of sensor frame relative to auxiliary frame 
}MagFrameRes_t;


#define drand(div) ((double)(rand()%(div)))


MAhonyFrameRes_t GetMahFrame()
{
   MAhonyFrameRes_t FrRes;
   FrRes.twoKp = twoKp;
   FrRes.twoKi = twoKi;
   FrRes.Mah_q0 = Mah_q0;
   FrRes.Mah_q1 = Mah_q1;
   FrRes.Mah_q2 = Mah_q2;
   FrRes.Mah_q3 = Mah_q3;
   FrRes.integralFBx = integralFBx;
   FrRes.integralFBy = integralFBy;
   FrRes.integralFBz = integralFBz;
   return FrRes;
}

void SetMahFrame(MAhonyFrameRes_t &Fr)
{
   twoKp = Fr.twoKp;
   twoKi = Fr.twoKi;
   Mah_q0 = Fr.Mah_q0;
   Mah_q1 = Fr.Mah_q1;
   Mah_q2 = Fr.Mah_q2;
   Mah_q3 = Fr.Mah_q3;
   integralFBx = Fr.integralFBx;
   integralFBy = Fr.integralFBy;
   integralFBz = Fr.integralFBz;
}

MagFrameRes_t GetMagFrame()
{
   MagFrameRes_t FrRes;
   FrRes.Madg_Fix_beta = Madg_Fix_beta;
   FrRes.Madg_Fix_q0 = Madg_Fix_q0;
   FrRes.Madg_Fix_q1 = Madg_Fix_q1;
   FrRes.Madg_Fix_q2 = Madg_Fix_q2;
   FrRes.Madg_Fix_q3 = Madg_Fix_q3;
   return FrRes;
}
void SetMagFrame(MagFrameRes_t &Fr)
{
   Madg_Fix_beta =Fr.Madg_Fix_beta;
   Madg_Fix_q0 =  Fr.Madg_Fix_q0;
   Madg_Fix_q1 =  Fr.Madg_Fix_q1;
   Madg_Fix_q2 =  Fr.Madg_Fix_q2;
   Madg_Fix_q3 =  Fr.Madg_Fix_q3;
}

TestFrame_t GenerateTestFrame()
{

   TestFrame_t Fr;
  
   Fr.Acc.x = sqrt(drand(100000) / 1000.0);
   Fr.Acc.y = sqrt(drand(100000) / 1000.0);
   Fr.Acc.z = sqrt(drand(100000) / 1000.0);

   Fr.Gyr.x = sqrt(drand(10000) / 1000.0);
   Fr.Gyr.y = sqrt(drand(10000) / 1000.0);
   Fr.Gyr.z = sqrt(drand(10000) / 1000.0);

   Fr.Mag.x = sqrt(drand(1000000) / 1000.0);
   Fr.Mag.y = sqrt(drand(1000000) / 1000.0);
   Fr.Mag.z = sqrt(drand(1000000) / 1000.0);
   return Fr;
}


MagFrameRes_t CurMag, CurMagIMU;
MAhonyFrameRes_t CurMah, CurMahIMU;
void GenTestData()
{
   CurMag = GetMagFrame();
   CurMagIMU = GetMagFrame();
   CurMah = GetMahFrame();
   CurMahIMU = GetMahFrame();
   FILE *TestData = fopen("IMU_TEST_DATA.bin", "wb");
   if (TestData)
   {
      for (int i = 0; i < 10000000; i++)
      {
         if (i % 10000 == 0)
         {
            printf("%i\n", i);
         }
         auto Fr = GenerateTestFrame();
         SetMagFrame(CurMagIMU);
         MadgwickAHRSupdateIMU_Fixed(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z);
         CurMagIMU = GetMagFrame();

         SetMagFrame(CurMag);
         MadgwickAHRSupdate_Fixed(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z,Fr.Mag.x, Fr.Mag.y,Fr.Mag.z);
         CurMag = GetMagFrame();


         SetMahFrame(CurMahIMU);
         MahonyAHRSupdateIMU(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z);
         CurMahIMU = GetMahFrame();

         SetMahFrame(CurMah);
         MahonyAHRSupdate(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z, Fr.Mag.x, Fr.Mag.y, Fr.Mag.z);
         CurMah = GetMahFrame();


         fwrite(&Fr, sizeof(Fr), 1, TestData);
         fwrite(&CurMagIMU, sizeof(CurMagIMU), 1, TestData);
         fwrite(&CurMag, sizeof(CurMag), 1, TestData);
         fwrite(&CurMahIMU, sizeof(CurMahIMU), 1, TestData);
         fwrite(&CurMah, sizeof(CurMah), 1, TestData);
         


      }
      fclose(TestData);
   }


}

int main()
{

   GenTestData();

   MadgwickAHRSupdateIMU(0, 0, 0, 0, 0, 0);
   return 0;
}