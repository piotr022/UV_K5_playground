#pragma once
#include "manager.hpp"
#include "registers.hpp"
#include "hardware/adc.hpp"

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

      if (CheckForPtt())
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
      if constexpr(!bAmpTests)
      {
         return;
      }

      // HandleMicInput();
      char S8DebugStr[6 * 40 + 1];
      // static constexpr auto reg = 0x6F;
      DisplayBuff.ClearAll();
      
      Fw.FormatString(S8DebugStr, "%05i  %05i  ", RadioDriver.GetVoiceAmplitude(), RadioDriver.GetAFAmplitude());
      Fw.PrintTextOnScreen(S8DebugStr, 0, 127, 0, 8, 0);
      DrawMicInChart();
   }

   void DrawMicInChart()
   {
      static constexpr auto chartBottomY = 55;
      static constexpr auto chartTopY = 16;
      unsigned char u8LastYpoint;
      for(unsigned short x = 0; x < 128; x++)
      {
         auto ypoint = RadioDriver.GetVoiceAmplitude();
         ypoint >>= 6;
         if(ypoint >= chartBottomY - chartTopY)
         {
            ypoint = chartTopY;
         }
         else
         {
            ypoint = chartBottomY - ypoint;
         }

         if(!x) u8LastYpoint = ypoint;
         DisplayBuff.SetPixel(x, ypoint);
         while (ypoint != u8LastYpoint)
         {
            u8LastYpoint += u8LastYpoint < ypoint ? 1 : -1;
            DisplayBuff.SetPixel(x, u8LastYpoint);
         }

         u8LastYpoint = ypoint;
      }
   }

   void HandleMicInput()
   {
      u16ActualAmp = Fw.BK4819Read(0x64);
      // u16ActualAmp = Fw.BK4819Read(0x6F) & 0b1111111;
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