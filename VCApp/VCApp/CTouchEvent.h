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
   virtual bool                            ParseEvent(const void* pData, size_t nDataSize, bool bEndian) override;
   virtual uint32_t                        GetEventID() override;
   virtual std::shared_ptr<IEventReceiver> GetEvShared() override;

 private:
   ndk_helper::TapDetector       m_TapDetector;
   ndk_helper::DoubletapDetector m_DoubletapDetector;
   ndk_helper::PinchDetector     m_PinchDetector;
   ndk_helper::DragDetector      m_DragDetector;
};
