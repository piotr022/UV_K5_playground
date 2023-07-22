#pragma once

enum eScreenRefreshFlag : unsigned char
{
   NoRefresh = 0,
   StatusBar = 0b01, 
   MainScreen = 0b10, 
};

class CViewStack;
struct TViewContext
{
   CViewStack& ViewStack;
   unsigned int u32SystemCounter;
   union
   {
      unsigned char u8Status;
      struct
      {
         unsigned char b1MenuDrawed : 1;
         unsigned char b1LockScreen : 1;
         unsigned char b1FrequencyScan : 1;
         unsigned char b1RadioSpiCommInUse : 1;
         unsigned char b1LcdSpiCommInUse : 1;
      };
   }OriginalFwStatus;
};

struct IView
{
   IView* pNext;
   // only called by manager only when on top of view stack
   virtual eScreenRefreshFlag HandleMainView(TViewContext& Context) {return eScreenRefreshFlag::NoRefresh;} 

   // always called, can be used to update some notifications on status bar
   virtual eScreenRefreshFlag HandleBackground(TViewContext& Context) {return eScreenRefreshFlag::NoRefresh;} 

   virtual void HandlePressedButton(TViewContext& Context, unsigned char u8Key) {};
   virtual void HandleReleasedButton(TViewContext& Context, unsigned char u8Key) {};
};

class CViewStack
{
   IView* pTop;

public:
   CViewStack():pTop(nullptr){};
   void Push(IView& View)
   {
      View.pNext = pTop;
      pTop = &View;
   }

   IView* Pop()
   {
      auto* const pPopek = pTop;
      if(pTop)
      {
         pTop = pTop->pNext;
         pPopek->pNext = nullptr;
      }

      return pPopek;
   }

   IView* GetTop()
   {
      return pTop;
   }
};
