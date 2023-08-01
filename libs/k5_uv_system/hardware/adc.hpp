#pragma once
#include "registers.hpp"
#include "system.hpp"

namespace Adc
{
   inline unsigned short ReadSingleChannelSync(unsigned char u8Channel)
   {
      ADC->ADC_START |= 1;

      while(!(ADC->CHANNEL[u8Channel].STAT & 1))
      {
      }

      ADC->ADC_IF = 1 << u8Channel;
      return ADC->CHANNEL[u8Channel].DATA & 0xFFF;
   }
};