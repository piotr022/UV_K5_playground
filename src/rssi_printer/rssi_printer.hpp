#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

class CRssiPrinter
{
   public:
   static constexpr auto ChartStartX = 4*7 + 4;
   static void Handle(void)
   {
      static bool bIsCleared = true;
      static unsigned char u8ChartPosition = 0;
      static unsigned char u8SqlDelayCnt = 0xFF;
      static unsigned char U8ScreenHistory[128 - ChartStartX] = {0};
      TUV_K5Display DisplayBuff(gDisplayBuffer);
      const TUV_K5SmallNumbers FontSmallNr(gSmallDigs);
      CDisplay Display(DisplayBuff);

      if(!(GPIOC->DATA & 0b1))
      {
         return;
      }

      auto* pMenuCheckData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(2, 6*8 + 1));
      if(BK4819Read(0x0C) & 0b10)
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
            memset(U8ScreenHistory, 0, sizeof(U8ScreenHistory));
            u8ChartPosition = 0;
            if(*pMenuCheckData != 0xFF)
            {
               FlushFramebufferToScreen();
            }
         }

         return;
      }

      u8SqlDelayCnt++;
      bIsCleared = false;
      
      Display.SetCoursor(3, 0);
      Display.SetFont(&FontSmallNr);

      char C8RssiString[] = "g000";
      unsigned char u8Rssi = ((BK4819Read(0x67) >> 1) & 0xFF);
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

      if(u8ChartPosition < ChartStartX || u8ChartPosition >= TUV_K5Display::SizeX)
         u8ChartPosition = ChartStartX;

      unsigned char u8Sub = (u8Rssi * 7) >> 7;
      unsigned char u8PrintShift = (u8Sub > 7 ? 7 : u8Sub);
      // Turn the proper pixel on, and the ones below it on as well
      // The code to turn just the correct pixel on is: U8ScreenHistory[u8ChartPosition - ChartStartX] = (1 << u8PrintShift) & 0xFF;
      U8ScreenHistory[u8ChartPosition - ChartStartX] = ~(0xFF >> (7 - u8PrintShift));

      if(u8ChartPosition + 4 < DisplayBuff.SizeX)
      {
         for(unsigned char i = 0; i < 4; i++)
         {
            U8ScreenHistory[u8ChartPosition - ChartStartX + i + 1] = 0;
         }
      }

      auto* pDData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0) + ChartStartX);
      memcpy(pDData, U8ScreenHistory, sizeof(U8ScreenHistory));

      u8ChartPosition++;
      FlushFramebufferToScreen();
   }
};
