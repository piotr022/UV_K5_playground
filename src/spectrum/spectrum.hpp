#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

#define REVERSE_UINT16(x) ((((x)&0xFF00) >> 8) | (((x)&0x00FF) << 8))

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData>
class CSpectrum
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   static constexpr unsigned char EnableKey = 13;
   static constexpr auto DrawingSize = TUV_K5Display::SizeX * 2;
   CSpectrum()
       : DisplayBuff(FwData.pDisplayBuffer),
         FontSmallNr(FwData.pSmallDigs),
         Display(DisplayBuff),
         bDisplayCleared(true),
         p8DrawingStart((unsigned char *)DisplayBuff.GetCoursorData(
             DisplayBuff.GetCoursorPosition(2, 0)))
   {
      Display.SetFont(&FontSmallNr);
   };

   void Handle()
   {
      // char C8RssiString[] = "g000";
      // auto u8Rssi = Fw.PollKeyboard();
      // C8RssiString[1] = '0' + u8Rssi / 100;
      // C8RssiString[2] = '0' + (u8Rssi / 10) % 10;
      // C8RssiString[3] = '0' + u8Rssi % 10;
      // Display.SetCoursor(3, 0);
      // Display.SetFont(&FontSmallNr);
      // Display.Print(C8RssiString);
      // Fw.FlushFramebufferToScreen();
      // while(1);

      // if(!FreeToDraw())
      // {
      //    if(!bDisplayCleared)
      //    {
      //       bDisplayCleared = true;
      //       ClearDrawings();
      //       Fw.FlushFramebufferToScreen();
      //    }

      //    return;
      // }

      bDisplayCleared = false;

      Display.SetCoursor(3, 0);
      Display.PrintFixedDigtsNumer(123456, 10); 
      Display.DrawCircle(64, 25, 8, false);

      Fw.FlushFramebufferToScreen();
   }

private:
   void SetFrequency(unsigned int u32Freq)
   {
      u32Freq /= 10;
      Fw.BK4819Write(0x39, REVERSE_UINT16((u32Freq >> 16) & 0xFFFF));
      Fw.BK4819Write(0x38, REVERSE_UINT16(u32Freq & 0xFFFF));
   }

   unsigned int GetFrequency()
   {
      unsigned short u16f1 = Fw.BK4819Write(0x39);
      unsigned short u16f2 = Fw.BK4819Write(0x38);
      return (u16f1 << 16) | u16f2;
   }

   bool FreeToDraw()
   {
      auto const PressedKey = Fw.PollKeyboard();
      auto *pMenuCheckData = (unsigned char *)DisplayBuff.GetCoursorData(
          DisplayBuff.GetCoursorPosition(2, 6 * 8 + 1));

      return PressedKey == EnableKey && *pMenuCheckData != 0xFF;
   }

   void ClearDrawings()
   {
      memset(p8DrawingStart, 0, DrawingSize);
   }

   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   bool bDisplayCleared;

   unsigned char *const p8DrawingStart;
};