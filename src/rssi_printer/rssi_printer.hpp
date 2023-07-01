#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

class CRssiPrinter
{
   public:
   static constexpr auto ChartStartX = 4*7 + 4;
   static void Handle(const System::TOrgFunctions& Fw, const System::TOrgData& FwData)
   {
      static bool bIsCleared = false;
      static unsigned char u8ChartPosition = 0;
      TUV_K5Display DisplayBuff(FwData.pDisplayBuffer);
      const TUV_K5SmallNumbers FontSmallNr(FwData.pSmallDigs);
      CDisplay Display(DisplayBuff);

      auto* pMenuCheckData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, ChartStartX));
      bool bIsSqlOpen = (Fw.BK4819Read(0x0C) & 0b10);
      if(!bIsSqlOpen || *pMenuCheckData == 0xFF)
      {
         if(!bIsCleared)
         {
            bIsCleared = true;
            u8ChartPosition = ChartStartX;
            auto* pDData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0));
            memset(pDData, 0, DisplayBuff.SizeX);
            Fw.FlushFramebufferToScreen();
         }

         return;
      }
      
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

      if(u8ChartPosition < ChartStartX || u8ChartPosition >= TUV_K5Display::SizeX)
         u8ChartPosition = ChartStartX;

      unsigned char u8Sub = (u8Rssi * 7) >> 7;
      unsigned char u8PrintShift = (u8Sub > 7 ? 7 : u8Sub);
      auto* pDData = (unsigned char*)DisplayBuff.GetCoursorData(DisplayBuff.GetCoursorPosition(3, 0) + u8ChartPosition);
      *pDData = (1 << u8PrintShift) & 0xFF;
      if(u8ChartPosition + 4 < DisplayBuff.SizeX)
      {
         for(unsigned char i = 0; i < 4; i++)
         {
            *(++pDData) = 0;
         }
      }
      u8ChartPosition++;
      Fw.FlushFramebufferToScreen();
   }
};