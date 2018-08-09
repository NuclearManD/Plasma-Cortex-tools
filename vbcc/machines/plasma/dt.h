

/* Machine generated file. DON'T TOUCH ME! */


#ifndef DT_H
#define DT_H 1
typedef signed char zchar;
typedef unsigned char zuchar;
typedef struct {char a[2];} dt3f;
typedef dt3f zshort;
typedef signed short dt3t;
dt3t dtcnv3f(dt3f);
dt3f dtcnv3t(dt3t);
typedef struct {char a[2];} dt4f;
typedef dt4f zushort;
typedef unsigned short dt4t;
dt4t dtcnv4f(dt4f);
dt4f dtcnv4t(dt4t);
typedef struct {char a[4];} dt5f;
typedef dt5f zint;
typedef signed int dt5t;
dt5t dtcnv5f(dt5f);
dt5f dtcnv5t(dt5t);
typedef struct {char a[4];} dt6f;
typedef dt6f zuint;
typedef unsigned int dt6t;
dt6t dtcnv6f(dt6f);
dt6f dtcnv6t(dt6t);
typedef struct {char a[4];} dt7f;
typedef dt7f zlong;
typedef signed int dt7t;
dt7t dtcnv7f(dt7f);
dt7f dtcnv7t(dt7t);
typedef struct {char a[4];} dt8f;
typedef dt8f zulong;
typedef unsigned int dt8t;
dt8t dtcnv8f(dt8f);
dt8f dtcnv8t(dt8t);
typedef struct {char a[4];} dt9f;
typedef dt9f zllong;
typedef signed int dt9t;
dt9t dtcnv9f(dt9f);
dt9f dtcnv9t(dt9t);
typedef struct {char a[4];} dt10f;
typedef dt10f zullong;
typedef unsigned int dt10t;
dt10t dtcnv10f(dt10f);
dt10f dtcnv10t(dt10t);
typedef struct {char a[4];} dt11f;
typedef dt11f zfloat;
typedef float dt11t;
dt11t dtcnv11f(dt11f);
dt11f dtcnv11t(dt11t);
typedef struct {char a[8];} dt12f;
typedef dt12f zdouble;
typedef double dt12t;
dt12t dtcnv12f(dt12f);
dt12f dtcnv12t(dt12t);
typedef struct {char a[8];} dt13f;
typedef dt13f zldouble;
typedef double dt13t;
dt13t dtcnv13f(dt13f);
dt13f dtcnv13t(dt13t);
typedef struct {char a[4];} dt14f;
typedef dt14f zpointer;
typedef unsigned int dt14t;
dt14t dtcnv14f(dt14f);
dt14f dtcnv14t(dt14t);
#define zc2zm(x) dtcnv9t((signed int)(x))
#define zs2zm(x) dtcnv9t((signed int)dtcnv3f(x))
#define zi2zm(x) dtcnv9t((signed int)dtcnv5f(x))
#define zl2zm(x) dtcnv9t((signed int)dtcnv7f(x))
#define zll2zm(x) dtcnv9t((signed int)dtcnv9f(x))
#define zm2zc(x) ((signed char)dtcnv9f(x))
#define zm2zs(x) dtcnv3t((signed short)dtcnv9f(x))
#define zm2zi(x) dtcnv5t((signed int)dtcnv9f(x))
#define zm2zl(x) dtcnv7t((signed int)dtcnv9f(x))
#define zm2zll(x) dtcnv9t((signed int)dtcnv9f(x))
#define zuc2zum(x) dtcnv10t((unsigned int)(x))
#define zus2zum(x) dtcnv10t((unsigned int)dtcnv4f(x))
#define zui2zum(x) dtcnv10t((unsigned int)dtcnv6f(x))
#define zul2zum(x) dtcnv10t((unsigned int)dtcnv8f(x))
#define zull2zum(x) dtcnv10t((unsigned int)dtcnv10f(x))
#define zum2zuc(x) ((unsigned char)dtcnv10f(x))
#define zum2zus(x) dtcnv4t((unsigned short)dtcnv10f(x))
#define zum2zui(x) dtcnv6t((unsigned int)dtcnv10f(x))
#define zum2zul(x) dtcnv8t((unsigned int)dtcnv10f(x))
#define zum2zull(x) dtcnv10t((unsigned int)dtcnv10f(x))
#define zum2zm(x) dtcnv9t((signed int)dtcnv10f(x))
#define zm2zum(x) dtcnv10t((unsigned int)dtcnv9f(x))
#define zf2zld(x) dtcnv13t((double)dtcnv11f(x))
#define zd2zld(x) dtcnv13t((double)dtcnv12f(x))
#define zld2zf(x) dtcnv11t((float)dtcnv13f(x))
#define zld2zd(x) dtcnv12t((double)dtcnv13f(x))
#define zld2zm(x) dtcnv9t((signed int)dtcnv13f(x))
#define zm2zld(x) dtcnv13t((double)dtcnv9f(x))
#define zld2zum(x) dtcnv10t((unsigned int)dtcnv13f(x))
#define zum2zld(x) dtcnv13t((double)dtcnv10f(x))
#define zp2zum(x) dtcnv10t((unsigned int)dtcnv14f(x))
#define zum2zp(x) dtcnv14t((unsigned int)dtcnv10f(x))
#define l2zm(x) dtcnv9t((signed int)(x))
#define ul2zum(x) dtcnv10t((unsigned int)(x))
#define d2zld(x) dtcnv13t((double)(x))
#define zm2l(x) ((long)dtcnv9f(x))
#define zum2ul(x) ((unsigned long)dtcnv10f(x))
#define zld2d(x) ((double)dtcnv13f(x))
#define zmadd(a,b) dtcnv9t(dtcnv9f(a)+dtcnv9f(b))
#define zumadd(a,b) dtcnv10t(dtcnv10f(a)+dtcnv10f(b))
#define zldadd(a,b) dtcnv13t(dtcnv13f(a)+dtcnv13f(b))
#define zmsub(a,b) dtcnv9t(dtcnv9f(a)-dtcnv9f(b))
#define zumsub(a,b) dtcnv10t(dtcnv10f(a)-dtcnv10f(b))
#define zldsub(a,b) dtcnv13t(dtcnv13f(a)-dtcnv13f(b))
#define zmmult(a,b) dtcnv9t(dtcnv9f(a)*dtcnv9f(b))
#define zummult(a,b) dtcnv10t(dtcnv10f(a)*dtcnv10f(b))
#define zldmult(a,b) dtcnv13t(dtcnv13f(a)*dtcnv13f(b))
#define zmdiv(a,b) dtcnv9t(dtcnv9f(a)/dtcnv9f(b))
#define zumdiv(a,b) dtcnv10t(dtcnv10f(a)/dtcnv10f(b))
#define zlddiv(a,b) dtcnv13t(dtcnv13f(a)/dtcnv13f(b))
#define zmmod(a,b) dtcnv9t(dtcnv9f(a)%dtcnv9f(b))
#define zummod(a,b) dtcnv10t(dtcnv10f(a)%dtcnv10f(b))
#define zmlshift(a,b) dtcnv9t(dtcnv9f(a)<<dtcnv9f(b))
#define zumlshift(a,b) dtcnv10t(dtcnv10f(a)<<dtcnv10f(b))
#define zmrshift(a,b) dtcnv9t(dtcnv9f(a)>>dtcnv9f(b))
#define zumrshift(a,b) dtcnv10t(dtcnv10f(a)>>dtcnv10f(b))
#define zmand(a,b) dtcnv9t(dtcnv9f(a)&dtcnv9f(b))
#define zumand(a,b) dtcnv10t(dtcnv10f(a)&dtcnv10f(b))
#define zmor(a,b) dtcnv9t(dtcnv9f(a)|dtcnv9f(b))
#define zumor(a,b) dtcnv10t(dtcnv10f(a)|dtcnv10f(b))
#define zmxor(a,b) dtcnv9t(dtcnv9f(a)^dtcnv9f(b))
#define zumxor(a,b) dtcnv10t(dtcnv10f(a)^dtcnv10f(b))
#define zmmod(a,b) dtcnv9t(dtcnv9f(a)%dtcnv9f(b))
#define zummod(a,b) dtcnv10t(dtcnv10f(a)%dtcnv10f(b))
#define zmkompl(a) dtcnv9t(~dtcnv9f(a))
#define zumkompl(a) dtcnv10t(~dtcnv10f(a))
#define zmleq(a,b) (dtcnv9f(a)<=dtcnv9f(b))
#define zumleq(a,b) (dtcnv10f(a)<=dtcnv10f(b))
#define zldleq(a,b) (dtcnv13f(a)<=dtcnv13f(b))
#define zmeqto(a,b) (dtcnv9f(a)==dtcnv9f(b))
#define zumeqto(a,b) (dtcnv10f(a)==dtcnv10f(b))
#define zldeqto(a,b) (dtcnv13f(a)==dtcnv13f(b))
#endif
