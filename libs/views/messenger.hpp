#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "t9.hpp"
#include "radio.hpp"
#include "manager.hpp"

template <
    TUV_K5Display &DisplayBuff,
    CDisplay<TUV_K5Display> &Display,
    Radio::CBK4819 &RadioDriver>
class CMessenger : public IView
{
public:
   static constexpr auto MaxCharsInLine = 128 / 8;

   enum class eState : unsigned char
   {
      InitFirst,
      InitRx,
      InitTx,
      RxDone,
      WaitForRx,
   };

   CMessenger()
       : T9(S8TxBuff),
         bDisplayCleared(true),
         bEnabled(0),
         State(eState::InitRx),
         u8RxDoneLabelCnt(0xFF){};

   eScreenRefreshFlag HandleBackground(TViewContext &Context) override
   {
      if (!FreeToDraw())
      {
         if (!bDisplayCleared)
         {
            bDisplayCleared = true;
            ClearDrawings();
            Context.ViewStack.Pop();
            return eScreenRefreshFlag::MainScreen;
         }

         return eScreenRefreshFlag::NoRefresh;
      }

      Context.ViewStack.Push(*this);
      return eScreenRefreshFlag::NoRefresh;
   }

   eScreenRefreshFlag HandleMainView(TViewContext &Context)
   {
      if (Context.OriginalFwStatus.b1RadioSpiCommInUse)
      {
         return eScreenRefreshFlag::NoRefresh;
      }

      ClearDrawingsIfNeeded();
      PrintTxData();
      PrintRxData();
      Display.DrawRectangle(0, (8 * 4) - 6, 127, 24 + 6, false);

      if (u8RxDoneLabelCnt < 100)
      {
         u8RxDoneLabelCnt++;
         PrintTextOnScreen("> RX <", 0, 128, 2, 8, 1);
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
         HandleInitTx();
         break;
      }
      default:
         break;
      }

      return eScreenRefreshFlag::MainScreen;
   }

   void ClearDrawingsIfNeeded()
   {
      if (bDisplayCleared)
      {
         bDisplayCleared = false;
         ClearDrawings();
      }
   }

   void PrintTxData()
   {
      char C8PrintBuff[30];
      FormatString(C8PrintBuff, ">%s", T9.C8WorkingBuff);
      PrintTextOnScreen(C8PrintBuff, 0, 128, 0, 8, 0);
   }

   void PrintRxData()
   {
      // char *tempRxBuff = S8RxBuff + MaxCharsInLine;
      // char C8Temp = tempRxBuff[MaxCharsInLine];
      // S8RxBuff[MaxCharsInLine] = '\0';
      PrintTextOnScreen(S8RxBuff, 1, 128, 3, 8, 0);
      // S8RxBuff[MaxCharsInLine] = C8Temp;
      // PrintTextOnScreen(tempRxBuff, 1, 128, 5, 8, 0);
   }

   void HandleInitTx()
   {
      static unsigned char u8TxDelay = 0;
      if (u8TxDelay++ >= 1)
      {
         u8TxDelay = 0;
         RadioDriver.SendSyncAirCopyMode72((unsigned char *)S8TxBuff);
         State = eState::InitRx;
      }

      PrintTextOnScreen("> TX <", 0, 128, 2, 8, 1);
   }
   void RxDoneHandler(unsigned char u8DataLen, bool bCrcOk)
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
         gFlashLightStatus = 3;
      }

      if (bEnabled)
      {
         // Keyboard.Handle(PollKeyboard());
      }

      return bEnabled;
   }

   void InitRxHandler()
   {
      RadioDriver.RecieveAsyncAirCopyMode((unsigned char *)S8RxBuff, sizeof(S8RxBuff), Radio::CallbackRxDoneType(this, &CMessenger::RxDoneHandler));
      State = eState::WaitForRx;
   }

   void ClearDrawings()
   {
      memset(gDisplayBuffer, 0, (DisplayBuff.SizeX / 8) * DisplayBuff.SizeY);
   }

   void HandlePressedButton(unsigned char u8Button) override
   {
   }

   void HandleReleasedButton(unsigned char u8Button) override
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
   CT9Decoder<sizeof(S8TxBuff)> T9;

   bool bDisplayCleared;
   unsigned char u8LastBtnPressed;
   bool bEnabled;
   eState State;
   unsigned char u8RxDoneLabelCnt;
};
