#include "stm32g030xx.h"
extern "C" long entry_point();
extern "C" void __asm_nop(int a, int b, int c, int d)
{
   __NOP();
}
int main()
{
   entry_point();
   return 0;
}

