#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "t9_texting.hpp"
#include <string.h>

Hardware::THardware Hw;
const System::TOrgFunctions& Fw = System::OrgFunc_01_26;
const System::TOrgData& FwData = System::OrgData_01_26;

CT9Texting<System::OrgFunc_01_26, System::OrgData_01_26> T9Texting;

int main()
{
   System::JumpToOrginalFw();
   return 0;
}

extern "C" void MultiIrq_Handler(unsigned int u32IrqSource)
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

   bool bPreventWhileKeypadPolling = pStackedRegs->LR > (unsigned int)Fw.PollKeyboard && 
      pStackedRegs->PC < (unsigned int)Fw.PollKeyboard + 0x100; // i made a mistake and compared PC and LR, but this works fine xD

   static unsigned int u32StupidCounter = 1;
   if(u32StupidCounter++ > 200 && !bPreventWhileKeypadPolling)
   {
      T9Texting.Handle();
   }
    System::JumpToOrginalVector(u32IrqSource);
}