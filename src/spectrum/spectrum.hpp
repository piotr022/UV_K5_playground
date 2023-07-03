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
       : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs), Display(DisplayBuff){};

   void Handle()
   {
      auto *pUpLineData = (unsigned char *)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(2, 0));
      auto *pDownLineData = (unsigned char *)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0));

      // Fw.BK4819Write(0x43, 0b01 << 4 | 0b001 << 6);
      for (int i = 0; i < TUV_K5Display::SizeX; i++)
      {
         SetFrequency(144'000'000 + i * StepSizeFreq);
         // Fw.BK4819Write(0x39, i * StepSize);
         Fw.DelayUs(1000);
         unsigned char u8Rssi = ((Fw.BK4819Read(0x67) >> 1) & 0xFF);

         if (u8Rssi > 160)
         {
            u8Rssi -= 160;
         }
         else
         {
            u8Rssi = 160 - u8Rssi;
         }

         u8Rssi -= 20;

         unsigned char u8Sub = (u8Rssi * 13) >> 7;
         u8Sub = (u8Sub > 15 ? 15 : u8Sub);
         unsigned short u16ToPrint = (0xFFFF << u8Sub);
         *(pUpLineData + i) = u16ToPrint & 0xFF;
         *(pDownLineData + i) = (u16ToPrint >> 8) & 0xFF;
      }

      // Display.SetCoursor(3, 0);
      // Display.SetFont(&FontSmallNr);
      // Display.Print(C8RssiString);
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
};