#pragma once
#include "views.hpp"
#include "keyboard.hpp"

struct IMenuElement
{
   virtual const char *GetLabel() = 0;
   virtual void HandleUserAction(unsigned char u8Button) = 0;
};

template <const System::TOrgFunctions &Fw,
          const System::TOrgData &FwData>
class CMenu : public IView
{
   static constexpr auto MenuLines = 3;
   IMenuElement* const* MenuElements;
   const unsigned char ElementsCnt;
   unsigned char u8Selected = 0;

public:
   template<unsigned char ArrSize>
   constexpr CMenu(IMenuElement* const (&_MenuElemets)[ArrSize])
   :MenuElements(_MenuElemets), ElementsCnt(ArrSize)
   {
   }

   eScreenRefreshFlag HandleMainView(TViewContext &Context) override
   {
      DrawMenu();
      return eScreenRefreshFlag::MainScreen;
   }

   eScreenRefreshFlag HandleBackground(TViewContext &Context) override
   {
      if(CheckForFlashlight())
      {
         if(Context.ViewStack.GetTop() == this)
         {
            Context.ViewStack.Pop();
         }
         else
         {
            Context.ViewStack.Push(*this);
         }
      }

      return eScreenRefreshFlag::NoRefresh;
   }

   void HandlePressedButton(TViewContext &Context, unsigned char u8Key) override
   {
      switch (u8Key)
      {
      case Button::Exit:
         Context.ViewStack.Pop();
         break;

      case Button::Down:
         if (u8Selected < ElementsCnt - 1)
            u8Selected++;
         break;

      case Button::Up:
         if (u8Selected)
            u8Selected--;
         break;

      default:
         MenuElements[u8Selected]->HandleUserAction(u8Key);
         break;
      }
   }

   void DrawMenu()
   {
      memset(FwData.pDisplayBuffer, 0, 128*6);
      for(unsigned char u8Line = 0; u8Line < MenuLines; u8Line++)
      {
         auto* Element = GetElement(u8Selected - 1 + u8Line);
         if(!Element)
            continue;

         Fw.PrintTextOnScreen(Element->GetLabel(), 0, 127, u8Line << 1, 8, 0);
      }

      for(auto i = 0; i < 256; i++)
         FwData.pDisplayBuffer[i + 2*128] ^= 0xFF; 
   }

   IMenuElement* GetElement(signed char s8Idx)
   {
      if(s8Idx >= 0 && s8Idx < ElementsCnt)
      {
         return MenuElements[s8Idx];
      }

      return nullptr;
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
};