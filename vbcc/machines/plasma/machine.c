#include "supp.h"

char cg_copyright[]="Copyright Dylan Brophy 2018";

zmax char_bit=8;
zmax align[MAX_TYPE+1];
zmax maxalign = 4;
zmax sizetab[MAX_TYPE+1];

zmax t_min[MAX_TYPE+1];
zumax t_min[MAX_TYPE+1];
zumax tu_min[MAX_TYPE+1];

char *regnames[MAXR+1]={"none","ax","bx","cx","dx","si","di","sp","pc"};
zmax regsize[MAXR+1];
int regscratch[MAXR+1]={0,1,1,1,1,0,0,0,0};
int regsa[MAXR+1]={0,0,0,0,0,0,0,0,0};
int reg_prio[MAXR+1]={0,1,1,1,1,1,1,1,1};



int init_cg(void){
	sizetab[CHAR]=l2zm(1L);
	sizetab[SHORT]=l2zm(2L);
	sizetab[INT]=l2zm(4L);
	sizetab[LONG]=l2zm(4L);
	sizetab[LLONG]=l2zm(4L);
	sizetab[FLOAT]=l2zm(4L);
	sizetab[DOUBLE]=l2zm(8L);
	sizetab[LDOUBLE]=l2zm(8L);
	sizetab[VOID]=l2zm(4L);
	sizetab[POINTER]=l2zm(4L);
	sizetab[ARRAY]=l2zm(1L);
	sizetab[STRUCT]=l2zm(1L);
	sizetab[UNION]=l2zm(1L);
	
	align[CHAR]=l2zm(1L);
	align[SHORT]=l2zm(2L);
	align[INT]=l2zm(4L);
	align[LONG]=l2zm(4L);
	align[LLONG]=l2zm(4L);
	align[FLOAT]=l2zm(4L);
	align[DOUBLE]=l2zm(8L);
	align[LDOUBLE]=l2zm(8L);
	align[VOID]=l2zm(4L);
	align[POINTER]=l2zm(4L);
	align[ARRAY]=l2zm(1L);
	align[STRUCT]=l2zm(1L);
	align[UNION]=l2zm(1L);
	
	t_min[CHAR]=l2zm(-128L);
    t_min[SHORT]=l2zm(-32768L);
    t_min[LONG]=zmsub(l2zm(-2147483647L),l2zm(1L));
    t_min[INT]=t_min(LONG);
    t_min[LLONG]=t_min(LONG);
    t_min[MAXINT]=t_min(LLONG);
	
    t_max[CHAR]=ul2zum(127L);
    t_max[SHORT]=ul2zum(32767UL);
    t_max[LONG]=ul2zum(2147483647UL);
    t_max[INT]=t_max(LONG);
    t_max[LLONG]=t_max(LONG);
    t_max[MAXINT]=t_max(LLONG);
	
    tu_max[CHAR]=ul2zum(255UL);
    tu_max[SHORT]=ul2zum(65535UL);
    tu_max[LONG]=ul2zum(4294967295UL);
    tu_max[INT]=tu_max(LONG);
    tu_max[LLONG]=tu_max(LONG);
    tu_max[MAXINT]=tu_max(LLONG);
	
	for(int i=0;i<MAXR+1;i++)
		regsize[i]=
}