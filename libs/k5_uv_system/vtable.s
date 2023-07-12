  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb


.extern Reset_Handler
.extern SysTick_Handler

  .section .isr_vectors,"a",%progbits
  .type VectorTable, %object
  .size VectorTable, .-VectorTable
VectorTable:
  .word _estack
  .word Reset_Handler + 1
  .word 2   //IRQ_NMI + 1
  .word 3   //IRQ_HardFault + 1
  .word 4   //IRQ_MemManage
  .word 5   //IRQ_BusFault
  .word 6   //IRQ_UsageFault
  .word 7   //Reserved1
  .word 8   //Reserved2
  .word 9   //Reserved3
  .word 10  //Reserved4
  .word 11  //IRQ_SVCall + 1
  .word 12  //Reserved5
  .word 13  //Reserved6
  .word 14  //IRQ_PendSV + 1
  .word SysTick_Handler + 1
  .word 16
  .word 17
  .word 18
  .word 19
  .word 20
  .word 21
  .word 22
  .word 23
  .word 24
  .word 25
  .word 26
  .word 27
  .word 28
  .word 29
  .word 30
  .word 31
  .word 32
  .word 33
  .word 34
  .word 35
  .word 36
  .word 37
  .word 38
  .word 39
  .word 40
  .word 41
  .word 42
  .word 43
  .word 44
  .word 45
  .word 46
  .word 47

  .size	VectorTable, .-VectorTable

.global VectorTable

