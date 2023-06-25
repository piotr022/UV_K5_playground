#include "system/system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"

Hardware::THardware Hw;
extern "C" void Reset_Handler();

int main()
{
   Hw.Power.EnableDbg();
   System::JumpToOrginalFw();
   return 0;
} 

__attribute__ ((interrupt)) void MultiIrq_Handler(unsigned int u32IrqSource)
{
   Hw.Power.EnableDbg();
   __BKPT();
   while(1);
}