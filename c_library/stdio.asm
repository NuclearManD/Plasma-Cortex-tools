
;extern void printf(const char * format, ...);
.equ _printf 5 ; printf offset in OS memory
;extern int remove(const char *filename);
;.equ remove 
;extern int rename(const char *old_filename, const char *new_filename);
;extern int sprintf(char *str, const char *format, ...);
;extern int getchar();
.equ _getchar 10
;extern char *gets(char *str);
.equ _gets 15