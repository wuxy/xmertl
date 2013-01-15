
/* mthomas: modified for LPC2138 */

MEMORY
{
  flash	(rx) : ORIGIN = 0x00000000, LENGTH = 256k
  ram	(rw) : ORIGIN = 0x40000000, LENGTH = 32k
}

/* __stack_end__ = ORIGIN(ram) + LENGTH(ram) -4; */

SECTIONS 
{
	. = 0;
	btext = .;

	/* first section is .text which is used for code */
	.text : 
	{
		*(.startup)
	} >flash
	
	.text : 
	{
		partikle_core.o (.text)

		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)
		
		. = ALIGN(4);
	} >flash

	.rodata :
	{
		partikle_core.o (.rodata)
		*(.rodata)	
	} >flash

	. = ALIGN(4);
	etext = .;
		

	/* .data section which is used for initialized data */
	.data :  AT (etext)
	{
		bdata = .;
		
		partikle_core.o (.data)

		*(.data)
		*(.gnu.linkonce.d*)
		
		. = ALIGN(4);
		edata = .;
		
	} >ram

	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD): 
	{
		bbss = .;
		partikle_core.o (.bss)

		*(.bss)
		*(.bss.*) /* mthomas 7/2006 */
		*(.gnu.linkonce.b*)
		*(COMMON)

		. = ALIGN(4);
	} >ram

	/* Align here to ensure that the .bss section occupies space up to
	_end.  Align after .bss to ensure correct alignment even if the
	.bss section disappears because there are no input sections.  */
	. = ALIGN(4);
	
	/DISCARD/ :
	{
		*(.note*)
		*(.comment*)
		*(.debug*)
	}
}
	. = ALIGN(4);
	_end = .;
	ebss = . ;
	PROVIDE (end = .);
