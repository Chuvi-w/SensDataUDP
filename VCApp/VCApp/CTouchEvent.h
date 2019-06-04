#pragma once
#include "EvCommon.h"
#include "TouchEvData.h"
#include "gestureDetector.h"

class CTouchEvent : public IEventReceiver
{
 private:
   CTouchEvent();
   using PTR = std::shared_ptr<CTouchEvent>;

 public:
   static PTR Create() { return std::shared_ptr<CTouchEvent>(new CTouchEvent); }
   ~CTouchEvent();
   virtual bool                            ParseEvent(const CDataPacket& pPacket) override;
   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
   TapDetector       m_TapDetector;
   DoubletapDetector m_DoubletapDetector;
   PinchDetector     m_PinchDetector;
   DragDetector      m_DragDetector;
};
