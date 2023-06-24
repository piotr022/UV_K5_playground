#include "system/system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"

Hardware::THardware Hw;

int main()
{
   Hw.Power.EnableDbg();
   __BKPT();
   System::JumpToOrginalFw();
   while(1);
   return 0;
}

void MultiIrq_Handler(unsigned int u32IrqSource)
{
   Hw.Power.EnableDbg();
}