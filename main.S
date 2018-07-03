; test program

_main:
	mov sp, 0x02FFFF ; end of first RAM sector
	mov si, _str
	call _print
	mov cx, 0xFFFFFFFF ; wait 4 billion instructions (less than 3 minutes at 100 Mhz)
	djnz $
	jmp _main
	
_str:
	"Hello world!!"
	db 0

_print:
	push ax
	push si
	xor ax
_print_loop:
	lodsb
	or ax
	jz _print_done
	out 0, ax
	jmp _print_loop
_print_done:
	pop si
	pop ax
	ret