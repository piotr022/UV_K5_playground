#pragma once
#include "views.hpp"
#include "system.hpp"
#include "keyboard.hpp"
#include "registers.hpp"

template <const System::TOrgFunctions &Fw, 
   const System::TOrgData &FwData, 
   unsigned char BackgroundViewPrescaler,
   unsigned char MainViewPrescaler,
   unsigned char RegisteredViews>
class CViewManager
{
   static constexpr auto ManagerStartupDelay = 200;
   friend class CKeyboard<CViewManager>;

   IView* const (&Modules)[RegisteredViews];
   CViewStack MainViewStack;
   TViewContext MainViewContext;
   CKeyboard<CViewManager> Keyboard;

   public:
   constexpr CViewManager(IView* const (&_Modules)[RegisteredViews])
      :  Modules(_Modules),
         MainViewContext({MainViewStack, 0, 0}),
         Keyboard(*this)
         {
         };
   
   void Handle()
   {
      MainViewContext.u32SystemCounter++;
      if(MainViewContext.u32SystemCounter < ManagerStartupDelay)
      {
         return;
      }

      CheckOriginalFwStatus();

      unsigned char u8ScreenRefreshFlag = 0;
      if(!(MainViewContext.u32SystemCounter % BackgroundViewPrescaler))
      {
         u8ScreenRefreshFlag |= HandleBackgroundTasks();
      }

      auto* pViewStackTop = MainViewStack.GetTop();
      if(pViewStackTop && !(MainViewContext.u32SystemCounter 
                              % MainViewPrescaler))
      {
         if(!MainViewContext.OriginalFwStatus.b1RadioSpiCommInUse)
            Keyboard.Handle(Fw.PollKeyboard());

         u8ScreenRefreshFlag |= 
            pViewStackTop->HandleMainView(MainViewContext);
      }

      if(u8ScreenRefreshFlag & eScreenRefreshFlag::MainScreen)
      {
         Fw.FlushFramebufferToScreen();
      }

      if(u8ScreenRefreshFlag & eScreenRefreshFlag::StatusBar)
      {
         Fw.FlushStatusbarBufferToScreen();
      }
   }

   private:
   inline unsigned char HandleBackgroundTasks()
   {
      unsigned char u8ScreenRefreshFlag = 0;
      for(auto& pModule : Modules)
      {
         if(!pModule)
         {
            continue;
         }

         u8ScreenRefreshFlag |= pModule->HandleBackground(MainViewContext);
      }

      return u8ScreenRefreshFlag;
   }

   inline void CheckOriginalFwStatus()
   {
      const auto *pMenuCheckData = (unsigned char *)(FwData.pDisplayBuffer + 2*128 + 6 * 8 + 1);
      const auto *pFrequencyScanCheckData = (unsigned char *)(FwData.pDisplayBuffer + 6*128 + 3 * 8 + 2);
      MainViewContext.OriginalFwStatus.b1MenuDrawed = *pMenuCheckData == 0xFF;
      MainViewContext.OriginalFwStatus.b1FrequencyScan = *pFrequencyScanCheckData;
      MainViewContext.OriginalFwStatus.b1RadioSpiCommInUse = !(GPIOC->DATA & 0b1);
      MainViewContext.OriginalFwStatus.b1LcdSpiCommInUse = !(GPIOB->DATA & GPIO_PIN_9);
   }

   void HandlePressedButton(unsigned char u8Key)
   {
      auto* const pTop = MainViewStack.GetTop();
      if(pTop)
      {
         pTop->HandlePressedButton(MainViewContext, u8Key);
      }
   }

   void HandleReleasedButton(unsigned char u8Key)
   {
      auto* const pTop = MainViewStack.GetTop();
      if(pTop)
      {
         pTop->HandleReleasedButton(MainViewContext, u8Key);
      }
   }
};