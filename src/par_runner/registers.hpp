#pragma once

struct TFlash
{
   unsigned int CFG;
   unsigned int ADDR;
   unsigned int WDATA;
   unsigned int START;
   unsigned int ST;
   unsigned int LOCK;
   unsigned int UNLOCK;
   unsigned int MASK;
   unsigned int ERASETIME;
   unsigned int PROGTIME;
};

#define FLASH_BASE 0x4006F000
#define FLASH ((TFlash*)FLASH_BASAE)

struct TGpio
{
   unsigned int PORTA_SEL0;
   unsigned int PORTA_SEL1;
   unsigned int PORTB_SEL0;
   unsigned int PORTB_SEL1;
   unsigned int PORTC_SEL0;
   unsigned int PORTC_SEL1;
   unsigned int PORTA_IE;
   unsigned int PORTB_IE;
   unsigned int PORTC_IE;
};

#define GPIO_BASE 0x400B0000
#define GPIO ((TGpio*)GPIO_BASE)