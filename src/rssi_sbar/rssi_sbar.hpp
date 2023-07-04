#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

class CRssiPrinter
{
   public:
   static constexpr auto ChartStartX = 32;
   static constexpr auto BlockSizeX = 4;
   static constexpr auto BlockSizeY = 7;
   static constexpr auto BlockSpace = 1;
   static constexpr auto BlocksCnt = (128 - ChartStartX) / (BlockSizeX + BlockSpace);
   static void Handle(const System::TOrgFunctions& Fw, const System::TOrgData& FwData)
   {
      static bool bIsCleared = true;
      static unsigned char u8SqlDelayCnt = 0xFF;

      TUV_K5Display DisplayBuff(FwData.pDisplayBuffer);
      const TUV_K5SmallNumbers FontSmallNr(FwData.pSmallDigs);
      CDisplay Display(DisplayBuff);

      if(!(GPIOC->DATA & 0b1))
      {
         return;
      }

      auto* pMenuCheckData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(2, 6*8 + 1));
      if(Fw.BK4819Read(0x0C) & 0b10)
      {
         u8SqlDelayCnt = 0;
      }

      if(u8SqlDelayCnt > 20 || *pMenuCheckData == 0xFF)
      {
         if(!bIsCleared)
         {
            bIsCleared = true;
            auto* pDData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0));
            memset(pDData, 0, DisplayBuff.SizeX);
            if(*pMenuCheckData != 0xFF)
            {
               Fw.FlushFramebufferToScreen();
            }
         }

         return;
      }

      u8SqlDelayCnt++;
      bIsCleared = false;

      auto* pDData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0));
      memset(pDData, 0, DisplayBuff.SizeX);
      
      Display.SetCoursor(3, 0);
      Display.SetFont(&FontSmallNr);

      char C8RssiString[] = "g000";
      unsigned char u8Rssi = ((Fw.BK4819Read(0x67) >> 1) & 0xFF);
      if(!u8Rssi)
      {
         return;
      }

      if(u8Rssi > 160)
      {
         u8Rssi -= 160;
         C8RssiString[0] = ' ';
      }
      else
      {
         u8Rssi = 160 - u8Rssi;
         C8RssiString[0] = '-';
      }

      C8RssiString[1] = '0' + u8Rssi / 100;
      C8RssiString[2] = '0' + (u8Rssi / 10) % 10;
      C8RssiString[3] = '0' + u8Rssi % 10;
      
      Display.Print(C8RssiString);
      unsigned char u8Sub = (u8Rssi * BlocksCnt) >> 7;
      unsigned char u8BlocksToFill = (u8Sub > BlocksCnt ? BlocksCnt : u8Sub);
      u8BlocksToFill = BlocksCnt - u8BlocksToFill;
      for(unsigned char i = 0; i < u8BlocksToFill; i++)
      {
         unsigned char u8X = i * (BlockSizeX + BlockSpace) + ChartStartX;
         Display.DrawRectangle(u8X, 24, BlockSizeX, BlockSizeY, true);
      }
      // Turn the proper pixel on, and the ones below it on as well
      // The code to turn just the correct pixel on is: U8ScreenHistory[u8ChartPosition - ChartStartX] = (1 << u8PrintShift) & 0xFF;
      // U8ScreenHistory[u8ChartPosition - ChartStartX] = ~(0xFF >> (7 - u8PrintShift));

      Fw.FlushFramebufferToScreen();
   }
};