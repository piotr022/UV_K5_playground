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

class CSpectrum
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   static constexpr unsigned char EnableKey = 13;
   static constexpr auto DrawingSize = TUV_K5Display::SizeX * 2;
   static constexpr auto DrawingSizeY = 16 + 4 * 8;
   static constexpr auto DrawingEndY = 57;
   static constexpr auto LabelsCnt = 4;
   static constexpr auto PressDuration = 30;
   static constexpr auto BWStep = 200_KHz;
   CSpectrum()
       : DisplayBuff(gDisplayBuffer),
         FontSmallNr(gSmallDigs),
         Display(DisplayBuff),
         bDisplayCleared(true),
         u32ScanRange(1_MHz),
         u8PressCnt(0),
         u8ResolutionDiv(1),
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
            FlushFramebufferToScreen();
            SetFrequency(u32OldFreq);
            BK4819Write(0x47, u16OldAfSettings); // set previous AF settings
         }

         return;
      }

      if (bDisplayCleared)
      {
         u32OldFreq = GetFrequency();
         u16OldAfSettings = BK4819Read(0x47);
         BK4819Write(0x47, 0); // mute AF during scan
      }

      bDisplayCleared = false;
      switch (u8LastBtnPressed)
      {
      case 11: // up
         u32OldFreq += 100_KHz;
         break;

      case 12: // down
         u32OldFreq -= 100_KHz;
         break;

      case 2: // bw up
         bChangingSettings = true;
         u32ScanRange += BWStep;
         break;

      case 8: // bw down
         bChangingSettings = true;
         u32ScanRange -= u32ScanRange > BWStep ? BWStep : 0;
         break;

      case 1: // res up
         u8ResolutionDiv++;
         break;

      case 7: // res down
         if (u8ResolutionDiv)
            u8ResolutionDiv--;
         break;

      default:
         bChangingSettings = false;
         break;
      }

      ClearDrawings();

      unsigned int u32Peak;
      unsigned char u8MaxRssi = 0;
      unsigned char u8PeakPos;
      unsigned char Rssi = 0;
      for (unsigned char u8Pos = 0; u8Pos < DisplayBuff.SizeX; u8Pos++)
      {
         if (!(u8Pos % (DisplayBuff.SizeX / LabelsCnt)) || u8Pos == DisplayBuff.SizeX - 1)
         {
            gDisplayBuffer[2 * DisplayBuff.SizeX + u8Pos] = 0xFF;
         }

         auto const FreqOffset = (u8Pos * u32ScanRange) >> 7;
         if (!u8Pos || (u8Pos & (bChangingSettings ? 2 : u8ResolutionDiv)) == (bChangingSettings ? 2 : u8ResolutionDiv))
         {
            Rssi = GetRssi(u32OldFreq - (u32ScanRange >> 1) + FreqOffset);
         }

         signed char u8Sub = ((Rssi * 250) >> 7) - 20;
         u8Sub = (u8Sub > DrawingSizeY ? DrawingSizeY : u8Sub);
         u8Sub = (u8Sub < 0 ? 0 : u8Sub);
         Display.DrawHLine(DrawingEndY - u8Sub, DrawingEndY, u8Pos);

         if (Rssi > u8MaxRssi)
         {
            u8MaxRssi = Rssi;
            u8PeakPos = u8Pos;
            u32Peak = u32OldFreq - (u32ScanRange >> 1) + FreqOffset;
         }
      }
      // Display.DrawRectangle(0,0, 7, 7, 0);
      memcpy(gDisplayBuffer + 8 * 2 + 10 * 6 + 2, gSmallLeters + 18 + 5, 7);
      Display.SetCoursor(0, 0);
      Display.PrintFixedDigitsNumber2(u32OldFreq);
      Display.SetCoursor(1, 0);
      Display.PrintFixedDigitsNumber2(u32ScanRange, 2);
      // Display.SetCoursor(1, 8 * 2 + 10 * 7);
      // Display.PrintFixedDigitsNumber2(u8ResolutionDiv, 0);

      // Display.DrawRectangle(8*2 + 10*6,0, 7, 7, 0);
      Display.SetCoursor(0, 8 * 2 + 10 * 7);
      Display.PrintFixedDigitsNumber2(u32Peak);

      memcpy(gDisplayBuffer + 128 * 2 + u8PeakPos - 3, gSmallLeters + 18 + 5, 7);
      FlushFramebufferToScreen();
   }

private:
   void SetFrequency(unsigned int u32Freq)
   {
      BK4819Write(0x39, ((u32Freq >> 16) & 0xFFFF));
      BK4819Write(0x38, (u32Freq & 0xFFFF));
      // BK4819Write(0x37,7951);
      BK4819Write(0x30, 0);
      BK4819Write(0x30, 0xbff1);
   }

   unsigned char GetRssi(unsigned int u32Freq)
   {
      SetFrequency(u32Freq);
      DelayUs(800);
      return ((BK4819Read(0x67) >> 1) & 0xFF);
   }

   unsigned int GetFrequency()
   {
      unsigned short u16f1 = BK4819Read(0x39);
      unsigned short u16f2 = BK4819Read(0x38);
      return ((u16f1 << 16) | u16f2);
   }

   bool FreeToDraw()
   {
      bool bFlashlight = (GPIOC->DATA & GPIO_PIN_3);
      if (bFlashlight)
      {
         bEnabled = true;
         GPIOC->DATA &= ~GPIO_PIN_3;
         gFlashLightStatus = 3;
      }

      if (bEnabled)
      {
         u8LastBtnPressed = PollKeyboard();
      }

      bool bPtt = !(GPIOC->DATA & GPIO_PIN_5);
      if (bPtt || u8LastBtnPressed == EnableKey)
      {
         bEnabled = false;
      }

      return bEnabled;
   }

   void ClearDrawings()
   {
      memset(gDisplayBuffer, 0, (DisplayBuff.SizeX / 8) * DisplayBuff.SizeY);
   }

   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   bool bDisplayCleared;
   bool bChangingSettings = false;

   unsigned int u32ScanRange;
   unsigned int u32OldFreq;
   unsigned short u16OldAfSettings;
   unsigned char u8PressCnt;
   unsigned char u8LastBtnPressed;
   unsigned char u8ResolutionDiv;
   bool bEnabled;
   char C8Info[20];
};
