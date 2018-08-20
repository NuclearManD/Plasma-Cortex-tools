.org 0
_bios_start:
	mov sp, 0xFFFF
	call _main
	mov ax, bios_term_errmsg
	call _printf
	mov pc, pc
bios_term_errmsg:
"Error: BIOS exited unexpectedly.  Could not load DOOM."
db 10
db 0
_printf:
	push si
	push ax
	push dx
	mov si, ax
	mov dx, 0
serwrt_loop:
	lodsb
	or ax, ax
	jz serwrt_done
	out dx, ax
	jmp serwrt_loop
serwrt_done:
	pop dx
	pop ax
	pop si
	ret
_in:
	mov dx, ax
	in ax, dx
	ret
_out:
	push dx
	mov dx, ax
	mov ax, [sp+8]
	out dx, ax
	pop dx
	ret
_memcpy:
	push di
	push si
	push cx
	mov di, ax
	mov si, [sp+16]
	mov cx, [sp+20]
memcpy_lopen:
	lodsb
	stosb
	djnz memcpy_lopen
	pop cx
	pop si
	pop di
	ret