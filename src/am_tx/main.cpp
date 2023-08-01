#include "system.hpp"
#include "uv_k5_display.hpp"
#include "messenger.hpp"
#include "radio.hpp"
#include "am_tx.hpp"
#include "rssi_sbar.hpp"
#include "manager.hpp"

const System::TOrgFunctions &Fw = System::OrgFunc_01_26;
const System::TOrgData &FwData = System::OrgData_01_26;

TUV_K5Display DisplayBuff(FwData.pDisplayBuffer);
const TUV_K5SmallNumbers FontSmallNr(FwData.pSmallDigs);
CDisplay Display(DisplayBuff);

TUV_K5Display StatusBarBuff(FwData.pStatusBarData);
CDisplay DisplayStatusBar(StatusBarBuff);

Radio::CBK4819<System::OrgFunc_01_26> RadioDriver;

// CRssiSbar<
//     System::OrgFunc_01_26,
//     System::OrgData_01_26,
//     DisplayBuff,
//     Display,
//     DisplayStatusBar,
//     FontSmallNr,
//     RadioDriver>
//     RssiSbar;
    
CAmTx<
    System::OrgFunc_01_26,
    System::OrgData_01_26,
    DisplayBuff,
    Display,
    DisplayStatusBar,
    FontSmallNr,
    RadioDriver>
    AmTx;

static IView * const Views[] = {&AmTx};
CViewManager<
    System::OrgFunc_01_26,
    System::OrgData_01_26,
    8, 1, sizeof(Views) / sizeof(*Views)>
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

   Manager.Handle();
   Fw.IRQ_SYSTICK();
}