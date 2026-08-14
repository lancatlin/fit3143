	.file	"omp_0.c"
	.intel_syntax noprefix
	.text
	.section	.rodata
.LC1:
	.string	"sum value: %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	sub	rsp, 144
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR -8[rbp], rax
	xor	eax, eax
	mov	DWORD PTR -96[rbp], 1
	mov	DWORD PTR -92[rbp], 2
	mov	DWORD PTR -88[rbp], 3
	mov	DWORD PTR -84[rbp], 4
	mov	DWORD PTR -80[rbp], 5
	mov	DWORD PTR -76[rbp], 6
	mov	DWORD PTR -72[rbp], 7
	mov	DWORD PTR -68[rbp], 8
	mov	DWORD PTR -64[rbp], 9
	mov	DWORD PTR -60[rbp], 2
	mov	DWORD PTR -48[rbp], 1
	mov	DWORD PTR -44[rbp], 2
	mov	DWORD PTR -40[rbp], 3
	mov	DWORD PTR -36[rbp], 4
	mov	DWORD PTR -32[rbp], 5
	mov	DWORD PTR -28[rbp], 6
	mov	DWORD PTR -24[rbp], 7
	mov	DWORD PTR -20[rbp], 8
	mov	DWORD PTR -16[rbp], 9
	mov	DWORD PTR -12[rbp], 2
	pxor	xmm0, xmm0
	movss	DWORD PTR -132[rbp], xmm0
	lea	rax, -96[rbp]
	mov	QWORD PTR -128[rbp], rax
	lea	rax, -48[rbp]
	mov	QWORD PTR -120[rbp], rax
	movss	xmm0, DWORD PTR -132[rbp]
	movss	DWORD PTR -112[rbp], xmm0
	lea	rax, -128[rbp]
	mov	ecx, 0
	mov	edx, 0
	mov	rsi, rax
	lea	rax, main._omp_fn.0[rip]
	mov	rdi, rax
	call	GOMP_parallel@PLT
	movss	xmm0, DWORD PTR -112[rbp]
	movss	DWORD PTR -132[rbp], xmm0
	pxor	xmm1, xmm1
	cvtss2sd	xmm1, DWORD PTR -132[rbp]
	movq	rax, xmm1
	movq	xmm0, rax
	lea	rax, .LC1[rip]
	mov	rdi, rax
	mov	eax, 1
	call	printf@PLT
	mov	eax, 0
	mov	rdx, QWORD PTR -8[rbp]
	sub	rdx, QWORD PTR fs:40
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC2:
	.string	"Thread: %d of %d. Local Sum: %f\n"
	.text
	.type	main._omp_fn.0, @function
main._omp_fn.0:
.LFB1:
	.cfi_startproc
	endbr64
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	push	rbx
	sub	rsp, 40
	.cfi_offset 3, -24
	mov	QWORD PTR -40[rbp], rdi
	pxor	xmm0, xmm0
	movss	DWORD PTR -32[rbp], xmm0
	call	omp_get_thread_num@PLT
	mov	DWORD PTR -24[rbp], eax
	call	omp_get_num_threads@PLT
	mov	DWORD PTR -20[rbp], eax
	call	omp_get_num_threads@PLT
	mov	ebx, eax
	call	omp_get_thread_num@PLT
	mov	esi, eax
	mov	eax, 10
	cdq
	idiv	ebx
	mov	ecx, eax
	mov	eax, 10
	cdq
	idiv	ebx
	mov	eax, edx
	cmp	esi, eax
	jl	.L5
.L8:
	imul	esi, ecx
	mov	edx, esi
	add	eax, edx
	lea	edx, [rax+rcx]
	cmp	eax, edx
	jge	.L6
	mov	DWORD PTR -28[rbp], eax
.L7:
	mov	rax, QWORD PTR -40[rbp]
	mov	rax, QWORD PTR [rax]
	mov	ecx, DWORD PTR -28[rbp]
	movsx	rcx, ecx
	mov	esi, DWORD PTR [rax+rcx*4]
	mov	rax, QWORD PTR -40[rbp]
	mov	rax, QWORD PTR 8[rax]
	mov	ecx, DWORD PTR -28[rbp]
	movsx	rcx, ecx
	mov	eax, DWORD PTR [rax+rcx*4]
	imul	eax, esi
	pxor	xmm0, xmm0
	cvtsi2ss	xmm0, eax
	movss	xmm1, DWORD PTR -32[rbp]
	addss	xmm0, xmm1
	movss	DWORD PTR -32[rbp], xmm0
	add	DWORD PTR -28[rbp], 1
	cmp	DWORD PTR -28[rbp], edx
	jl	.L7
.L6:
	call	GOMP_barrier@PLT
	call	GOMP_critical_start@PLT
	pxor	xmm2, xmm2
	cvtss2sd	xmm2, DWORD PTR -32[rbp]
	movq	rcx, xmm2
	mov	edx, DWORD PTR -20[rbp]
	mov	eax, DWORD PTR -24[rbp]
	movq	xmm0, rcx
	mov	esi, eax
	lea	rax, .LC2[rip]
	mov	rdi, rax
	mov	eax, 1
	call	printf@PLT
	mov	rax, QWORD PTR -40[rbp]
	movss	xmm0, DWORD PTR 16[rax]
	addss	xmm0, DWORD PTR -32[rbp]
	mov	rax, QWORD PTR -40[rbp]
	movss	DWORD PTR 16[rax], xmm0
	call	GOMP_critical_end@PLT
	jmp	.L9
.L5:
	mov	eax, 0
	add	ecx, 1
	jmp	.L8
.L9:
	mov	rbx, QWORD PTR -8[rbp]
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main._omp_fn.0, .-main._omp_fn.0
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
