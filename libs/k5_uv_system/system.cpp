#include "system.hpp"
using namespace System;

extern "C" unsigned int _estack;
extern "C" unsigned int _sdata;
extern "C" unsigned int _edata;
extern "C" unsigned int _sidata;


void System::CopyDataSection()
{
   unsigned int* sdata_ptr = &_sdata;
   unsigned int* edata_ptr = &_edata;
   unsigned int* sidata_ptr = &_sidata;

   for (; sdata_ptr < edata_ptr;)
   {
      *sdata_ptr++ = *sidata_ptr++;
   }
}

extern "C" unsigned int __wrap___udivsi3(unsigned int a, unsigned int b)
{
   return IntDivide(a, b);
}
