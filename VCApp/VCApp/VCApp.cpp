#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"

int main()
{

   auto TouchEv = CTouchEvent::Create();
   auto SensEv = CSensorEvent::Create();

#if 0
   CReceiverFile RF;
   RF.AddListener(TouchEv);
   RF.AddListener(SensEv);
   RF.LoadFile(R"(D:\AnDroid\2019_05_23-13_07_18.bin)");

   return 0;
#endif
   CReceiverUDP Recv(4452);
   Recv.AddListener(TouchEv);
   Recv.AddListener(SensEv);

   do
   {

      if(_kbhit())
      {
         auto c = _getch();
         break;
      }
   } while(true);
   return 1;
}