# offset=8
	.text
	.global	main
main:
	leas	-8,sp
	ldax	
	stax		ldax	
	stax		ldax	
	ldbx	
	add	ax, bx
	stax		ldax	
l1:
	@leas	8,sp
	.type	main,@function
	.size	main,$-main
# stacksize=8
	.equ	__stack_main,8
