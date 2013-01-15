/*
 * $FILE: setjmp.S
 *
 * Setjmp and Longjmp functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mmasmano@ai2.upv.es>
 *         Salvador Peiró <speiro@ai2.upv.es>
 *
 * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
	
.text

.global setjmp, longjmp
.type   setjmp,@function
.type   longjmp,@function

setjmp:
	movl 0(%esp), %ecx
	leal 4(%esp), %edx
	movl (%edx), %eax
	movl %ebx, 0(%eax)
	movl %esi, 4(%eax)
	movl %edi, 8(%eax)
	movl %ebp, 12(%eax)
	movl %edx, 16(%eax)
	movl %ecx, 20(%eax)
	movl $0, %eax
	ret

longjmp:
	movl 4(%esp), %ecx
	movl 8(%esp), %eax
	movl 20(%ecx), %edx
	movl 0(%ecx), %ebx
	movl 4(%ecx), %esi
	movl 8(%ecx), %edi
	movl 12(%ecx), %ebp
	movl 16(%ecx), %esp
        jmp *%edx
