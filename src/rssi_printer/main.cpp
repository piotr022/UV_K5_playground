#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "rssi_printer.hpp"
#include <string.h>

Hardware::THardware Hw;

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
   if(!bFirstInit)
   {
      System::CopyDataSection();
      __libc_init_array();
      bFirstInit = true;
   }

   static unsigned int u32StupidCounter = 1;
   if((!(u32StupidCounter++ % 15) && u32StupidCounter > 200)) // exit key
   {
      CRssiPrinter::Handle();
   }
    IRQ_SYSTICK();
}
