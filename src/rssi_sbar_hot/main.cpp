#include "system.hpp"
#include "uv_k5_display.hpp"
#include "messenger.hpp"
#include "radio.hpp"
#include "am_tx.hpp"
#include "rssi_sbar.hpp"
#include "manager.hpp"
#include "heater.hpp"

TUV_K5Display DisplayBuff(gDisplayBuffer);
const TUV_K5SmallNumbers FontSmallNr(gSmallDigs);
CDisplay Display(DisplayBuff);

TUV_K5Display StatusBarBuff(gStatusBarData);
CDisplay DisplayStatusBar(StatusBarBuff);

Radio::CBK4819 RadioDriver;

CRssiSbar<
    DisplayBuff,
    Display,
    DisplayStatusBar,
    FontSmallNr,
    RadioDriver>
    RssiSbar;
// CAmTx<
//     DisplayBuff,
//     Display,
//     DisplayStatusBar,
//     FontSmallNr,
//     RadioDriver>
//     AmTx;

CHeater Heater;
CAmRx AmRx;
// CMicVal<RadioDriver> MicVal;
// CRssiVal<RadioDriver> RssiVal;

static IMenuElement * const MainMenuElements[] = {&Heater, &AmRx, &RssiSbar};

CMenu Menu(MainMenuElements);

static IView * const Views[] = {&RssiSbar, &Menu};
CViewManager<
    8, 1, sizeof(Views) / sizeof(*Views)>
    Manager(Views);

int main()
{
   IRQ_RESET();
   return 0;
}

extern "C" void Reset_Handler()
{
   IRQ_RESET();
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

   // RadioDriver.InterruptHandler();
   Manager.Handle();
   IRQ_SYSTICK();
}
