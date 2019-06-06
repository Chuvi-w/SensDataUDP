#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CNMEAEvent.h"
#include <iostream>
#include <functional>
#include "CIMU_Acc.h"

typedef CTouchEvent::PTR(*pfnCreate)();

void Create(std::function<std::shared_ptr<IEventReceiver>()> fnCreate)
{

   auto cr = fnCreate();

   printf("");
}


void RunReceiver(CDataReceiver& pReceiver)
{

  Create(std::bind(&CTouchEvent::Create));
  Create(std::bind(&CSensorEvent::Create));
  
  
  std::vector<CBaseIMUSensor::PTR> vIMU;
   
  

  pReceiver.SetNewReceiversFunc([&vIMU]()
   {
      std::vector<IEventReceiver::PTR> vRecv;
      //auto TouchEv = CTouchEvent::Create();
      auto SensEv = CSensorEvent::Create();
      //auto NMEAEv = CNMEAEvent::Create();
     
      vRecv.push_back(SensEv);

      return vRecv;
      
   });


//   pReceiver.AddEvListenerCreator(&CTouchEvent::Create);
 //  pReceiver.AddEvListenerCreator(&CSensorEvent::Create);
 //  pReceiver.AddEvListenerCreator(&CNMEAEvent::Create);

   pReceiver.StartThread();
   do
   {
      if(_kbhit())
      {
         auto c = _getch();

         if (c == 'q' || c == 'Q')
         {
            break;
         }
         if (c == 's' || c == 'S')
         {
            std::cout << pReceiver.GetStat();
         }

         if(c == 'r' || c == 'R')
         {
            for(auto &IMU : vIMU)
            {
               IMU->ResetFrames();
            }
           
         }

         if(c == 'c' || c == 'C')
         {
            for(auto &IMU : vIMU)
            {
               IMU->Calibrate();
            }

         }
         
      }
   } while(true);
   pReceiver.StopThread();
}

void RunUDP()

{
   CReceiverUDP Recv(4452);
   RunReceiver(Recv);
}

void RunFRead()
{
   CReceiverFile RF;
   printf("Loading...");
   RF.LoadFile(R"(D:\AnDroid\2019_05_31-21_46_50.bin)");
   //RF.LoadFile(R"(D:\AnDroid\2019_05_31-20_26_41.bin)");
 

   RunReceiver(RF);
}
int main()
{

  // RunFRead();

  RunUDP();

   return 1;
}