#include "dt.h"
struct AddressingMode {
    int never_used;
};

#define MAXR 8
#define MAXGF 1


#define USEQ2ASZ 1
#define MINADDI2P zchar
#define MAXADDI2P zlong
#define BIGENDIAN 1
#define LITTLEENDIAN 0
#define SWITCHSUBS 1
#define INLINEMEMCPY 1024*1024
#define ORDERED_PUSH 0

#define HAVE_REGPARMS 1

#define EMIT_BUF_LEN 128
#define EMIT_BUF_DEPTH 1
#define HAVE_TARGET_PEEPHOLE 0

#define JUMP_TABLE_DENSITY 2
#define JUMP_TABLE_LENGTH 16
//These values controls the creation of jump-tables (see section Jump Tables).

#define ALLOCVLA_REG 0
#define ALLOCVLA_INLINEASM <inline-asm>
#define FREEVLA_REG 0
#define FREEVLA_INLINEASM <inline-asm>
#define OLDSPVLA_INLINEASM <inline-asm>
#define FPVLA_REG 0
//Necessary defines for C99 variable-length-arrays.