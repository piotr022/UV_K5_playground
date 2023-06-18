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

#define FLASH_BASAE 0x4006F000
#define FLASH ((TFlash*)FLASH_BASAE)