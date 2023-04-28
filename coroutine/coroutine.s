	.file	"coroutine.c"
	.text
	.comm	main_coroutine,43200,32
	.comm	now_nest,4,4
	.comm	nest_id,40,32
	.comm	now_coroutine,8,8
	.comm	context,800,32
	.comm	retval,400,32
	.comm	fin_co,400,32
	.globl	now_co_id
	.bss
	.align 4
	.type	now_co_id, @object
	.size	now_co_id, 4
now_co_id:
	.zero	4
	.text
	.globl	refresh_context
	.type	refresh_context, @function
refresh_context:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	leal	1(%rax), %edx
	movl	now_co_id(%rip), %eax
	cmpl	%eax, %edx
	jne	.L2
	movl	now_co_id(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, now_co_id(%rip)
	cltq
	leaq	0(,%rax,8), %rcx
	leaq	context(%rip), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rcx,%rdx)
	jmp	.L4
.L2:
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,8), %rcx
	leaq	context(%rip), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, (%rcx,%rdx)
.L4:
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	refresh_context, .-refresh_context
	.section	.rodata
.LC0:
	.string	"coroutine.c"
.LC1:
	.string	"now_coroutine != NULL"
	.text
	.globl	co_yield
	.type	co_yield, @function
co_yield:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	now_coroutine(%rip), %rax
	testq	%rax, %rax
	jne	.L6
	leaq	__PRETTY_FUNCTION__.2921(%rip), %rcx
	movl	$35, %edx
	leaq	.LC0(%rip), %rsi
	leaq	.LC1(%rip), %rdi
	call	__assert_fail@PLT
.L6:
	movq	now_coroutine(%rip), %rax
	addq	$24, %rax
	movq	%rax, %rdi
	call	_setjmp@PLT
	endbr64
	movl	%eax, -8(%rbp)
	movq	now_coroutine(%rip), %rax
	movq	%rax, %rdi
	call	refresh_context
	cmpl	$0, -8(%rbp)
	jne	.L13
.L11:
	call	rand@PLT
	movl	now_co_id(%rip), %ecx
	cltd
	idivl	%ecx
	movl	%edx, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	fin_co(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	je	.L14
	jmp	.L11
.L14:
	nop
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	context(%rip), %rax
	movq	(%rdx,%rax), %rax
	movq	%rax, now_coroutine(%rip)
	movq	now_coroutine(%rip), %rax
	addq	$24, %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	longjmp@PLT
.L13:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	co_yield, .-co_yield
	.globl	co_getret
	.type	co_getret, @function
co_getret:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	context(%rip), %rax
	movq	(%rdx,%rax), %rax
	movl	16(%rax), %eax
	cmpl	$2, %eax
	je	.L16
	movl	$0, %eax
	call	co_yield
.L16:
	movl	-4(%rbp), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	retval(%rip), %rax
	movl	(%rdx,%rax), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	co_getret, .-co_getret
	.globl	co_waitall
	.type	co_waitall, @function
co_waitall:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	co_waitall, .-co_waitall
	.globl	wait
	.type	wait, @function
wait:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	jmp	.L20
.L21:
	movl	$0, %eax
	call	co_yield
.L20:
	movq	-8(%rbp), %rax
	movl	16(%rax), %eax
	cmpl	$2, %eax
	jne	.L21
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	fin_co(%rip), %rax
	movl	$1, (%rdx,%rax)
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	wait, .-wait
	.globl	co_wait
	.type	co_wait, @function
co_wait:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	context(%rip), %rax
	movq	(%rdx,%rax), %rax
	movq	%rax, %rdi
	call	wait
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	co_wait, .-co_wait
	.section	.rodata
.LC2:
	.string	"%d "
	.text
	.globl	exit_
	.type	exit_, @function
exit_:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
#APP
# 78 "coroutine.c" 1
	movl %eax, %edx;
# 0 "" 2
#NO_APP
	movl	%edx, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	now_coroutine(%rip), %rax
	movl	(%rax), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,4), %rcx
	leaq	retval(%rip), %rdx
	movl	-4(%rbp), %eax
	movl	%eax, (%rcx,%rdx)
	movq	now_coroutine(%rip), %rax
	movl	$2, 16(%rax)
.L24:
	movl	$0, %eax
	call	co_yield
	jmp	.L24
	.cfi_endproc
.LFE12:
	.size	exit_, .-exit_
	.section	.rodata
.LC3:
	.string	"id:%d\n"
.LC4:
	.string	"%d\n"
	.text
	.globl	co_start
	.type	co_start, @function
co_start:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -56(%rbp)
	movl	$43200, %edi
	call	malloc@PLT
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	-56(%rbp), %rdx
	movq	%rdx, 8(%rax)
	movq	-32(%rbp), %rax
	movl	$1, 16(%rax)
	movq	-32(%rbp), %rax
	addq	$224, %rax
	addq	$4096, %rax
	andq	$-16, %rax
	movq	%rax, -24(%rbp)
	movl	now_nest(%rip), %eax
	cltq
	imulq	$4320, %rax, %rax
	leaq	main_coroutine(%rip), %rdx
	movl	$-1, (%rax,%rdx)
	movl	now_nest(%rip), %eax
	addl	$1, %eax
	movl	%eax, now_nest(%rip)
	movl	$0, -40(%rbp)
	movl	now_co_id(%rip), %eax
	testl	%eax, %eax
	jne	.L26
	movl	now_nest(%rip), %eax
	leal	-1(%rax), %edx
	movl	now_co_id(%rip), %eax
	movslq	%edx, %rdx
	leaq	0(,%rdx,4), %rcx
	leaq	nest_id(%rip), %rdx
	movl	%eax, (%rcx,%rdx)
	jmp	.L32
.L26:
	movl	now_co_id(%rip), %eax
	movl	now_nest(%rip), %edx
	subl	$1, %edx
	leal	-1(%rax), %ecx
	movslq	%edx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	nest_id(%rip), %rax
	movl	%ecx, (%rdx,%rax)
.L32:
	movl	now_nest(%rip), %eax
	subl	$1, %eax
	cltq
	imulq	$4320, %rax, %rax
	leaq	16(%rax), %rdx
	leaq	main_coroutine(%rip), %rax
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, %rdi
	call	_setjmp@PLT
	endbr64
	movl	%eax, -36(%rbp)
	movl	now_nest(%rip), %eax
	subl	$1, %eax
	cltq
	imulq	$4320, %rax, %rax
	leaq	main_coroutine(%rip), %rdx
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	refresh_context
	cmpl	$0, -36(%rbp)
	je	.L35
	movl	now_nest(%rip), %eax
	subl	$1, %eax
	movl	%eax, now_nest(%rip)
	movl	$1, -40(%rbp)
	movl	now_nest(%rip), %eax
	testl	%eax, %eax
	je	.L36
	movl	now_nest(%rip), %eax
	subl	$1, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	nest_id(%rip), %rax
	movl	(%rdx,%rax), %eax
	addl	$1, %eax
	cltq
	leaq	0(,%rax,8), %rdx
	leaq	context(%rip), %rax
	movq	(%rdx,%rax), %rax
	movq	%rax, now_coroutine(%rip)
	movq	now_coroutine(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L32
.L35:
	nop
	jmp	.L30
.L36:
	nop
.L30:
	cmpl	$0, -40(%rbp)
	jne	.L33
	movl	now_co_id(%rip), %eax
	leal	-1(%rax), %edx
	movq	-32(%rbp), %rax
	movl	%edx, (%rax)
	movq	-32(%rbp), %rax
	movq	%rax, now_coroutine(%rip)
	movq	now_coroutine(%rip), %rax
	movq	%rax, %rdi
	call	refresh_context
	movq	-32(%rbp), %rax
	movq	8(%rax), %rdx
	movq	-24(%rbp), %rax
	leaq	exit_(%rip), %rcx
	movq	%rax, %rbx
#APP
# 140 "coroutine.c" 1
	movq %rbx, %rsp;pushq %rcx;jmp *%rdx;
# 0 "" 2
#NO_APP
.L33:
	movl	now_nest(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	nest_id(%rip), %rax
	movl	(%rdx,%rax), %eax
	movl	%eax, %esi
	leaq	.LC4(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	now_nest(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	nest_id(%rip), %rax
	movl	(%rdx,%rax), %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	co_start, .-co_start
	.section	.rodata
	.align 8
	.type	__PRETTY_FUNCTION__.2921, @object
	.size	__PRETTY_FUNCTION__.2921, 9
__PRETTY_FUNCTION__.2921:
	.string	"co_yield"
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
