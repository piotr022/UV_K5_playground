#include "hardware.hpp"
#include "registers.hpp"

using namespace Hardware;

void TPower::EnableDbg()
{
   GPIOB->DIR &= ~(GPIO_PIN_11|GPIO_PIN_14);
   // PB11 alternate fx to SWDIO
   GPIO->PORTB_SEL1 &= ~(0b1111 << 12);
   GPIO->PORTB_SEL1 |= (0b1 << 12);

   // PB14 alternate fx to SWDIO
   GPIO->PORTB_SEL1 &= ~(0b1111 << 24);
   GPIO->PORTB_SEL1 |= (0b1 << 24);
}

void TSystem::Delay(unsigned int u32Ticks)
{
   for(volatile unsigned int i = 0; i < u32Ticks; i++)
   {
      __asm volatile ("dsb sy" : : : "memory");
   }
}

void TFlashLight::On()
{
   GPIOC->DATA |= 1 << 3;
}

void TFlashLight::Off()
{
   GPIOC->DATA &= ~(1 << 3);
}

void TFlashLight::Toggle()
{
   GPIOC->DATA ^= 1 << 3;
}

void TFlashLight::BlinkSync(unsigned char u8BlinksCnt)
{
   for(unsigned char i = 0; i < u8BlinksCnt*2; i++)
   {
      Toggle();
      System.Delay(200000);
   }

   Off();
}