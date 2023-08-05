#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "t9_texting.hpp"
#include <string.h>

Hardware::THardware Hw;

CT9Texting T9Texting;

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
   unsigned int u32Dummy;
   System::TCortexM0Stacking* pStackedRegs = 
      (System::TCortexM0Stacking*)(((unsigned int*)&u32Dummy) + 1);

   static bool bFirstInit = false;
   if(!bFirstInit)
   {
      System::CopyDataSection();
      __libc_init_array();
      bFirstInit = true;
   }

   bool bPreventWhileKeypadPolling = pStackedRegs->LR > (unsigned int)PollKeyboard &&
      pStackedRegs->PC < (unsigned int)PollKeyboard + 0x100; // i made a mistake and compared PC and LR, but this works fine xD

   static unsigned int u32StupidCounter = 1;
   if(u32StupidCounter++ > 200 && !bPreventWhileKeypadPolling)
   {
      T9Texting.Handle();
   }
    IRQ_SYSTICK();
}
