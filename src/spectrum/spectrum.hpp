#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

#define REVERSE_UINT16(x) ((((x)&0xFF00) >> 8) | (((x)&0x00FF) << 8))

static constexpr auto operator""_Hz(unsigned long long Hertz)
{
   return Hertz / 10;
}

static constexpr auto operator""_KHz(unsigned long long KiloHertz)
{
   return KiloHertz * 1000_Hz;
}

static constexpr auto operator""_MHz(unsigned long long KiloHertz)
{
   return KiloHertz * 1000_KHz;
}

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData>
class CSpectrum
{
public:
   static constexpr auto ScanRange = 2_MHz;
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   static constexpr unsigned char EnableKey = 13;
   static constexpr auto DrawingSize = TUV_K5Display::SizeX * 2;
   static constexpr auto DrawingSizeY = 16 + 4 * 8;
   static constexpr auto DrawingEndY = 57;
   static constexpr auto LabelsCnt = 6;
   static constexpr auto PressDuration = 30;
   CSpectrum()
       : DisplayBuff(FwData.pDisplayBuffer),
         FontSmallNr(FwData.pSmallDigs),
         Display(DisplayBuff),
         bDisplayCleared(true),
         u8PressCnt(0),
         bEnabled(0)
   {
      Display.SetFont(&FontSmallNr);
   };

   void Handle()
   {
      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      if (!FreeToDraw())
      {
         if (!bDisplayCleared)
         {
            bDisplayCleared = true;
            ClearDrawings();
            Fw.FlushFramebufferToScreen();
            SetFrequency(u32OldFreq);
            Fw.BK4819Write(0x47, u16OldAfSettings); // set previous AF settings
         }

         return;
      }

      if (bDisplayCleared)
      {
         u32OldFreq = GetFrequency();
         u16OldAfSettings = Fw.BK4819Read(0x47);
         Fw.BK4819Write(0x47, 0); // mute AF during scan
      }

      bDisplayCleared = false;
      if (u8LastBtnPressed == 11) // up
      {
         u32OldFreq += 100_KHz;
      }
      else if (u8LastBtnPressed == 12) // down
      {
         u32OldFreq -= 100_KHz;
      }

      ClearDrawings();

      unsigned int u32Peak;
      unsigned char u8MaxRssi = 0;
      unsigned char u8PeakPos;
      for (unsigned char u8Pos = 0; u8Pos < DisplayBuff.SizeX; u8Pos++)
      {
         if (!(u8Pos % (DisplayBuff.SizeX / LabelsCnt)))
         {
            Display.DrawHLine(10, 15, u8Pos);
         }

         auto const FreqOffset = (u8Pos * ScanRange) >> 7;
         auto const Rssi = GetRssi(u32OldFreq - (ScanRange >> 1) + FreqOffset);
         signed char u8Sub = ((Rssi * 200) >> 7) - 20;
         u8Sub = (u8Sub > DrawingSizeY ? DrawingSizeY : u8Sub);
         u8Sub = (u8Sub < 0 ? 0 : u8Sub);
         Display.DrawHLine(DrawingEndY - u8Sub, DrawingEndY, u8Pos);

         if (Rssi > u8MaxRssi)
         {
            u8MaxRssi = Rssi;
            u8PeakPos = u8Pos;
            u32Peak = u32OldFreq - (ScanRange >> 1) + FreqOffset;
         }
      }

      // Display.DrawRectangle(0,0, 7, 7, 0);
      memcpy(FwData.pDisplayBuffer + 8 * 2 + 10 * 6, FwData.pSmallLeters + 18 + 5, 7);
      Display.SetCoursor(0, 0);
      Display.PrintFixedDigitsNumber2(u32OldFreq);

      // Display.DrawRectangle(8*2 + 10*6,0, 7, 7, 0);
      Display.SetCoursor(0, 8 * 2 + 10 * 7);
      Display.PrintFixedDigitsNumber2(u32Peak);

      memcpy(FwData.pDisplayBuffer + 128 * 2 + u8PeakPos - 3, FwData.pSmallLeters + 18 + 5, 7);

      Fw.FlushFramebufferToScreen();
   }

private:
   void SetFrequency(unsigned int u32Freq)
   {
      Fw.BK4819Write(0x39, ((u32Freq >> 16) & 0xFFFF));
      Fw.BK4819Write(0x38, (u32Freq & 0xFFFF));
      // Fw.BK4819Write(0x37,7951);
      Fw.BK4819Write(0x30, 0);
      Fw.BK4819Write(0x30, 0xbff1);
   }

   unsigned char GetRssi(unsigned int u32Freq)
   {
      SetFrequency(u32Freq);
      Fw.DelayUs(800);
      return ((Fw.BK4819Read(0x67) >> 1) & 0xFF);
   }

   unsigned int GetFrequency()
   {
      unsigned short u16f1 = Fw.BK4819Read(0x39);
      unsigned short u16f2 = Fw.BK4819Read(0x38);
      return ((u16f1 << 16) | u16f2);
   }

   bool FreeToDraw()
   {
      bool bFlashlight = (GPIOC->DATA & GPIO_PIN_3);
      if (bFlashlight)
      {
         bEnabled = true;
         GPIOC->DATA &= ~GPIO_PIN_3;
      }

      bool bPtt = !(GPIOC->DATA & GPIO_PIN_5);
      if(bPtt)
      {
         bEnabled = false;
      }

      if(bEnabled)
      {
         u8LastBtnPressed = Fw.PollKeyboard();
      }

         // u8LastBtnPressed = Fw.PollKeyboard();
         // if (u8LastBtnPressed == EnableKey)
         // {
         //    u8PressCnt++;
         // }

         // if (u8PressCnt > (bEnabled ? 3 : PressDuration))
         // {
         //    u8PressCnt = 0;
         //    bEnabled = !bEnabled;
         // }
      

      return bEnabled;
   }

   void ClearDrawings()
   {
      memset(FwData.pDisplayBuffer, 0, (DisplayBuff.SizeX / 8) * DisplayBuff.SizeY);
   }

   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   bool bDisplayCleared;

   unsigned int u32OldFreq;
   unsigned short u16OldAfSettings;
   unsigned char u8PressCnt;
   bool bEnabled;
   unsigned char u8LastBtnPressed;
};