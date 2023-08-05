#pragma once
#include "menu.hpp"
#include "system.hpp"
#include "keyboard.hpp"
 
inline char S8Label[20];
class CHeater : public IMenuElement
{
   bool bEnabled = false;

   public:
   const char *GetLabel() override
   {
      if(bEnabled)
         return "Heater    ON";
      
      return "Heater    OFF";
   }

   void HandleUserAction(unsigned char u8Button) override
   {
      if(u8Button != Button::Ok)
      {
         return;
      }

      bEnabled = !bEnabled;
      if(bEnabled)
      {
         BK4819SetGpio(1, true);
      }
      else
      {
         BK4819SetGpio(1, false);
      }
   }
};

class CAmRx : public IMenuElement
{
   unsigned char u8Mode = 0;
   public:
   const char *GetLabel() override
   {
      FormatString(S8Label, "AM RX     %u", u8Mode);
      return S8Label;
   }

   void HandleUserAction(unsigned char u8Button) override
   {
      if(u8Button != Button::Ok)
      {
         return;
      }

      u8Mode++;
      if(u8Mode > 2)
         u8Mode = 0;
      
      // SomeAmStuff(u8Mode);
   }
};

template <Radio::CBK4819 &RadioDriver>
class CMicVal : public IMenuElement
{
   unsigned char u8Mode = 0;

   public:
   const char *GetLabel() override
   {
      FormatString(S8Label, "MIC in    %u", BK4819Read(0x64));
      return S8Label;
   }

   void HandleUserAction(unsigned char u8Button) override
   {
   }
};

template <Radio::CBK4819 &RadioDriver>
class CRssiVal : public IMenuElement
{
   unsigned char u8Mode = 0;
   char S8Label[20];

   public:
   const char *GetLabel() override
   {                         
      FormatString(S8Label, "RSSI     %i", RadioDriver.GetRssi());
      return S8Label;
   }

   void HandleUserAction(unsigned char u8Button) override
   {
   }
};
