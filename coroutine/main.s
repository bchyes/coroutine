	.file	"main.c"
	.text
	.globl	getid_val
	.data
	.align 8
	.type	getid_val, @object
	.size	getid_val, 8
getid_val:
	.quad	-1
	.section	.rodata
.LC0:
	.string	"Hello World!"
	.text
	.globl	test_costart
	.type	test_costart, @function
test_costart:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	movl	$100, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	test_costart, .-test_costart
	.section	.rodata
.LC1:
	.string	"reach to this area0"
.LC2:
	.string	"Nested creation failed"
.LC3:
	.string	"reach to this area1"
.LC4:
	.string	"reach to this area2"
	.text
	.globl	nested_costart
	.type	nested_costart, @function
nested_costart:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC1(%rip), %rdi
	call	puts@PLT
	leaq	test_costart(%rip), %rdi
	call	co_start@PLT
	cltq
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jns	.L4
	movl	$16, %edx
	leaq	__func__.3525(%rip), %rsi
	leaq	.LC2(%rip), %rdi
	call	fail@PLT
.L4:
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	movq	-8(%rbp), %rax
	movl	%eax, %edi
	call	co_wait@PLT
	leaq	.LC4(%rip), %rdi
	call	puts@PLT
	movl	$200, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	nested_costart, .-nested_costart
	.section	.rodata
.LC5:
	.string	"Coroutine ID not equal"
.LC6:
	.string	"Coroutine return value failed"
.LC7:
	.string	"Nested coroutine ID not equal"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$0, %edi
	call	srand@PLT
	movl	$0, -104(%rbp)
	jmp	.L7
.L9:
	leaq	test_costart(%rip), %rdi
	call	co_start@PLT
	movslq	%eax, %rdx
	movl	-104(%rbp), %eax
	cltq
	movq	%rdx, -96(%rbp,%rax,8)
	movl	-104(%rbp), %eax
	cltq
	movq	-96(%rbp,%rax,8), %rdx
	movl	-104(%rbp), %eax
	cltq
	cmpq	%rax, %rdx
	je	.L8
	movl	$54, %edx
	leaq	__func__.3530(%rip), %rsi
	leaq	.LC5(%rip), %rdi
	call	fail@PLT
.L8:
	addl	$1, -104(%rbp)
.L7:
	cmpl	$9, -104(%rbp)
	jle	.L9
	movl	$0, %eax
	call	co_waitall@PLT
	movl	$0, -100(%rbp)
	jmp	.L10
.L12:
	movl	-100(%rbp), %eax
	cltq
	movq	-96(%rbp,%rax,8), %rax
	movl	%eax, %edi
	call	co_getret@PLT
	cmpl	$100, %eax
	je	.L11
	movl	$60, %edx
	leaq	__func__.3530(%rip), %rsi
	leaq	.LC6(%rip), %rdi
	call	fail@PLT
.L11:
	addl	$1, -100(%rbp)
.L10:
	cmpl	$9, -100(%rbp)
	jle	.L12
	leaq	nested_costart(%rip), %rdi
	call	co_start@PLT
	cltq
	movq	%rax, -96(%rbp)
	movq	-96(%rbp), %rax
	cmpq	$10, %rax
	je	.L13
	movl	$64, %edx
	leaq	__func__.3530(%rip), %rsi
	leaq	.LC7(%rip), %rdi
	call	fail@PLT
.L13:
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L15
	call	__stack_chk_fail@PLT
.L15:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.section	.rodata
	.align 8
	.type	__func__.3525, @object
	.size	__func__.3525, 15
__func__.3525:
	.string	"nested_costart"
	.type	__func__.3530, @object
	.size	__func__.3530, 5
__func__.3530:
	.string	"main"
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
