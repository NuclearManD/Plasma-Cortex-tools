.org 0x8300
	.global	putchar
_putchar:
	add	sp,-4
	; type=2
	mov	[sp+0], ax
	; type=68
	mov	di, _stdout+0
	mov	ax, [di]
	or	ax
	; type=71
	jnz	l3
l2:
	; type=94
	mov	ax,65024
	; type=76
	jmp	l1
l3:
	; type=2
	mov	ax, [sp+0]
	mov	di, _stdout+0
	mov	[di], ax
	; type=81
	mov	di, _stdout+0
	mov	ax, [di]
	inc	ax
	mov	di, _stdout+0
	mov	[di], ax
l4:
	; type=94
	mov	ax, [sp+0]
l1:
	add	sp,4
	ret
	.equ	__stack_putchar,4
	.global	printf
_printf:
	add	sp,-8
	; type=2
	mov	[sp+0], ax
	; type=68
	mov	di, _stdout+0
	mov	ax, [di]
	or	ax
	; type=71
	jnz	l7
l6:
	; type=94
	mov	ax,65535
	; type=76
	jmp	l5
l7:
	; type=2
	xor	ax
	mov	[sp+4], ax
	; type=76
	jmp	l9
l8:
	; type=81
	mov	ax, [sp+0]
	mov	bx, [sp+4]
	add	ax, bx
	mov	cx,ax
	; type=50
	mov	ax, cx
	mov	cx,ax
	; type=2
	mov	ax,cx
	; type=42
	call _putchar
	; type=93
	mov	cx,ax
	; type=77
	sub	cx, 65024	; type=71
	jnz	l13
l12:
	; type=94
	mov	ax, [sp+4]
	; type=76
	jmp	l5
l13:
l11:
	; type=27
	mov	ax, [sp+4]
	inc	ax
	mov	[sp+4], ax
l9:
	; type=81
	mov	ax, [sp+0]
	mov	bx, [sp+4]
	add	ax, bx
	mov	cx,ax
	; type=50
	mov	ax, cx
	mov	cx,ax
	; type=68
	mov	ax,cx
	or	ax
	; type=71
	jnz	l8
l10:
	; type=94
	mov	ax, [sp+4]
l5:
	add	sp,8
	ret
	.equ	__stack_printf,8
	.global	set_stdout
_set_stdout:
	add	sp,-4
	; type=2
	mov	[sp+0], ax
	; type=2
	mov	ax, [sp+0]
	mov	di, _stdout+0
	mov	[di], ax
l14:
	add	sp,4
	ret
	.equ	__stack_set_stdout,4
.section DATA
	.global	_stdout
.align 4
_stdout:
		dd	 0
