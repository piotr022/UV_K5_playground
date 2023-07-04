#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "gpt_pong.hpp"

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData>
class CSPong
{
public:
   static constexpr auto StepSize = 0xFFFF / TUV_K5Display::SizeX;
   static constexpr auto StepSizeFreq = 10'000;
   CSPong()
       : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs), Display(DisplayBuff), x(DisplayBuff.SizeX / 2), y(DisplayBuff.SizeY / 2){};

   void Handle()
   {
      DisplayBuff.ClearAll();
      char C8RssiString[] = "000";

      unsigned int u32Key = Fw.PollKeyboard();

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
      Fw.FlushFramebufferToScreen();
   }

private:
   TUV_K5Display DisplayBuff;
   const TUV_K5SmallNumbers FontSmallNr;
   CDisplay<const TUV_K5Display> Display;
   PongGame Game;

   unsigned char x, y;
};