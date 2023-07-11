  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

  .section .isr_vectors,"a",%progbits
  .type VectorTable, %object
  .size VectorTable, .-VectorTable
VectorTable:
  .word _estack
  .word IRQ_Wrapper01 + 1
  .word IRQ_Wrapper02 + 1
  .word IRQ_Wrapper03 + 1
  .word 0x01
  .word 0x02
  .word 0x03
  .word 0x04
  .word IRQ_Wrapper08 + 1
  .word IRQ_Wrapper09 + 1
  .word IRQ_Wrapper10 + 1
  .word IRQ_Wrapper11 + 1
  .word IRQ_Wrapper12 + 1
  .word IRQ_Wrapper13 + 1
  .word IRQ_Wrapper14 + 1
  .word IRQ_Wrapper15 + 1
  .word IRQ_Wrapper16 + 1
  .word IRQ_Wrapper17 + 1
  .word IRQ_Wrapper18 + 1
  .word IRQ_Wrapper19 + 1
  .word IRQ_Wrapper20 + 1
  .word IRQ_Wrapper21 + 1
  .word IRQ_Wrapper22 + 1
  .word IRQ_Wrapper23 + 1
  .word IRQ_Wrapper24 + 1
  .word IRQ_Wrapper25 + 1
  .word IRQ_Wrapper26 + 1
  .word IRQ_Wrapper27 + 1
  .word IRQ_Wrapper28 + 1
  .word IRQ_Wrapper29 + 1
  .word IRQ_Wrapper30 + 1
  .word IRQ_Wrapper31 + 1
  .word IRQ_Wrapper32 + 1
  .word IRQ_Wrapper33 + 1
  .word IRQ_Wrapper34 + 1
  .word IRQ_Wrapper35 + 1
  .word IRQ_Wrapper36 + 1
  .word IRQ_Wrapper37 + 1
  .word IRQ_Wrapper38 + 1
  .word IRQ_Wrapper39 + 1
  .word IRQ_Wrapper40 + 1
  .word IRQ_Wrapper41 + 1
  .word IRQ_Wrapper42 + 1
  .word IRQ_Wrapper43 + 1
  .word IRQ_Wrapper44 + 1
  .word IRQ_Wrapper45 + 1
  .word IRQ_Wrapper46 + 1
  .word IRQ_Wrapper47 + 1

  .size	VectorTable, .-VectorTable


.section .text
.extern MultiIrq_Handler
IRQ_Wrapper01:
  movs    r0, #1
  b MultiIrq_Handler
IRQ_Wrapper02:
  movs    r0, #2
  b MultiIrq_Handler
IRQ_Wrapper03:
  movs    r0, #3
  b MultiIrq_Handler
IRQ_Wrapper04:
  movs    r0, #4
  b MultiIrq_Handler
IRQ_Wrapper05:
  movs    r0, #5
  b MultiIrq_Handler
IRQ_Wrapper06:
  movs    r0, #6
  b MultiIrq_Handler
IRQ_Wrapper07:
  movs    r0, #7
  b MultiIrq_Handler
IRQ_Wrapper08:
  movs    r0, #8
  b MultiIrq_Handler
IRQ_Wrapper09:
  movs    r0, #9
  b MultiIrq_Handler
IRQ_Wrapper10:
  movs    r0, #10
  b MultiIrq_Handler
IRQ_Wrapper11:
  movs    r0, #11
  b MultiIrq_Handler
IRQ_Wrapper12:
  movs    r0, #12
  b MultiIrq_Handler
IRQ_Wrapper13:
  movs    r0, #13
  b MultiIrq_Handler
IRQ_Wrapper14:
  movs    r0, #14
  b MultiIrq_Handler
IRQ_Wrapper15:
  movs    r0, #15
  b MultiIrq_Handler
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