#include "system.hpp"
using namespace System;

extern "C" void Reset_Handler();
TVectorTable VectorTable = 
{
   &Reset_Handler,
   &CommonIrqWrapper<1>,
};