#pragma once
#include "uv_k5_display.hpp"
#include "keyboard.hpp"
#include "radio.hpp"
#include "t9.hpp"

template <const System::TOrgFunctions &Fw,
          const System::TOrgData &FwData,
          Radio::CBK4819<Fw> &RadioDriver>
class CMessenger : public Radio::IRadioUser
{
public:
   static constexpr auto MaxCharsInLine = 128 / 8;
   friend class CKeyboard<CMessenger>;

   enum class eState : unsigned char
   {
      InitFirst,
      InitRx,
      InitTx,
      RxDone,
      WaitForRx,
   };

   CMessenger()
       : DisplayBuff(FwData.pDisplayBuffer),
         Display(DisplayBuff),
         Keyboard(*this),
         T9(S8TxBuff),
         bDisplayCleared(true),
         bEnabled(0),
         State(eState::InitRx),
         u8RxDoneLabelCnt(0xFF){};

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
         }

         return;
      }

      if (bDisplayCleared)
      {
      }

      char C8PrintBuff[30];
      bDisplayCleared = false;
      ClearDrawings();

      Display.DrawHLine(3, 3 + 10, 1 * 8 + T9.GetIdx() * 8 + 2);
      // print tx data
      Fw.FormatString(C8PrintBuff, ">%s", T9.C8WorkingBuff);
      Fw.PrintTextOnScreen(C8PrintBuff, 0, 128, 0, 8, 0);

      // print rx data
      char C8Temp = S8RxBuff[MaxCharsInLine];
      S8RxBuff[MaxCharsInLine] = '\0';
      Fw.PrintTextOnScreen(S8RxBuff, 1, 128, 3, 8, 0);
      S8RxBuff[MaxCharsInLine] = C8Temp;
      Fw.PrintTextOnScreen(S8RxBuff + MaxCharsInLine, 1, 128, 5, 8, 0);

      Display.DrawRectangle(0, (8 * 4) - 6, 127, 24 + 6, false);

      if (u8RxDoneLabelCnt < 100)
      {
         u8RxDoneLabelCnt++;
         Fw.PrintTextOnScreen("  >> RX <<   ", 0, 128, 2, 8, 1);
      }

      switch (State)
      {
      case eState::InitRx:
      {
         InitRxHandler();
         break;
      }

      case eState::InitTx:
      {
         static unsigned char u8TxDelay = 0;
         if (u8TxDelay++ >= 1)
         {
            u8TxDelay = 0;
            RadioDriver.SendSyncAirCopyMode72((unsigned char *)S8TxBuff);
            State = eState::InitRx;
         }

         Fw.PrintTextOnScreen("  >> TX <<   ", 0, 128, 2, 8, 1);
         break;
      }
      default:
         break;
      }

      Fw.FlushFramebufferToScreen();
   }

   void RxDoneHandler(unsigned char u8DataLen, bool bCrcOk) override
   {
      bEnabled = true;
      State = eState::InitRx;
      u8RxDoneLabelCnt = 0;
   }

private:
   bool
   FreeToDraw()
   {
      bool bFlashlight = (GPIOC->DATA & GPIO_PIN_3);
      if (bFlashlight)
      {
         bEnabled = true;
         GPIOC->DATA &= ~GPIO_PIN_3;
         *FwData.p8FlashLightStatus = 3;
      }

      if (bEnabled)
      {
         Keyboard.Handle(Fw.PollKeyboard());
      }

      return bEnabled;
   }

   void InitRxHandler()
   {
      RadioDriver.RecieveAsyncAirCopyMode((unsigned char *)S8RxBuff, sizeof(S8RxBuff), this);
      State = eState::WaitForRx;
   }

   void ClearDrawings()
   {
      memset(FwData.pDisplayBuffer, 0, (DisplayBuff.SizeX / 8) * DisplayBuff.SizeY);
   }

   void HandlePressedButton(unsigned char u8Button)
   {
   }

   void HandleReleasedButton(unsigned char u8Button)
   {
      if (u8Button == 10)
      {
         State = eState::InitTx;
         return;
      }

      if (u8Button == 13 && !T9.GetIdx())
      {
         bEnabled = false;
         return;
      }

      T9.ProcessButton(u8Button);
   }

   char S8TxBuff[50];
   char S8RxBuff[72];
   TUV_K5Display DisplayBuff;
   CDisplay<const TUV_K5Display> Display;
   CKeyboard<CMessenger> Keyboard;
   CT9Decoder<sizeof(S8TxBuff)> T9;

   bool bDisplayCleared;
   unsigned char u8LastBtnPressed;
   bool bEnabled;
   eState State;
   unsigned char u8RxDoneLabelCnt;
};