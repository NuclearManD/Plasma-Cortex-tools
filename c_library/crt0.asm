.org 0x10000
.section CODE
.global _start
_start:
	
	; init standard library here if needed
	
	; call init_cstdlib
	
	;call _init	; call global constructors
	
	mov ax, cx	; cx has argc
	push si		; si has argv
	call _main
	add sp, 4	; restore stack location
	
	;push ax		; save ax
	;call _fini	; call global deconstructors
	;pop ax		; restore ax
	
	ret ; OS will expect exit value in ax register, as returned from _main
	
.size _start $ - _start
