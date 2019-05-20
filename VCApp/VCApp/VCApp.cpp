#include <stdio.h>
#include "CDataReceiver.h"
#include <conio.h>
#include "CTouchEvent.h"
#include "CSensorEvent.h"


int main()
{

   CReceiverUDP Recv(4452);
   Recv.AddListener(CTouchEvent::Create());
   Recv.AddListener(CSensorEvent::Create());

   do
   {


      if (_kbhit())
      {
         auto c = _getch();
         break;
      }
   }
   while (true);
   return 1;
}