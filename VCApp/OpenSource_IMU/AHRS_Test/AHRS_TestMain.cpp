
extern "C"
{
#include "../MadgwickAHRS_C/MadgwickAHRS_C.h"
#include "../MadgwickAHRS_FIXED_C/MadgwickAHRS_FIXED.h"
#include "../MahonyAHRS_C/MahonyAHRS_C.h"
}

#include "../MadgwickAHRS_FIXED_C_NEW/MadgwickAHRS_FIXED_NEW.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <conio.h>
#include <MatConv.h>



typedef struct TestFrame_s
{
   Vec3D Gyr;
   Vec3D Acc;
   Vec3D Mag;
}TestFrame_t;

typedef struct MAhonyFrameRes_s
{
   double twoKp;// 2 * proportional gain (Kp)
   double twoKi;// 2 * integral gain (Ki)
   double Mah_q0, Mah_q1, Mah_q2, Mah_q3; // quaternion of sensor frame relative to auxiliary frame
   double integralFBx, integralFBy, integralFBz; // integral error terms scaled by Ki
}MAhonyFrameRes_t;

#define ComparePrecision 0.0000000000001
typedef struct MagFrameRes_s
{
   double Madg_Fix_beta;                                                        // 2 * proportional gain (Kp)
   double Madg_Fix_q0, Madg_Fix_q1, Madg_Fix_q2, Madg_Fix_q3; // quaternion of sensor frame relative to auxiliary frame 


   bool IsEqual(const MagFrameRes_s &Fr2)
   {
      if(
         abs(Madg_Fix_beta - Fr2.Madg_Fix_beta) > ComparePrecision ||
         abs(Madg_Fix_q0 - Fr2.Madg_Fix_q0) > ComparePrecision ||
         abs(Madg_Fix_q1 - Fr2.Madg_Fix_q1) > ComparePrecision ||
         abs(Madg_Fix_q2 - Fr2.Madg_Fix_q2) > ComparePrecision ||
         abs(Madg_Fix_q3 - Fr2.Madg_Fix_q3) > ComparePrecision
         )
      {
         return false;
      }
      return true;

   }

   bool IsEqual(const MadgAHRSData_s &Fr2)
   {
      if(
         abs(Madg_Fix_beta - Fr2.beta) > ComparePrecision ||
         abs(Madg_Fix_q0 - Fr2._q.w) > ComparePrecision ||
         abs(Madg_Fix_q1 - Fr2._q.x) > ComparePrecision ||
         abs(Madg_Fix_q2 - Fr2._q.y) > ComparePrecision ||
         abs(Madg_Fix_q3 - Fr2._q.z) > ComparePrecision
         )
      {
         return false;
      }
      return true;

   }

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
  
   Fr.Acc.x =(drand(100)>50?1.0:-1.0)*sqrt(drand(100000) / 1000.0);
   Fr.Acc.y =(drand(100)>50?1.0:-1.0)*sqrt(drand(100000) / 1000.0);
   Fr.Acc.z =(drand(100)>50?1.0:-1.0)*sqrt(drand(100000) / 1000.0);

   Fr.Gyr.x =(drand(100)>50?1.0:-1.0)*sqrt(drand(10000) / 1000.0);
   Fr.Gyr.y =(drand(100)>50?1.0:-1.0)*sqrt(drand(10000) / 1000.0);
   Fr.Gyr.z =(drand(100)>50?1.0:-1.0)*sqrt(drand(10000) / 1000.0);

   Fr.Mag.x =(drand(100)>50?1.0:-1.0)*sqrt(drand(1000000) / 1000.0);
   Fr.Mag.y =(drand(100)>50?1.0:-1.0)*sqrt(drand(1000000) / 1000.0);
   Fr.Mag.z =(drand(100)>50?1.0:-1.0)*sqrt(drand(1000000) / 1000.0);
   return Fr;
}

typedef struct FileFrame_s
{
   TestFrame_t Fr;
   MagFrameRes_t CurMag, CurMagIMU;
   MAhonyFrameRes_t CurMah, CurMahIMU;
}FileFrame_t;

void GenTestData()
{
   FileFrame_t FFrame;
//    MagFrameRes_t CurMag, CurMagIMU;
//    MAhonyFrameRes_t CurMah, CurMahIMU;
   FFrame.CurMag = GetMagFrame();
   FFrame.CurMagIMU = GetMagFrame();
   FFrame.CurMah = GetMahFrame();
   FFrame.CurMahIMU = GetMahFrame();
   FILE *TestData = fopen("IMU_TEST_DATA.bin", "wb");
   if (TestData)
   {
      for (int i = 0; i < 50000000; i++)
      {
         if (i % 10000 == 0)
         {
            printf("%i\n", i);
         }
         FFrame.Fr = GenerateTestFrame();
         SetMagFrame(FFrame.CurMagIMU);
         MadgwickAHRSupdateIMU_Fixed(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z);
         FFrame.CurMagIMU = GetMagFrame();

         SetMagFrame(FFrame.CurMag);
         MadgwickAHRSupdate_Fixed(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z,FFrame.Fr.Mag.x, FFrame.Fr.Mag.y,FFrame.Fr.Mag.z);
         FFrame.CurMag = GetMagFrame();


         SetMahFrame(FFrame.CurMahIMU);
         MahonyAHRSupdateIMU(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z);
         FFrame.CurMahIMU = GetMahFrame();

         SetMahFrame(FFrame.CurMah);
         MahonyAHRSupdate(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z, FFrame.Fr.Mag.x, FFrame.Fr.Mag.y, FFrame.Fr.Mag.z);
         FFrame.CurMah = GetMahFrame();


         fwrite(&FFrame, sizeof(FFrame), 1, TestData);
       
      }
      fclose(TestData);
   }


}


void ReadTestData()
{
//    CurMag = GetMagFrame();
//    CurMagIMU = GetMagFrame();
//    CurMah = GetMahFrame();
//    CurMahIMU = GetMahFrame();
   FILE *TestData = fopen("IMU_TEST_DATA.bin", "rb");
   size_t FileSz;
   MagFrameRes_t CurMag, CurMagIMU;
   MAhonyFrameRes_t CurMah, CurMahIMU;

   CurMag = GetMagFrame();
   CurMagIMU = GetMagFrame();
   CurMah = GetMahFrame();
   CurMahIMU = GetMahFrame();
   int i = 0;
   bool bHaveError = false;
   FileFrame_t FFrame;
   if(TestData)
   {
      auto SeeRes = _fseeki64(TestData, 0, SEEK_END);
      FileSz = ftell(TestData);
      _fseeki64(TestData, 0, SEEK_SET);
      while(FileSz - ftell(TestData) > sizeof(FileFrame_t)&&!bHaveError)
      {
         i++;
         fread(&FFrame, sizeof(FFrame), 1, TestData);
         if(i % 10000 == 0)
         {
            printf("%i\n", i);
         }
        
         SetMagFrame(CurMagIMU);
         MadgwickAHRSupdateIMU_Fixed(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z);
         CurMagIMU = GetMagFrame();

         SetMagFrame(CurMag);
         MadgwickAHRSupdate_Fixed(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z,FFrame.Fr.Mag.x, FFrame.Fr.Mag.y,FFrame.Fr.Mag.z);
         CurMag = GetMagFrame();


         SetMahFrame(CurMahIMU);
         MahonyAHRSupdateIMU(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z);
         CurMahIMU = GetMahFrame();

         SetMahFrame(CurMah);
         MahonyAHRSupdate(FFrame.Fr.Gyr.x, FFrame.Fr.Gyr.y, FFrame.Fr.Gyr.z, FFrame.Fr.Acc.x, FFrame.Fr.Acc.y, FFrame.Fr.Acc.z, FFrame.Fr.Mag.x, FFrame.Fr.Mag.y, FFrame.Fr.Mag.z);
         CurMah = GetMahFrame();

         if(memcmp(&CurMag, &FFrame.CurMag, sizeof(CurMag)))
         {
            printf("Error: CurMag\n");
            bHaveError = true;
         }

         if(memcmp(&CurMagIMU, &FFrame.CurMagIMU, sizeof(CurMagIMU)))
         {
            printf("Error: CurMagIMU\n");
            bHaveError = true;
         }


         if(memcmp(&CurMah, &FFrame.CurMah, sizeof(CurMah)))
         {
            printf("Error: CurMah\n");
            bHaveError = true;
         }

         if(memcmp(&CurMahIMU, &FFrame.CurMahIMU, sizeof(CurMahIMU)))
         {
            printf("Error: CurMahIMU\n");
            bHaveError = true;
         }


      }
      fclose(TestData);
   }
}




void TestNoWrite()
{
   MadgAHRSData_t CurMagNew, CurMagIMUNew;
   MagFrameRes_t CurMag, CurMagIMU;
   MAhonyFrameRes_t CurMah, CurMahIMU;

   InitMadgAHRS(&CurMagNew);
   InitMadgAHRS(&CurMagIMUNew);
   CurMag = GetMagFrame();
   CurMagIMU = GetMagFrame();
   CurMah = GetMahFrame();
   CurMahIMU = GetMahFrame();
   TestFrame_t Fr;
   bool bHaveError = false;
   int i = 0;
   do
   {
      if(i % 10000 == 0)
      {
         printf("%i\n", i);
      }
      Fr = GenerateTestFrame();
      SetMagFrame(CurMagIMU);
      MadgwickAHRSupdateIMU_Fixed(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z);
      CurMagIMU = GetMagFrame();

      SetMagFrame(CurMag);
      MadgwickAHRSupdate_Fixed(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z, Fr.Mag.x, Fr.Mag.y, Fr.Mag.z);
      CurMag = GetMagFrame();

      gMadgAHRSNew = CurMagNew;
      MadgwickAHRSupdate_FixedNew(1.0/512.0f, Fr.Gyr, Fr.Acc, Fr.Mag);
      CurMagNew = gMadgAHRSNew;

      gMadgAHRSNew = CurMagIMUNew;
      MadgwickAHRSupdate_FixedNew(1.0 / 512.0f,Fr.Gyr, Fr.Acc, NullVec3D);
      CurMagIMUNew = gMadgAHRSNew;


#if 0

      SetMahFrame(CurMahIMU);
      MahonyAHRSupdateIMU(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z);
      CurMahIMU = GetMahFrame();

      SetMahFrame(CurMah);
      MahonyAHRSupdate(Fr.Gyr.x, Fr.Gyr.y, Fr.Gyr.z, Fr.Acc.x, Fr.Acc.y, Fr.Acc.z, Fr.Mag.x, Fr.Mag.y, Fr.Mag.z);
      CurMah = GetMahFrame();

#endif
      //if(memcmp(&CurMag, &CurMagNew, sizeof(CurMag)))
      if(!CurMag.IsEqual(CurMagNew))
      {
         printf("Error: CurMag\n");
         bHaveError = true;
      }

      //if(memcmp(&CurMagIMU, &CurMagIMUNew, sizeof(CurMagIMU)))
      if(!CurMagIMU.IsEqual(CurMagIMUNew))
      {
         printf("Error: CurMagIMU\n");
         bHaveError = true;
      }

      if(bHaveError)
      {
         break;;
      }

      if(_kbhit())
      {
         if(_getch()!=-1)
         {
            break;;
         }
      }

      i++;
   }
   while(1);
   


}

int main()
{
   //GenTestData();
  // ReadTestData();
   TestNoWrite();

   MadgwickAHRSupdateIMU(0, 0, 0, 0, 0, 0);
   return 0;
}