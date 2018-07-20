# offset=4
	.text
	.global	quata
_quata:
	add	sp,-4
	; type=2
	mov	[sp+0], ax
	; type=27
	mov	ax, [sp+0]
	mov	bx, [sp+8]
	add	ax, bx
	mov	cx,ax
	; type=28
	mov	ax,cx
	mov	bx, [sp+12]
	sub	ax, bx
	mov	cx,ax
	; type=29
	mov	ax,cx
	mov	bx, [sp+16]
	mul	ax, bx
	mov	cx,ax
	; type=94
	mov	ax,cx
l1:
	add	sp,4
	ret
	.type	quata,@function
	.size	quata,$-quata
# stacksize=4
	.equ	__stack_quata,4
# offset=4
	.global	main
_main:
	add	sp,-4
	; type=78
	mov	ax,2
	push	ax
	; type=78
	mov	ax,3
	push	ax
	; type=78
	mov	ax,4
	push	ax
	; type=2
	mov	ax,1
	; type=42
	call _quata
	; type=93
	mov	cx,ax
	; type=2
	mov	ax,cx
	mov	[sp+12], ax
l2:
	add	sp,16
	ret
	.type	main,@function
	.size	main,$-main
# stacksize=16
	.equ	__stack_main,16
