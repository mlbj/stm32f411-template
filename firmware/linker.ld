ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Top of stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);  

SECTIONS
{
  .isr_vector :
  {
    /* Startup vector */
    KEEP(*(.isr_vector))  
  } > FLASH

  .text :
  {
    /* Code */
    . = ALIGN(4);
    *(.text*)  
	
    /* Constants */ 
    *(.rodata*)    
   . = ALIGN(4); 
    _etext = .;
  } > FLASH

  .data : AT(_etext)
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  /* Symbols for startup */
  _sidata = LOADADDR(.data);
}

