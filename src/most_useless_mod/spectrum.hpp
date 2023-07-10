#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "radio.hpp"

template <const System::TOrgFunctions &Fw,
    const System::TOrgData &FwData,
    Radio::CBK4819<Fw>& RadioDriver>
class CSpectrum : public Radio::IRadioUser
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;

   enum eState : unsigned char
   {
      Init,
      Idle,
      RxPending,
      SendData,
      RxDone,
      Exit,
   };

   CSpectrum()
       : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs), Display(DisplayBuff), State(eState::SendData), u8RxCnt(0){};

   void Handle()
   {
      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      switch(State)
      {
         case eState::Init:
         {
            RadioDriver.RecieveAsyncAirCopyMode(U8RxBuff, sizeof(U8RxBuff), this);
            State = eState::RxPending;
            break;
         }

         case eState::RxDone:
         {
            u8RxCnt++;
            char kupa[20];
            Fw.FormatString(kupa, "test %u", u8RxCnt);
            Fw.PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
            Fw.FlushFramebufferToScreen();
            State = eState::Init;
            break;
         }

         case eState::SendData:
         {
            RadioDriver.SendSyncAirCopyMode72((unsigned char*)this);
            return;
         }

         default:
            return;
      }
   }

   void RxDoneHandler() override
   {
      State = eState::RxDone;
   }

private:
   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   eState State;
   unsigned char U8RxBuff[72];
   unsigned char u8RxCnt;

};