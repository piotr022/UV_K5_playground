#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "radio.hpp"

template <const System::TOrgFunctions &Fw,
          const System::TOrgData &FwData,
          Radio::CBK4819<Fw> &RadioDriver>
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
       : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs), Display(DisplayBuff), State(eState::Init), u8RxCnt(0){};

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
         

         Fw.DelayMs(600);
         //memset(U8Buff, 0, sizeof(U8Buff));
         RadioDriver.RecieveAsyncAirCopyMode(U8Buff, sizeof(U8Buff), Radio::CallbackRxDoneType(this, &CSpectrum::RxDoneHandler));
         State = eState::RxPending;
         // while(State == eState::RxPending)
         // {
         //    RadioDriver.InterruptHandler();
         //    // if(Fw.PollKeyboard() != 0xFF)
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
         Fw.FormatString(kupa, "RX DONE %u", u8RxCnt);
         Fw.PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         Fw.FormatString(kupa, "LEN: %i", RadioDriver.u16RxDataLen);
         Fw.PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         Fw.FormatString(kupa, "S: %s", U8Buff);
         Fw.PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         Fw.FlushFramebufferToScreen();

         static unsigned int u32Cnt = 1;
         if(!(u32Cnt++%8))
         {
            u8RxCnt++;
            State = eState::Init;
         }

         // Fw.WriteSerialData((unsigned char *)"RX packet, hex: ", 17);
         // for (unsigned int i = 0; i < sizeof(U8Buff); i++)
         // {
         //    Fw.FormatString(kupa, "%02X", U8Buff[i]);
         //    Fw.WriteSerialData((unsigned char *)kupa, 1);
         // }
         // Fw.WriteSerialData((unsigned char *)"\n", 1);
         break;
      }

      case eState::RxPending:
      {

         //Fw.AirCopyFskSetup();
         char kupa[20];
         DisplayBuff.ClearAll();
         Fw.FormatString(kupa, "Rx: %u kHz", Fw.IntDivide(RadioDriver.GetFrequency(), 100));
         Fw.PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         Fw.FormatString(kupa, "0x3F: 0x%04X", Fw.BK4819Read(0x3F));
         Fw.PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         Fw.FormatString(kupa, "len: %i", Fw.BK4819Read(0x5D) >> 8);
         Fw.PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         Fw.FlushFramebufferToScreen();
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
            Fw.FormatString((char *)C8TxStr, "packet %i", u8TxCnt);
            RadioDriver.SendSyncAirCopyMode72((unsigned char *)C8TxStr);
         }

         DisplayBuff.ClearAll();
         Fw.FormatString(kupa, "TX: %u kHz", Fw.IntDivide(RadioDriver.GetFrequency(), 100));
         Fw.PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);
         Fw.FormatString(kupa, "Irq: 0x%04X", RadioDriver.GetIrqReg());
         Fw.PrintTextOnScreen(kupa, 0, 127, 2, 8, 0);
         Fw.FormatString(kupa, "S: %s", C8TxStr);
         Fw.PrintTextOnScreen(kupa, 0, 127, 4, 8, 0);
         Fw.FlushFramebufferToScreen();


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