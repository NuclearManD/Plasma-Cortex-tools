#include "dt.h"
struct AddressingMode {
    int never_used;
};

#define MAXR 8
#define MAXGF 1


#define USEQ2ASZ 1
#define MINADDI2P INT
#define MAXADDI2P LONG
#define BIGENDIAN 1
#define LITTLEENDIAN 0
#define SWITCHSUBS 1
#define INLINEMEMCPY 1024*1024
#define ORDERED_PUSH 0

#define HAVE_REGPARMS 1

struct reg_handle {
  int gpr;
};
#define EMIT_BUF_LEN 128
#define EMIT_BUF_DEPTH 1
#define HAVE_TARGET_PEEPHOLE 0

#define JUMP_TABLE_DENSITY 2
#define JUMP_TABLE_LENGTH 16
//These values controls the creation of jump-tables (see section Jump Tables).

#define ALLOCVLA_REG 0
//#define ALLOCVLA_INLINEASM <inline-asm>
#define FREEVLA_REG 0
//#define FREEVLA_INLINEASM <inline-asm>
//#define OLDSPVLA_INLINEASM <inline-asm>
#define FPVLA_REG 0
//Necessary defines for C99 variable-length-arrays.

#undef CHAR
#undef SHORT
#undef INT
#undef LONG
#undef LLONG
#undef FLOAT
#undef DOUBLE
#undef LDOUBLE
#undef VOID
#undef POINTER
#undef ARRAY
#undef STRUCT
#undef UNION
#undef ENUM
#undef FUNKT
#undef MAXINT
#undef MAX_TYPE

#define BIT 1
#define CHAR 2
#define SHORT 3
#define INT 4
#define LONG 5
#define LLONG 6
#define FLOAT 7
#define DOUBLE 8
#define LDOUBLE 9
#define VOID 10
#define POINTER 11
#define ARRAY 12
#define STRUCT 13
#define UNION 14
#define ENUM 15
#define FUNKT 16

#define MAXINT 17

#define MAX_TYPE MAXINT

typedef zllong zmax;
typedef zullong zumax;
