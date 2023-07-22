#pragma once
#include "manager.hpp"
#include "registers.hpp"

template <
    const System::TOrgFunctions &Fw,
    const System::TOrgData &FwData,
    TUV_K5Display &DisplayBuff,
    CDisplay<TUV_K5Display> &Display,
    CDisplay<TUV_K5Display> &DisplayStatusBar,
    const TUV_K5SmallNumbers &FontSmallNr,
    Radio::CBK4819<Fw> &RadioDriver>
class CAmTx : public IView
{
   static constexpr bool bAmpTests = true;
   bool bAmMode = false;
   bool bEnabled = false;
   unsigned short u16OldAmp = 0;
   int s32DeltaAmp = 0;
   unsigned int u32StartFreq = 0;
   // unsigned short U16OldAgcTable[5];
   // unsigned short U16NewAgcTable[5] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

public:
   eScreenRefreshFlag HandleMainView(TViewContext &Context) override
   {
      if (Context.OriginalFwStatus.b1RadioSpiCommInUse || !RadioDriver.IsTx())
      {
         return eScreenRefreshFlag::NoRefresh;
      }

      if constexpr (bAmpTests)
      {
         // Fw.BK4819Write(0x29, 0);
         // Fw.BK4819Write(0x2B, 0);
         // Fw.BK4819Write(0x19, 0);
         // RadioDriver.SetDeviationPresent(0);
         HandleTests();
         if (!CheckForPtt())
         {
            RadioDriver.SetDeviationPresent(1);
            Context.ViewStack.Pop();
         }

         return eScreenRefreshFlag::MainScreen;
      }

      RadioDriver.SetDeviationPresent(0);
      auto InitialBias = Fw.BK4819Read(0x36);

      u32StartFreq = RadioDriver.GetFrequency();

      while (CheckForPtt())
      {
         HandleMicInput();
         HandleTxAm();
         //HandleTxWfm();
      }

      RadioDriver.SetFrequency(u32StartFreq);
      RadioDriver.SetDeviationPresent(1);
      Fw.BK4819Write(0x36, InitialBias);

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
      Fw.AdcReadout(U16AdcData, U16AdcData+1);
      Fw.FormatString(S8DebugStr, "in 1: %05i   ", U16AdcData[0]);
      Fw.PrintTextOnScreen(S8DebugStr, 0, 127, 0, 8, 0);
      Fw.FormatString(S8DebugStr, "in 2: %05i   ", U16AdcData[1]);
      Fw.PrintTextOnScreen(S8DebugStr, 0, 127, 2, 8, 0);
   }

   void HandleMicInput()
   {
      unsigned short u16ActualAmp = Fw.BK4819Read(0x64);
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
      Fw.BK4819Write(0x36, ((MicAmp & 0b111) << 3) | (MicAmp & 0b111));
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
         *FwData.p8FlashLightStatus = 3;
         return true;
      }

      return false;
   }

   void DrawAmIcon(bool bDraw)
   {
      memset(FwData.pStatusBarData, 0, 14);
      if (!bDraw)
      {
         return;
      }

      memcpy(FwData.pStatusBarData, FwData.pSmallLeters + 223, 12);
      unsigned char *pNegative = FwData.pStatusBarData;
      for (unsigned char i = 0; i < 14; i++)
      {
         *pNegative++ ^= 0xFF;
      }
   }
};