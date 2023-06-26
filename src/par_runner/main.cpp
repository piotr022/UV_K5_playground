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

void MultiIrq_Handler(unsigned int u32IrqSource)
{
   GPIOC->DATA ^= 1 << 3;
}