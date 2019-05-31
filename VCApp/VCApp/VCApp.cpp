#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"
#include "CNMEAEvent.h"

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
         break;
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
   RF.LoadFile(R"(D:\AnDroid\2019_05_31-18_43_20.bin)");
   RF.LoadFile(R"(D:\AnDroid\2019_05_31-20_26_41.bin)");
   printf("OK");

   RunReceiver(RF);
}
int main()
{

   RunFRead();

   // RunUDP();

   return 1;
}