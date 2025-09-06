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
    *(.text*)  
	
    /* Constants */ 
    *(.rodata*)     
    _etext = .;
  } > FLASH

  .data : AT(_etext)
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  /* Symbols for startup */
  _sidata = LOADADDR(.data);
}

