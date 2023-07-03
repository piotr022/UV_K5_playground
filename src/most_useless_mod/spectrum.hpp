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
      DisplayBuff.ClearAll();
      char C8RssiString[] = "g000";

      unsigned int u32Key = Fw.PollKeyboard();

      C8RssiString[1] = '0' + (u32Key / 100 )% 10;
      C8RssiString[2] = '0' + (u32Key / 10) % 10;
      C8RssiString[3] = '0' + u32Key % 10;

      switch(u32Key)
      {
         case 2:
            y -= 3;
            break;
         case 8:
            y += 3;
            break;
         case 4:
            x -= 3;
            break;
         case 6:
            x += 3;
            break;
         default:
         break;
      }

      Display.DrawCircle(x, y, 5, true);
      Display.SetCoursor(3, 0);
      Display.SetFont(&FontSmallNr);
      Display.Print(C8RssiString);
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