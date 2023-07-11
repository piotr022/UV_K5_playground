#include "system.hpp"
using namespace System;

extern "C" void Reset_Handler();
extern "C" unsigned int _estack;
extern "C" unsigned int _sdata;
extern "C" unsigned int _edata;
extern "C" unsigned int _sidata;

extern "C" const unsigned int __org_vectors_start;
volatile static const auto *pOrgVectors = (const TVectorTable *)&__org_vectors_start;

#define ASM_VT

extern void MultiIrq_Handler(unsigned int);

#ifndef ASM_VT
template <int VectorNr> __attribute__((interrupt)) static void CommonIrqWrapper()
{
   MultiIrq_Handler(VectorNr);
   if (pOrgVectors->Vectors[VectorNr])
   {
      pOrgVectors->Vectors[VectorNr]();
   }
}

#endif

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

typedef void func_ptr(void*);

void System::JumpToOrginalFw()
{
   pOrgVectors->Vectors[1]();
}

void System::JumpToOrginalVector(unsigned int u32IrqSource)
{
    pOrgVectors->Vectors[u32IrqSource]();
}


#ifndef ASM_VT


TVectorTable __attribute__((section(".isr_vectors"))) VectorTable =
    {
        (VoidFxPointer)&_estack,            //STACK PTR
        (VoidFxPointer)&Reset_Handler,      //RESET
        &CommonIrqWrapper<2>,               //NMI
        &CommonIrqWrapper<3>,               //hard fault or flash?
        //nullptr,

        nullptr, //&CommonIrqWrapper<4>,
        nullptr, //&CommonIrqWrapper<5>,
        nullptr, //&CommonIrqWrapper<6>,
        nullptr, //&CommonIrqWrapper<7>,

        nullptr, //&CommonIrqWrapper<8>,
        nullptr, //&CommonIrqWrapper<9>,
        nullptr, //&CommonIrqWrapper<10>,
        &CommonIrqWrapper<11>,              //RCC

        nullptr, //&CommonIrqWrapper<12>,
        nullptr, //&CommonIrqWrapper<13>,
        &CommonIrqWrapper<14>,              //TIM1
        &CommonIrqWrapper<15>,              //Reserved

        &CommonIrqWrapper<16>,
        &CommonIrqWrapper<17>,
        &CommonIrqWrapper<18>,
        &CommonIrqWrapper<19>,

        &CommonIrqWrapper<20>,
        &CommonIrqWrapper<21>,
        &CommonIrqWrapper<22>,          //TIM7
        &CommonIrqWrapper<23>,

        &CommonIrqWrapper<24>,
        &CommonIrqWrapper<25>,
        &CommonIrqWrapper<26>,
        //&CommonIrqWrapper<27>,  //UART0?
        nullptr,

        &CommonIrqWrapper<28>,  //UART1?
        //&CommonIrqWrapper<29>,  //UART2?
        nullptr,
        &CommonIrqWrapper<30>,
        &CommonIrqWrapper<31>,

        &CommonIrqWrapper<32>,
        &CommonIrqWrapper<33>,
        &CommonIrqWrapper<34>,
        &CommonIrqWrapper<35>,

        &CommonIrqWrapper<36>,
        &CommonIrqWrapper<37>,
        &CommonIrqWrapper<38>,
        &CommonIrqWrapper<39>,

        &CommonIrqWrapper<40>,
        &CommonIrqWrapper<41>,
        &CommonIrqWrapper<42>,
        &CommonIrqWrapper<43>,

        &CommonIrqWrapper<44>,
        &CommonIrqWrapper<45>,
        &CommonIrqWrapper<46>,
        &CommonIrqWrapper<47>,
};

#else


#endif