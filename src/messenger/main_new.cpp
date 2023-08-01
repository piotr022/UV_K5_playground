#include "system.hpp"
#include "uv_k5_display.hpp"
#include "messenger.hpp"
#include "radio.hpp"
#include "rssi_sbar.hpp"
#include "manager.hpp"

TUV_K5Display DisplayBuff(gDisplayBuffer);
const TUV_K5SmallNumbers FontSmallNr(gSmallDigs);
CDisplay Display(DisplayBuff);

Radio::CBK4819 RadioDriver;
CMessenger<
    DisplayBuff,
    Display,
    RadioDriver>
    Messenger;

CRssiSbar<
    DisplayBuff,
    Display,
    FontSmallNr>
    RssiSbar;

static IView * const Views[] = {&Messenger, &RssiSbar};
CViewManager<
    16, 2, sizeof(Views) / sizeof(*Views)>
    Manager(Views);

int main()
{
   Fw.IRQ_RESET();
   return 0;
}

extern "C" void Reset_Handler()
{
   Fw.IRQ_RESET();
}

extern "C" void SysTick_Handler()
{
   static bool bFirstInit = false;
   if (!bFirstInit)
   {
      System::CopyDataSection();
      __libc_init_array();
      bFirstInit = true;
   }

   RadioDriver.InterruptHandler();
   Manager.Handle();
   Fw.IRQ_SYSTICK();
}
