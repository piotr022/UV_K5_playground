#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "radio.hpp"

template <Radio::CBK4819 &RadioDriver>
class CSpectrum
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
       : DisplayBuff(gDisplayBuffer), FontSmallNr(gSmallDigs), Display(DisplayBuff), State(eState::Init), u8RxCnt(0){};

   void Handle()
   {
      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      switch (State)
      {
      case eState::Init:
      {
         

         DelayMs(600);
         //memset(U8Buff, 0, sizeof(U8Buff));
         RadioDriver.RecieveAsyncAirCopyMode(U8Buff, sizeof(U8Buff), Radio::CallbackRxDoneType(this, &CSpectrum::RxDoneHandler));
         State = eState::RxPending;
         // while(State == eState::RxPending)
         // {
         //    RadioDriver.InterruptHandler();
         //    // if(PollKeyboard() != 0xFF)
         //    // {
         //    //    break;
         //    // }
         // }

         break;
      }

      case eState::RxDone:
      {
         char kupa[20];
         U8Buff[10] = 0;
         DisplayBuff.ClearAll();
         FormatString(kupa, "RX DONE %u", u8RxCnt);
         PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         FormatString(kupa, "LEN: %i", RadioDriver.u16RxDataLen);
         PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         FormatString(kupa, "S: %s", U8Buff);
         PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         FlushFramebufferToScreen();

         static unsigned int u32Cnt = 1;
         if(!(u32Cnt++%8))
         {
            u8RxCnt++;
            State = eState::Init;
         }

         // WriteSerialData((unsigned char *)"RX packet, hex: ", 17);
         // for (unsigned int i = 0; i < sizeof(U8Buff); i++)
         // {
         //    FormatString(kupa, "%02X", U8Buff[i]);
         //    WriteSerialData((unsigned char *)kupa, 1);
         // }
         // WriteSerialData((unsigned char *)"\n", 1);
         break;
      }

      case eState::RxPending:
      {

         //AirCopyFskSetup();
         char kupa[20];
         DisplayBuff.ClearAll();
         FormatString(kupa, "Rx: %u kHz", IntDivide(RadioDriver.GetFrequency(), 100));
         PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         FormatString(kupa, "0x3F: 0x%04X", BK4819Read(0x3F));
         PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         FormatString(kupa, "len: %i", BK4819Read(0x5D) >> 8);
         PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         FlushFramebufferToScreen();
         return;
      }

      case eState::SendData:
      {

         static char C8TxStr[20] __attribute__((aligned(4)));
         char kupa[20];
         static unsigned char u8TxCnt = 0;
         static unsigned int u32Cnt = 1;
         if(!(u32Cnt++%8))
         {
            u8TxCnt++;
            FormatString((char *)C8TxStr, "packet %i", u8TxCnt);
            RadioDriver.SendSyncAirCopyMode72((unsigned char *)C8TxStr);
         }

         DisplayBuff.ClearAll();
         FormatString(kupa, "TX: %u kHz", IntDivide(RadioDriver.GetFrequency(), 100));
         PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         FormatString(kupa, "Irq: 0x%04X", RadioDriver.GetIrqReg());
         PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         FormatString(kupa, "S: %s", C8TxStr);
         PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         FlushFramebufferToScreen();


         return;
      }

      default:
         return;
      }
   }

   void RxDoneHandler(unsigned char u8DataLen, bool bCrcOk)
   {
      State = eState::RxDone;
   }

private:
   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   eState State;
   unsigned char U8Buff[72];
   unsigned char u8RxCnt;
};
