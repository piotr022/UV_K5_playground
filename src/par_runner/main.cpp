#include "system/system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"

Hardware::THardware Hw;

int main()
{
   while(1);
   Hw.Power.EnableDbg();
   System::JumpToOrginalFw();
   return 0;
} 

__attribute__ ((interrupt)) void MultiIrq_Handler(unsigned int u32IrqSource)
{
   while(1);
   Hw.Power.EnableDbg();
}