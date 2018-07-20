# offset=0
	.text
	.global	main
_main:
	mov	di, l2
	mov	ax, [di]
	call _printf
	mov	cx,ax
l1:
	ret
	.type	main,@function
	.size	main,$-main
# stacksize=0
	.equ	__stack_main,0
.section DATA
	l2:		db	 72
		db	 101
		db	 108
		db	 108
		db	 111
		db	 32
		db	 87
		db	 111
		db	 114
		db	 108
		db	 100
		db	 33
		db	 0
.section DATA
	global	_printf
