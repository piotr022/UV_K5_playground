  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */

  bl main

LoopForever:
    b LoopForever

.size Reset_Handler, .-Reset_Handler
