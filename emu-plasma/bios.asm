.org 0x100
	.global	main
_main:
	; type=2
	mov	ax, l2
	; type=42
	call _printf
	; type=42
	call _sd_init
	; type=93
	mov	cx,ax
	; type=68
	mov	ax,cx
	or	ax, ax
	; type=70
	jz	l4
	; type=2
	mov	ax, l5
	; type=42
	call _printf
	; type=76
	jmp	l6
l4:
	; type=2
	mov	ax, l7
	; type=42
	call _printf
l6:
l8:
	; type=76
	jmp	l8
l10:
l1:
	ret
	.equ	__stack_main,0
.section DATA
	l5:		db	 66
		db	 108
		db	 111
		db	 99
		db	 107
		db	 32
		db	 100
		db	 101
		db	 118
		db	 105
		db	 99
		db	 101
		db	 32
		db	 102
		db	 111
		db	 117
		db	 110
		db	 100
		db	 58
		db	 32
		db	 83
		db	 68
		db	 32
		db	 99
		db	 97
		db	 114
		db	 100
		db	 10
		db	 0
.section DATA
	l2:		db	 76
		db	 111
		db	 97
		db	 100
		db	 105
		db	 110
		db	 103
		db	 46
		db	 46
		db	 46
		db	 10
		db	 0
.section DATA
	l7:		db	 69
		db	 114
		db	 114
		db	 111
		db	 114
		db	 32
		db	 105
		db	 110
		db	 105
		db	 116
		db	 105
		db	 97
		db	 108
		db	 105
		db	 122
		db	 105
		db	 110
		db	 103
		db	 32
		db	 83
		db	 68
		db	 32
		db	 99
		db	 97
		db	 114
		db	 100
		db	 46
		db	 32
		db	 32
		db	 78
		db	 111
		db	 32
		db	 98
		db	 108
		db	 111
		db	 99
		db	 107
		db	 32
		db	 100
		db	 101
		db	 118
		db	 105
		db	 99
		db	 101
		db	 115
		db	 44
		db	 32
		db	 99
		db	 97
		db	 110
		db	 110
		db	 111
		db	 116
		db	 32
		db	 98
		db	 111
		db	 111
		db	 116
		db	 46
		db	 10
		db	 0
	.global	sd_exec
_sd_exec:
	add	sp,-12
	; type=50
	mov	[sp+0], ax
	; type=2
	xor	ax, ax
	mov	[sp+4], ax
	; type=78
	xor	ax, ax
	push	ax
	; type=2
	mov	ax,16
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=50
	mov	ax, [sp+0]
	mov	cx,ax
	; type=16
	mov	ax,cx
	mov	bx,64
	or	ax, bx
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=26
	mov	ax, [sp+16]
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=26
	mov	ax, [sp+16]
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=26
	mov	ax, [sp+16]
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	sar ax
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=18
	mov	ax, [sp+16]
	mov	bx,255
	and	ax, bx
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=50
	mov	ax, [sp+20]
	mov	cx,ax
	; type=78
	mov	ax,cx
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=76
	jmp	l13
l12:
	; type=78
	xor	ax, ax
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=2
	mov	ax,17
	; type=42
	call _in
	; type=93
	mov	cx,ax
	; type=50
	mov	ax,cx
	mov	[sp+8], ax
	; type=18
	mov	ax, [sp+8]
	mov	bx,128
	and	ax, bx
	mov	cx,ax
	; type=77
	mov	ax,cx
	mov	bx,128
	sub	ax, bx
	; type=71
	jnz	l17
l16:
	; type=94
	mov	ax, [sp+8]
	; type=76
	jmp	l11
l17:
l15:
	; type=27
	mov	ax, [sp+4]
	inc	ax
	mov	[sp+4], ax
l13:
	; type=77
	mov	ax, [sp+4]
	mov	bx,64
	sub	ax, bx
	; type=72
	jc	l12
l14:
	; type=94
	xor	ax, ax
l11:
	add	sp,12
	ret
	.equ	__stack_sd_exec,16
	.global	sd_init
_sd_init:
	add	sp,-4
	; type=2
	xor	ax, ax
	mov	[sp+0], ax
	; type=78
	mov	ax,255
	push	ax
	; type=2
	mov	ax,16
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=76
	jmp	l20
l19:
	; type=78
	xor	ax, ax
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
l22:
	; type=27
	mov	ax, [sp+0]
	inc	ax
	mov	[sp+0], ax
l20:
	; type=77
	mov	ax, [sp+0]
	mov	bx,10
	sub	ax, bx
	; type=72
	jc	l19
l21:
	; type=2
	xor	ax, ax
	mov	[sp+0], ax
	; type=76
	jmp	l24
l23:
	; type=78
	mov	ax,149
	push	ax
	; type=78
	xor	ax, ax
	push	ax
	; type=2
	xor	ax, ax
	; type=42
	call _sd_exec
	add	sp, 8
	; s_o+=8
	; type=93
	mov	cx,ax
	; type=50
	mov	ax,cx
	mov	cx,ax
	; type=77
	mov	ax,cx
	mov	bx,0
	sub	ax, bx
	; type=74
	jzc	l28
l27:
	; type=76
	jmp	l25
l28:
	; type=77
	mov	ax, [sp+0]
	mov	bx,9
	sub	ax, bx
	; type=71
	jnz	l31
l30:
	; type=94
	xor	ax, ax
	; type=76
	jmp	l18
l31:
l29:
l26:
	; type=27
	mov	ax, [sp+0]
	inc	ax
	mov	[sp+0], ax
l24:
	; type=77
	mov	ax, [sp+0]
	mov	bx,10
	sub	ax, bx
	; type=72
	jc	l23
l25:
	; type=94
	mov	ax,1
l18:
	add	sp,4
	ret
	.equ	__stack_sd_init,12
.section DATA
	.global	_out
.section DATA
	.global	_in
.section DATA
	.global	_printf
