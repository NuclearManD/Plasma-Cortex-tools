.org 0x80
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
	; type=2
	mov	ax, l12
	; type=42
	call _printf
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
	mov	ax, [sp+23]
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
	jmp	l14
l13:
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
	; type=68
	mov	ax,cx
	or	ax, ax
	; type=70
	jz	l18
	; type=94
	mov	ax, [sp+8]
	; type=76
	jmp	l11
l18:
l16:
	; type=27
	mov	ax, [sp+4]
	inc	ax
	mov	[sp+4], ax
l14:
	; type=77
	mov	ax, [sp+4]
	mov	bx,64
	sub	ax, bx
	; type=72
	jc	l13
l15:
	; type=94
	xor	ax, ax
l11:
	add	sp,12
	ret
	.equ	__stack_sd_exec,16
.section DATA
	l12:		db	 45
		db	 32
		db	 115
		db	 100
		db	 95
		db	 101
		db	 120
		db	 101
		db	 99
		db	 32
		db	 45
		db	 10
		db	 0
	.global	sd_init
_sd_init:
	add	sp,-4
	; type=2
	xor	ax, ax
	mov	[sp+0], ax
	; type=2
	mov	ax, l20
	; type=42
	call _printf
	; type=78
	mov	ax,255
	push	ax
	; type=2
	mov	ax,16
	; type=42
	call _out
	pop bx
	; s_o+=4
	; type=2
	mov	ax, l21
	; type=42
	call _printf
	; type=76
	jmp	l23
l22:
	; type=78
	xor	ax, ax
	push	ax
	; type=2
	mov	ax,17
	; type=42
	call _out
	pop bx
	; s_o+=4
l25:
	; type=27
	mov	ax, [sp+0]
	inc	ax
	mov	[sp+0], ax
l23:
	; type=77
	mov	ax, [sp+0]
	mov	bx,10
	sub	ax, bx
	; type=72
	jc	l22
l24:
	; type=2
	mov	ax, l26
	; type=42
	call _printf
	; type=2
	xor	ax, ax
	mov	[sp+0], ax
	; type=76
	jmp	l28
l27:
	; type=2
	mov	ax, l31
	; type=42
	call _printf
	; type=78
	xor	ax, ax
	push	ax
	; type=78
	mov	ax,149
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
	jzc	l33
l32:
	; type=2
	mov	ax, l34
	; type=42
	call _printf
	; type=76
	jmp	l29
l33:
	; type=77
	mov	ax, [sp+0]
	mov	bx,9
	sub	ax, bx
	; type=71
	jnz	l37
l36:
	; type=2
	mov	ax, l38
	; type=42
	call _printf
	; type=94
	xor	ax, ax
	; type=76
	jmp	l19
l37:
l35:
l30:
	; type=27
	mov	ax, [sp+0]
	inc	ax
	mov	[sp+0], ax
l28:
	; type=77
	mov	ax, [sp+0]
	mov	bx,10
	sub	ax, bx
	; type=72
	jc	l27
l29:
	; type=94
	mov	ax,1
l19:
	add	sp,4
	ret
	.equ	__stack_sd_init,12
.section DATA
	l34:		db	 65
		db	 76
		db	 76
		db	 71
		db	 79
		db	 79
		db	 68
		db	 59
		db	 0
.section DATA
	l38:		db	 101
		db	 114
		db	 114
		db	 111
		db	 114
		db	 33
		db	 10
		db	 0
.section DATA
	l31:		db	 65
		db	 10
		db	 0
.section DATA
	l20:		db	 45
		db	 32
		db	 115
		db	 100
		db	 95
		db	 105
		db	 110
		db	 105
		db	 116
		db	 32
		db	 45
		db	 10
		db	 0
.section DATA
	l21:		db	 99
		db	 108
		db	 111
		db	 99
		db	 107
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
		db	 46
		db	 46
		db	 10
		db	 0
.section DATA
	l26:		db	 99
		db	 111
		db	 109
		db	 109
		db	 117
		db	 110
		db	 105
		db	 99
		db	 97
		db	 116
		db	 105
		db	 110
		db	 103
		db	 46
		db	 46
		db	 46
		db	 10
		db	 0
.section DATA
	.global	_out
.section DATA
	.global	_in
.section DATA
	.global	_printf
