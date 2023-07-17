ENTRY(Reset_Handler)
EXTERN(VectorTable)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x2000138C, LENGTH = 300
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 60K
}

_estack = 0x20001388;

SECTIONS
{
    . = 0x0;
    .isr_vectors : 
    {
     . = ALIGN(4);
     KEEP(*(.isr_vectors))
     . = ALIGN(4);
    } >FLASH

    .org_fw_rest :
    {
      . = ALIGN(4);
      KEEP(*(.org_fw_rest))
    } > FLASH

    /*
    .org_vectors :
    {
      . = ALIGN(4);
      __org_vectors_start = .;
      KEEP(*(.org_vectors))
    } > FLASH
    */
    .text :
    {
        . = ALIGN(4);
        *(.text)   
        *(.text*) 
        *(.rodata)
        *(.rodata*)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(4);
        /* __udivsi3 = __wrap___udivsi3; */

    } >FLASH

    .preinit_array     :
    {
      __preinit_array_start = .;
      KEEP (*(.preinit_array*))
      __preinit_array_end = .;
    } >FLASH
    .init_array :
    {
      __init_array_start = .;
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      __init_array_end = .;
    } >FLASH
    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
      . = ALIGN(4);
      _flash_data_start = .;
    } >FLASH
    
     _sidata = LOADADDR(.data);
    .data : AT (_flash_data_start)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        *(.ramsection)
        _edata = .;
    } >RAM

    .bss :
    {
      . = ALIGN(4);
       _sbss = .;
        *(.bss)       
      _ebss = .;
    } >RAM

      /DISCARD/ :
  {
  }    
}