#include "system.hpp"
using namespace System;

extern "C" void Reset_Handler();
extern "C" unsigned int _estack;
extern "C" unsigned int _sdata;
extern "C" unsigned int _edata;
extern "C" unsigned int _sidata;

extern "C" const unsigned int __org_vectors_start;
volatile static const auto *pOrgVectors = (const TVectorTable *)&__org_vectors_start;

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

void System::JumpToOrginalFw()
{
   pOrgVectors->Vectors[1]();
}

void System::JumpToOrginalVector(unsigned int u32IrqSource)
{
    pOrgVectors->Vectors[u32IrqSource]();
}
