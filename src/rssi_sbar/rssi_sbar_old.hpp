#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

namespace Rssi
{
   inline const unsigned char U8RssiMap[] =
       {
           141,
           135,
           129,
           123,
           117,
           111,
           105,
           99,
           93,
           83,
           73,
           63,
           53,
   };

   struct TRssi
   {
      TRssi(unsigned char u8Val)
      {
         unsigned char i;
         for (i = 0; i < sizeof(U8RssiMap); i++)
         {
            if (u8Val >= U8RssiMap[i])
            {
               u8SValue = i + 1;
               return;
            }
         }
         u8SValue = i + 1;
      }

      unsigned char u8SValue;
   };
}

class CRssiPrinter
{
public:
   static constexpr auto ChartStartX = 5 * 7 + 8 + 3 * 7; // 32;
   static constexpr auto BlockSizeX = 4;
   static constexpr auto BlockSizeY = 7;
   static constexpr auto BlockSpace = 1;
   static constexpr auto BlocksCnt = (128 - ChartStartX) / (BlockSizeX + BlockSpace);
   static constexpr auto LinearBlocksCnt = 9;
   static constexpr auto VoltageOffset = 77;
   static void Handle(const System::TOrgFunctions &Fw, const System::TOrgData &FwData)
   {
      static bool bIsCleared = true;
      static unsigned char u8SqlDelayCnt = 0xFF;

      TUV_K5Display DisplayBuff(FwData.pDisplayBuffer);
      TUV_K5Display StatusBarBuff(FwData.pStatusBarData);
      const TUV_K5SmallNumbers FontSmallNr(FwData.pSmallDigs);
      CDisplay Display(DisplayBuff);
      CDisplay DisplayStatusBar(StatusBarBuff);
      DisplayStatusBar.SetFont(&FontSmallNr);

      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      static unsigned int u32DrawVoltagePsc = 0;
      if (u32DrawVoltagePsc++ % 16)
      {
         memset(FwData.pStatusBarData + VoltageOffset, 0, 4 * 5);
         DisplayStatusBar.SetCoursor(0, VoltageOffset);
         DisplayStatusBar.PrintFixedDigitsNumber2(*FwData.p16Voltage, 2, 1);
         memset(FwData.pStatusBarData + VoltageOffset + 7 + 1, 0b1100000, 2); // dot
         DisplayStatusBar.SetCoursor(0, VoltageOffset + 7 + 4);
         DisplayStatusBar.PrintFixedDigitsNumber2(*FwData.p16Voltage, 0, 2);
         memcpy(FwData.pStatusBarData + VoltageOffset + 4 * 6 + 2, FwData.pSmallLeters + 128 * 2 + 102, 5); // V character
         Fw.FlushStatusbarBufferToScreen();
      }

      if (Fw.BK4819Read(0x0C) & 0b10)
      {
         u8SqlDelayCnt = 0;
      }

      auto *pMenuCheckData = (unsigned char *)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(2, 6 * 8 + 1));
      auto *pFrequencyScanCheckData = (unsigned char *)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(6, 3 * 8 +2));
      auto *pDData = (unsigned char *)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0));
      if (u8SqlDelayCnt > 20 || *pMenuCheckData == 0xFF || *pFrequencyScanCheckData)
      {
         if (!bIsCleared)
         {
            bIsCleared = true;
            memset(pDData, 0, DisplayBuff.SizeX);
            if (*pMenuCheckData != 0xFF || *pFrequencyScanCheckData)
            {
               Fw.FlushFramebufferToScreen();
            }
         }

         return;
      }

      u8SqlDelayCnt++;
      bIsCleared = false;

      memset(pDData, 0, DisplayBuff.SizeX);

      Display.SetCoursor(3, 0);
      Display.SetFont(&FontSmallNr);

      char C8RssiString[] = "g000";
      unsigned char u8Rssi = ((Fw.BK4819Read(0x67) >> 1) & 0xFF);
      if (!u8Rssi)
      {
         return;
      }

      if (u8Rssi > 160)
      {
         u8Rssi -= 160;
      }
      else
      {
         u8Rssi = 160 - u8Rssi;
         C8RssiString[0] = '-';
      }

      u8Rssi += 10;
      unsigned char u8RssiCpy = u8Rssi;
      unsigned char hundreds = 0;
      while (u8RssiCpy >= 100)
      {
         hundreds++;
         u8RssiCpy -= 100;
      }

      unsigned char tens = 0;
      while (u8RssiCpy >= 10)
      {
         tens++;
         u8RssiCpy -= 10;
      }

      C8RssiString[1] = '0' + hundreds;
      C8RssiString[2] = '0' + tens;
      C8RssiString[3] = '0' + u8RssiCpy;
      Display.Print(C8RssiString);

      unsigned char u8Sub = (u8Rssi * BlocksCnt) >> 7;
      unsigned char u8BlocksToFill = (u8Sub > BlocksCnt ? BlocksCnt : u8Sub);
      u8BlocksToFill = Rssi::TRssi(u8Rssi).u8SValue;

      char C8SignalString[] = "  ";

      if (u8BlocksToFill > 9)
      {
         memcpy(pDData + 5 * 7, FwData.pSmallLeters + 109 - 3 * 8, 8);
         C8SignalString[1] = '0';
         C8SignalString[0] = '0' + u8BlocksToFill - 9;
      }
      else
      {
         memcpy(pDData + 5 * 7, FwData.pSmallLeters + 109, 8);
         C8SignalString[0] = '0' + u8BlocksToFill;
         C8SignalString[1] = ' ';
      }

      Display.SetCoursor(3, 5 * 7 + 8);
      Display.Print(C8SignalString);

      u8BlocksToFill = u8BlocksToFill > 13 ? 13 : u8BlocksToFill;
      for (unsigned char i = 0; i < u8BlocksToFill; i++)
      {
         unsigned char u8BlockHeight = i + 1 > BlockSizeY ? BlockSizeY : i + 1;
         unsigned char u8X = i * (BlockSizeX + BlockSpace) + ChartStartX;
         Display.DrawRectangle(u8X, 24 + BlockSizeY - u8BlockHeight, BlockSizeX,
                               u8BlockHeight, i < LinearBlocksCnt);
      }

      Fw.FlushFramebufferToScreen();
   }
};