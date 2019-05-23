#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"



void RunReceiver(CDataReceiver &pReceiver)
{
   auto TouchEv = CTouchEvent::Create();
   auto SensEv = CSensorEvent::Create();

   pReceiver.AddListener(TouchEv);
   pReceiver.AddListener(SensEv);
   pReceiver.StartThread();
   do
   {

      if(_kbhit())
      {
         auto c = _getch();
         break;
      }
   }
   while(true);
}


void RunUDP()
{
   CReceiverUDP Recv(4452);
   RunReceiver(Recv);
}


void RunFRead()
{
   CReceiverFile RF;
   RF.LoadFile(R"(D:\AnDroid\2019_05_23-13_07_18.bin)");

   RunReceiver(RF);
}
int main()
{

 
  
   RunUDP();
   
   return 1;
}