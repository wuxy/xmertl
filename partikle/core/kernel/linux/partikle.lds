/*
 * $FILE$
 * 
 * ld script to build i386
 * 
 * $VERSION$
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *  
 * $LICENSE$
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
  . = 0x00100000;
  _spartikle = .;
  .text_core ALIGN (0x1000): {
	_stext = .;
  	partikle_core.o (.text)
	_etext = .;
  }
  
  .rodata_core ALIGN (0x1000) : {
	_srodata = .;
	partikle_core.o (.rodata)
  	partikle_core.o (.rodata.*)
	_erodata = .;
  }
  
  .data_core ALIGN (0x1000) : {
	_sdata = .;
    	partikle_core.o (.data)
	_edata = .;
  }
  
  .bss_core : {
	_sbss = .;
	partikle_core.o (COMMON)
	partikle_core.o (.bss)
	_ebss = .;
  }
  _epartikle = .;
  . = 0x10000000;
  _sapp = .;
  .text ALIGN (0x1000): {
	_stext_app = .;
  	* (.text)
	_etext_app = .;
  }
  
  .rodata ALIGN (0x1000) : {
	_srodata_app = .;
	* (.rodata)
  	* (.rodata.*)
	* (.rodata.*.*)

	. = ALIGN(4);
	__CTOR_LIST__ = .;
	LONG(-1)
	* (.ctors)
	. = ALIGN(4);
	__CTOR_END__ = .;
	LONG(0)

	. = ALIGN(4);
	__DTOR_LIST__ = .;
	LONG(-1)
	* (.dtors)
	. = ALIGN(4);
	__DTOR_END__ = .;
	LONG(0)

	. = ALIGN (4);
	__JCR_LIST__ = .;
	* (.jcr)
	. = ALIGN (4);
	__JCR_END__ = .;
	LONG(0)

	. = ALIGN (4);
	__EH_FRAME_BEGIN__ = .;
	partikle_core.o (.eh_frame)
	* (.eh_frame)
	. = ALIGN (4);
	__FRAME_END__ = .;
	LONG(0)

	. = ALIGN (4);
	partikle_core.o (.gcc_except_table)
	* (.gcc_except_table)
	_erodata_app = .;
  }
  
  .data ALIGN (0x1000) : {
	_sdata_app = .;
    	* (.data)
	_edata_app = .;
  }
  
  .bss : {
	_sbss_app = .;
	* (COMMON)
	* (.bss)
	_ebss_app = .;
  }

  _eapp = .;
  /DISCARD/ :
  {
	*(.note)
	*(.comment*)
  }
}
