#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "spectrum.hpp"
#include "radio.hpp"
#include <string.h>

extern "C" void __libc_init_array();

Radio::CBK4819 RadioDriver;
CSpectrum<RadioDriver> Spectrum;

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

   static unsigned int u32StupidCounter = 1;
   if((!(u32StupidCounter++ % 15) && u32StupidCounter > 200))
   {
      Spectrum.Handle();
   }
    IRQ_SYSTICK();
}
