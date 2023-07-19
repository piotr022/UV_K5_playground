#include "system.hpp"
#include "uv_k5_display.hpp"
#include "messenger.hpp"
#include "radio.hpp"

const System::TOrgFunctions &Fw = System::OrgFunc_01_26;
const System::TOrgData &FwData = System::OrgData_01_26;

Radio::CBK4819<System::OrgFunc_01_26> RadioDriver;

CMessenger<
    System::OrgFunc_01_26,
    System::OrgData_01_26,
    RadioDriver>
    Messenger;

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
   Messenger.Handle();
   Fw.IRQ_SYSTICK();
}