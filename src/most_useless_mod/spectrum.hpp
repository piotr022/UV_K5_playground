#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

#define REVERSE_UINT16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData>
class CSpectrum
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   CSpectrum()
       : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs), Display(DisplayBuff), x(DisplayBuff.SizeX/2), y(DisplayBuff.SizeY/2){};

   void Handle()
   {
      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      if (!(Fw.BK4819Read(0x0C) & 0b10))
      {
         return;
      }

      char kupa[20];

      Fw.FormatString(kupa, "test %u", 100);
       

      Fw.PrintTextOnScreen(kupa, 0, 127, 0, 8, 0);   
      Fw.FlushFramebufferToScreen();
   }

private:
   void SetFrequency(unsigned int u32Freq)
   {
      u32Freq /= 10;
      Fw.BK4819Write(0x39, REVERSE_UINT16((u32Freq >> 16) & 0xFFFF));
      Fw.BK4819Write(0x38, REVERSE_UINT16(u32Freq & 0xFFFF));
   }

   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   
   unsigned char x, y;
};