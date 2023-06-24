#include "hardware.hpp"
#include "../registers.hpp"

using namespace Hardware;

void TPower::EnableDbg()
{
   // PB11 alternate fx to SWDIO
   GPIO->PORTB_SEL1 &= ~(0b1111 << 12);
   GPIO->PORTB_SEL1 |= (0b1 << 12);

   // PB14 alternate fx to SWDIO
   GPIO->PORTB_SEL1 &= ~(0b1111 << 24);
   GPIO->PORTB_SEL1 |= (0b1 << 24);
}