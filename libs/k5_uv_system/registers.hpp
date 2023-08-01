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
   unsigned int DIR;
};

struct TSysCon
{
   unsigned int CLK_SEL;
   unsigned int DIV_CLK_GATE;
   unsigned int DEV_CLK_GATE;
   unsigned int RC_FREQ_DELTA;
   unsigned int CHIP_ID[4];
   unsigned int PLL_CTRL;
   unsigned int PLL_ST;
};

struct TAdc
{
   unsigned int ADC_CFG;
   unsigned int ADC_START;
   unsigned int ADC_IE;
   unsigned int ADC_IF;
   struct
   {
      unsigned int STAT;
      unsigned int DATA;
   }CHANNEL[16];
   unsigned int ADC_FIFO_STAT;
   unsigned int ADC_FIFO_DATA;
   unsigned int EXTTRIG_SEL;
   unsigned int ADC_CALIB_OFFSET;
   unsigned int ADC_CALIB_KD;
};

#define GPIO_BASE 0x400B0000
#define GPIO ((TPort*)GPIO_BASE)
#define __BKPT(value)                       __asm volatile ("bkpt "#value)


#define GPIOA_BASE 0x40060000
#define GPIOA ((TGpio*)GPIOA_BASE)

#define GPIOB_BASE 0x40060800
#define GPIOB ((TGpio*)GPIOB_BASE)

#define GPIOC_BASE 0x40061000
#define GPIOC ((TGpio*)GPIOC_BASE)

#define GPIO_PIN_0  (1 << 0 )
#define GPIO_PIN_1  (1 << 1 )
#define GPIO_PIN_2  (1 << 2 )
#define GPIO_PIN_3  (1 << 3 )
#define GPIO_PIN_4  (1 << 4 )
#define GPIO_PIN_5  (1 << 5 )
#define GPIO_PIN_6  (1 << 6 )
#define GPIO_PIN_7  (1 << 7 )
#define GPIO_PIN_8  (1 << 8 )
#define GPIO_PIN_9  (1 << 9 )
#define GPIO_PIN_10 (1 << 10)
#define GPIO_PIN_11 (1 << 11)
#define GPIO_PIN_12 (1 << 12)
#define GPIO_PIN_13 (1 << 13)
#define GPIO_PIN_14 (1 << 14)
#define GPIO_PIN_15 (1 << 15)

#define SYSCON_BASE 0x40000000
#define SYSCON ((TSysCon*)SYSCON_BASE)

#define ADC_BASE 0x400BA000
#define ADC ((TAdc*)ADC_BASE)


