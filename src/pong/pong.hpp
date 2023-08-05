#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "gpt_pong.hpp"

class CSPong
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   CSPong()
       : DisplayBuff(gDisplayBuffer), FontSmallNr(gSmallDigs), Display(DisplayBuff), x(DisplayBuff.SizeX / 2), y(DisplayBuff.SizeY / 2)
         ,bEnabled(true){};

   void Handle()
   {
      if(!bEnabled)
      {
         return;
      }

      DisplayBuff.ClearAll();
      char C8RssiString[] = "000";

      unsigned int u32Key = PollKeyboard();
      if(u32Key == 13) // exit key
      {
         bEnabled = false;
      }

      C8RssiString[0] = '0' + (u32Key / 100);
      C8RssiString[1] = '0' + (u32Key / 10) % 10;
      C8RssiString[2] = '0' + u32Key % 10;

      Game.handle(u32Key);
      auto const BallPosition = Game.getBallPosition();
      auto const PlatformPosition = Game.getPlatformPosition();
      
      Display.DrawRectangle(BallPosition.x, BallPosition.y, 7, 7, true);
      Display.DrawRectangle(PlatformPosition.x, PlatformPosition.y, 6, 20, true);
      Display.SetCoursor(0, 0);
      Display.SetFont(&FontSmallNr);
      Display.Print(C8RssiString);

      static unsigned int u32Cnt = 0;
      u32Cnt++;
      if((u32Cnt >> 8) % 2)
      {
         unsigned int* p32Buff = (unsigned int*)gDisplayBuffer;
         for(int i = 0; i < (DisplayBuff.SizeX * DisplayBuff.SizeY) / (8*4); i++)
         {
            *p32Buff = ~(*p32Buff);
            p32Buff++;
         }
      }


      FlushFramebufferToScreen();
   }

private:
   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   PongGame Game;

   unsigned char x, y;
   bool bEnabled;
};
