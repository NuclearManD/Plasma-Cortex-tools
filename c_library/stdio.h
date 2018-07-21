#ifndef STDIO_H
#define STDIO_H

#define NULL 0

extern void printf(const char * format, ...);
extern int remove(const char *filename);
extern int rename(const char *old_filename, const char *new_filename);
extern int sprintf(char *str, const char *format, ...);
extern int getchar();
extern char *gets(char *str);

#endif