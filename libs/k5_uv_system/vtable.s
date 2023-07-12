  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

  .section .isr_vectors,"a",%progbits
  .type VectorTable, %object
  .size VectorTable, .-VectorTable
VectorTable:
  .word _estack
  .word IRQ_RESET + 1
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
  .word IRQ_SysTick + 1
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

.section .text
.extern MultiIrq_Handler
IRQ_RESET:
  movs    r0, #1
  b MultiIrq_Handler

/*
IRQ_NMI:  //jumps to endless loop in Original FW
  movs    r0, #2
  b MultiIrq_Handler
*/

/*
IRQ_HardFault:   //jumps to endless loop in Original FW
  movs    r0, #3
  b MultiIrq_Handler
*/

//--- all zeros in OFW ----
//4 MemManage
//5 BusFault
//6 UsageFault
//7 Reserved1
//8 Reserved2
//9 Reserved3
//10 Reserved4

/*
IRQ_SVCall: //jumps to endless loop in Original FW
  movs    r0, #11
  b MultiIrq_Handler
*/

//12 Reserved5
//13 Reserved6

/*
IRQ_PendSV: //jumps to endless loop in Original FW
  movs    r0, #14
  b MultiIrq_Handler
*/

IRQ_SysTick:
  movs    r0, #15
  b MultiIrq_Handler

//every interrupt below jumps to endless loop in OFW
/*
IRQ_Wrapper16:
  movs    r0, #16
  b MultiIrq_Handler
IRQ_Wrapper17:
  movs    r0, #17
  b MultiIrq_Handler
IRQ_Wrapper18:
  movs    r0, #18
  b MultiIrq_Handler
IRQ_Wrapper19:
  movs    r0, #19
  b MultiIrq_Handler
IRQ_Wrapper20:
  movs    r0, #20
  b MultiIrq_Handler
IRQ_Wrapper21:
  movs    r0, #21
  b MultiIrq_Handler
IRQ_Wrapper22:
  movs    r0, #22
  b MultiIrq_Handler
IRQ_Wrapper23:
  movs    r0, #23
  b MultiIrq_Handler
IRQ_Wrapper24:
  movs    r0, #24
  b MultiIrq_Handler
IRQ_Wrapper25:
  movs    r0, #25
  b MultiIrq_Handler
IRQ_Wrapper26:
  movs    r0, #26
  b MultiIrq_Handler
IRQ_Wrapper27:
  movs    r0, #27
  b MultiIrq_Handler
IRQ_Wrapper28:
  movs    r0, #28
  b MultiIrq_Handler
IRQ_Wrapper29:
  movs    r0, #29
  b MultiIrq_Handler
IRQ_Wrapper30:
  movs    r0, #30
  b MultiIrq_Handler
IRQ_Wrapper31:
  movs    r0, #31
  b MultiIrq_Handler
IRQ_Wrapper32:
  movs    r0, #32
  b MultiIrq_Handler
IRQ_Wrapper33:
  movs    r0, #33
  b MultiIrq_Handler
IRQ_Wrapper34:
  movs    r0, #34
  b MultiIrq_Handler
IRQ_Wrapper35:
  movs    r0, #35
  b MultiIrq_Handler
IRQ_Wrapper36:
  movs    r0, #36
  b MultiIrq_Handler
IRQ_Wrapper37:
  movs    r0, #37
  b MultiIrq_Handler
IRQ_Wrapper38:
  movs    r0, #38
  b MultiIrq_Handler
IRQ_Wrapper39:
  movs    r0, #39
  b MultiIrq_Handler
IRQ_Wrapper40:
  movs    r0, #40
  b MultiIrq_Handler
IRQ_Wrapper41:
  movs    r0, #41
  b MultiIrq_Handler
IRQ_Wrapper42:
  movs    r0, #42
  b MultiIrq_Handler
IRQ_Wrapper43:
  movs    r0, #43
  b MultiIrq_Handler
IRQ_Wrapper44:
  movs    r0, #44
  b MultiIrq_Handler
IRQ_Wrapper45:
  movs    r0, #45
  b MultiIrq_Handler
IRQ_Wrapper46:
  movs    r0, #46
  b MultiIrq_Handler
IRQ_Wrapper47:
  movs    r0, #47
  b MultiIrq_Handler
*/