#include "system/system.hpp"
#include "hardware/hardware.hpp"
#include "registers.hpp"

Hardware::THardware Hw;
const System::TOrgFunctions Fw = System::OrgFunc_01_26;

int main()
{
   System::JumpToOrginalFw();
   return 0;
}

void MultiIrq_Handler(unsigned int u32IrqSource)
{
   if(GPIOA->DATA & GPIO_PIN_3) // exit key
   {
      Hw.FlashLight.BlinkSync(1);

      Fw.BK4819Write(0x58, 0b01111100110011);
      for(int i = 0; i < 8; i++)
      {
         Fw.BK4819Write(0x5F, 0xAB);
      }

      Fw.BK4819Read(0x1);
      // Fw.BK4819Write(0x5D, 0xF << 5);
      // Fw.BK4819Write(0x59, 0b1 << 11);
      // Fw.BK4819Write(0x58, 0b01111100110011);

   }

   unsigned char u8Pa10_13 = (GPIOA->DATA >> 10) & 0xF;
   if(u8Pa10_13)
   { 
      Hw.FlashLight.BlinkSync(u8Pa10_13);
   }
}