	;function_top
# offset=8
	.text
	.global	main
main:
	add	sp,8
	mov	ax,3
	push	ax
	mov	ax,453
	add	sp, 8
	push	ax
	add	sp, 4
	pop	ax
	mov	bx,33
	mul	ax, bx
	mov	cx,ax
	mov	ax,cx
	add	sp, -8
	pop	bx
	add	ax, bx
	push	ax
	add	sp, 4
	pop	ax
l1:
	;function_bottom
	add	sp,-8
	.type	main,@function
	.size	main,$-main
# stacksize=12
	.equ	__stack_main,12
