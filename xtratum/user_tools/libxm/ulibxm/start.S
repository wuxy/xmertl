.text

.globl start, _start
.align 4

start:
_start:
	cld			;
	movl 4(%esp), %eax	;
	movl %eax, event_handling ;
	
/*	movl 8(%esp), %ebx	;
	movl %ebx, heap_addr	;
	movl 4(%esp), %ebx	;
	movl %ebx, heap_size	;
	movl (%esp), %ebx	;
	movl %ebx, event_handling ;
	*/
	call kmain		;

	movl $0, %eax		;
	int $0x82		;

