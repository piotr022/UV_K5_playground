#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "rssi_sbar.hpp"
#include <string.h>

Hardware::THardware Hw;
const System::TOrgFunctions& Fw = System::OrgFunc_01_26;
const System::TOrgData& FwData = System::OrgData_01_26;

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
   if(!bFirstInit)
   {
      System::CopyDataSection();
      __libc_init_array();
      bFirstInit = true;
   }

   static unsigned int u32StupidCounter = 1;
   if((!(u32StupidCounter++ % 10) && u32StupidCounter > 200)) // exit key
   {
      CRssiPrinter::Handle(Fw, FwData);
   }
    Fw.IRQ_SYSTICK();
}