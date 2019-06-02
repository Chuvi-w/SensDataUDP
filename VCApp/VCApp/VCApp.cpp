#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CNMEAEvent.h"
#include <iostream>

void RunReceiver(CDataReceiver& pReceiver)
{
   auto TouchEv = CTouchEvent::Create();
   auto SensEv  = CSensorEvent::Create();
   auto NMEAEv  = CNMEAEvent::Create();
   pReceiver.AddListener(TouchEv);
   pReceiver.AddListener(SensEv);
   pReceiver.AddListener(NMEAEv);
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

   //RunFRead();

    RunUDP();

   return 1;
}