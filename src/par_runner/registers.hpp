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

struct TPort
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
   unsigned int PORTA_PU;
   unsigned int PORTB_PU;
   unsigned int PORTC_PU;
   unsigned int PORTA_PD;
   unsigned int PORTB_PD;
   unsigned int PORTC_PD;
   unsigned int PORTA_OD;
   unsigned int PORTB_OD;
   unsigned int PORTC_OD;
   unsigned int PORTA_WKE;
   unsigned int PORTB_WKE;
   unsigned int PORTC_WKE;
   unsigned int PORT_CFG;
};

struct TGpio
{
   unsigned int DATA;
};

#define GPIO_BASE 0x400B0000
#define GPIO ((TPort*)GPIO_BASE)
#define __BKPT(value)                       __asm volatile ("bkpt "#value)

#define GPIOC_BASE 0x40061000
#define GPIOC ((TGpio*)GPIOC_BASE)
