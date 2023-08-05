#pragma once
#include "manager.hpp"
#include "registers.hpp"
#include "hardware/adc.hpp"

template <
    TUV_K5Display &DisplayBuff,
    CDisplay<TUV_K5Display> &Display,
    CDisplay<TUV_K5Display> &DisplayStatusBar,
    const TUV_K5SmallNumbers &FontSmallNr,
    Radio::CBK4819 &RadioDriver>
class CAmTx : public IView
{
   static constexpr bool bAmpTests = true;
   bool bAmMode = false;
   bool bEnabled = false;
   unsigned short u16OldAmp = 0;
   unsigned short u16ActualAmp = 0;
   int s32DeltaAmp = 0;
   unsigned int u32StartFreq = 0;
   bool bInit = false;
   // unsigned short U16OldAgcTable[5];
   // unsigned short U16NewAgcTable[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

public:
   eScreenRefreshFlag HandleMainView(TViewContext &Context) override
   {
      if (Context.OriginalFwStatus.b1RadioSpiCommInUse || !RadioDriver.IsTx())
      {
         return eScreenRefreshFlag::NoRefresh;
      }

      if(bInit)
      {
         bInit = false;
         RadioDriver.SetDeviationPresent(0);
         u32StartFreq = RadioDriver.GetFrequency();
      }

      HandleTests();

      if(CheckForPtt())
      {
         return eScreenRefreshFlag::MainScreen;
      }

      RadioDriver.SetFrequency(u32StartFreq);
      RadioDriver.SetDeviationPresent(1);

      Context.ViewStack.Pop();
      return eScreenRefreshFlag::NoRefresh;
   }

   void HandleTests()
   {
      HandleMicInput();
      char S8DebugStr[20];
      // int MicAmp = s32DeltaAmp / 8; // RadioDriver.GetAFAmplitude();
      // MicAmp = 4 + MicAmp;
      // if (MicAmp > 0b111)
      //    MicAmp = 0b111;
      // if (MicAmp < 0)
      //    MicAmp = 0;
      unsigned short U16AdcData[2];
      AdcReadout(U16AdcData, U16AdcData+1);
      FormatString(S8DebugStr, "in 1: %05i   ", U16AdcData[0]);
      PrintTextOnScreen(S8DebugStr, 0, 127, 0, 8, 0);
      FormatString(S8DebugStr, "in 2: %05i   ", U16AdcData[1]);
      PrintTextOnScreen(S8DebugStr, 0, 127, 2, 8, 0);
   }

   void HandleMicInput()
   {
      u16ActualAmp = BK4819Read(0x64);
      // u16ActualAmp = BK4819Read(0x6F) & 0b1111111;
      s32DeltaAmp = u16OldAmp - u16ActualAmp;
      u16OldAmp = u16ActualAmp;
   }

   void HandleTxAm()
   {
      int MicAmp = s32DeltaAmp / 8; // RadioDriver.GetAFAmplitude();
      MicAmp = 4 + MicAmp;
      if (MicAmp > 0b111)
         MicAmp = 0b111;
      if (MicAmp < 0)
         MicAmp = 0;
      BK4819Write(0x36, ((MicAmp & 0b111) << 3) | (MicAmp & 0b111));
   }

   void HandleTxWfm()
   {
      RadioDriver.SetFrequency(u32StartFreq + s32DeltaAmp);
   }

   // always called, can be used to update some notifications on status bar
   eScreenRefreshFlag HandleBackground(TViewContext &Context)
   {
      if (CheckForFlashlight())
      {
         bAmMode = !bAmMode;
         if(!bAmMode)
         {
            DrawAmIcon(false);
            return eScreenRefreshFlag::StatusBar;
         }
      }

      if (!bAmMode)
      {
         return eScreenRefreshFlag::NoRefresh;
      }

      DrawAmIcon(true);

      if (!CheckForPtt())
      {
         return eScreenRefreshFlag::StatusBar;
      }

      bInit = true;
      Context.ViewStack.Push(*this);
      return eScreenRefreshFlag::StatusBar;
   }

   bool CheckForPtt()
   {
      return !(GPIOC->DATA & GPIO_PIN_5);
   }

   bool CheckForFlashlight()
   {
      if (GPIOC->DATA & GPIO_PIN_3)
      {
         GPIOC->DATA &= ~GPIO_PIN_3;
         gFlashLightStatus = 3;
         return true;
      }

      return false;
   }

   void DrawAmIcon(bool bDraw)
   {
      memset(gStatusBarData, 0, 14);
      if (!bDraw)
      {
         return;
      }

      memcpy(gStatusBarData, gSmallLeters + 223, 12);
      unsigned char *pNegative = gStatusBarData;
      for (unsigned char i = 0; i < 14; i++)
      {
         *pNegative++ ^= 0xFF;
      }
   }
};
