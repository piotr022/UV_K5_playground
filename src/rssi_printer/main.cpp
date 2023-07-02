#include "system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"
#include "uv_k5_display.hpp"
#include "rssi_printer.hpp"
#include <string.h>

Hardware::THardware Hw;
const System::TOrgFunctions& Fw = System::OrgFunc_01_26;
const System::TOrgData& FwData = System::OrgData_01_26;

int main()
{
   System::JumpToOrginalFw();
   return 0;
} 

void MultiIrq_Handler(unsigned int u32IrqSource)
{
   static unsigned int u32StupidCounter = 1;
   //GPIOA->DATA & GPIO_PIN_3  
   if((!(u32StupidCounter++ % 15) && u32StupidCounter > 200)) // exit key
   {
      CRssiPrinter::Handle(Fw, FwData);
   }
}