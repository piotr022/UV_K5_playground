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
   static constexpr auto DrawingSizeY = 16;
   static constexpr auto DrawingEndY = 32;
   static constexpr auto LabelsCnt = 6;
   CSpectrum()
       : DisplayBuff(FwData.pDisplayBuffer),
         FontSmallNr(FwData.pSmallDigs),
         Display(DisplayBuff),
         bDisplayCleared(true){};

   void Handle()
   {
      if ((GPIOA->DATA & ((1 << 0xA) | (1 << 0xB) | (1 << 0xC) | (1 << 0xD))) || !(GPIOC->DATA & 0b1))
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

      ClearDrawings();
      for (unsigned char u8Pos = 0; u8Pos < DisplayBuff.SizeX; u8Pos++)
      {
         if (!(u8Pos % (DisplayBuff.SizeX / LabelsCnt)))
         {
            Display.DrawHLine(16, 18, u8Pos);
         }

         auto const FreqOffset = u8Pos * ScanRange / (DisplayBuff.SizeX - 1);
         auto const Rssi = GetRssi(u32OldFreq - (ScanRange / 2) + FreqOffset);
         signed char u8Sub = ((Rssi * 120) >> 7) - 12;
         u8Sub = (u8Sub > DrawingSizeY ? DrawingSizeY : u8Sub);
         u8Sub = (u8Sub < 0 ? 0 : u8Sub);
         Display.DrawHLine(DrawingEndY - u8Sub, DrawingEndY, u8Pos);
      }

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
      bool bLedKey = GPIOA->DATA & GPIO_PIN_3; // Fw.PollKeyboard(); keyboard polling causes many problems probably it disrupt main fw polling procedure
      auto *pMenuCheckData = (unsigned char *)DisplayBuff.GetCoursorData(
          DisplayBuff.GetCoursorPosition(1, 6 * 8 + 1));

      unsigned char u8Keyboard = 0;
      if (bLedKey)
      {
         u8Keyboard = Fw.PollKeyboard();
      }

      return bLedKey && *pMenuCheckData != 0xFF && u8Keyboard != EnableKey;
   }

   void ClearDrawings()
   {
      auto *p8Drw = (unsigned char *)DisplayBuff.GetCoursorData(
          DisplayBuff.GetCoursorPosition(2, 0));
      memset(p8Drw, 0, DrawingSize);
   }

   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   bool bDisplayCleared;

   unsigned int u32OldFreq;
   unsigned short u16OldAfSettings;
};