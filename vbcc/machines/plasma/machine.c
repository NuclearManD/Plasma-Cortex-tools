/*

	Assembly Generator for the Nuclaer Tech Plasma Cortex, a VHDL soft processor for FPGAs.
	Outputs NASM syntax, meant to be used with as.exe (on windows), or the respective program on Linux (not yet built).

	Assembly Generator für den Nuclaer Tech Plasma Cortex, einen VHDL Softprozessor für FPGAs.
	Gibt die NASM-Syntax aus, die mit as.exe (unter Windows) verwendet werden soll, oder das entsprechende Programm unter Linux (noch nicht erstellt).
	Bitte vergib Fehlübersetzungen.
*/
#include "supp.h"
#include "vbc.h" /* nicht schoen, aber ... */

static char FILE_[]=__FILE__;

//#include "dwarf2.c"
#define acc 1
#define DATA 0
#define BSS 1
#define CODE 2
#define RODATA 3
#define SPECIAL 4

#define isreg(x) ((p->x.flags&(REG|DREFOBJ))==REG)
#define isconst(x) ((p->x.flags&(KONST|DREFOBJ))==KONST)

#define ISNULL() (zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&zldeqto(vldouble,d2zld(0.0)))
//#define ISINT(t) ((t&NQ)==INT||(t&NQ)==LONG||(t&NQ)==POINTER||(t&NQ)==FLOAT)
#define ISWORD(t) ((t&NQ)==SHORT)
#define ISSTATIC(v) ((v)->storage_class==EXTERN||(v)->storage_class==STATIC)
#define ISBADDR(v) ((v)->vtyp->attr&&strstr(STR_BADDR,(v)->vtyp->attr))

char cg_copyright[]="Copyright Dylan Brophy 2018";

zmax char_bit=8;
zmax align[MAX_TYPE+1];
zmax maxalign = 4;
zmax sizetab[MAX_TYPE+1];

zmax t_min[MAX_TYPE+1];
zumax t_max[MAX_TYPE+1];
zumax tu_max[MAX_TYPE+1];
//zumax tu_min[MAX_TYPE+1];

char *regnames[MAXR+1]={"none","ax","bx","cx","dx","si","di","sp","pc"};
zmax regsize[MAXR+1];
int regscratch[MAXR+1]={0,1,1,1,1,1,0,0,0};
int regsa[MAXR+1]={1,0,0,0,0,1,1,1,1};
int reg_prio[MAXR+1]={0,1,1,1,1,1,1,1,1};
struct Typ *regtype[MAXR+1];

struct reg_handle empty_reg_handle={0};

int g_flags[MAXGF]={STRINGFLAG};
char *g_flags_name[MAXGF]={"off"};
union ppi g_flags_val[MAXGF];

/*

private functions/data

much of this is from the hc12 backend.
*/

struct Typ regType={INT};

static int section=-1,newobj,scnt,pushed_acc;
static char *codename="\t.text\n",
						*dataname="\t.data\n",
						*bssname="\t.section\t.bss\n",
						*rodataname="\t.section\t.rodata\n";

static long loff,stackoffset,notpopped,dontpop,maxpushed,stack;
static char *x_t[]={"?","","b","","","","","","","","","","","","","",""};
static char *logicals[]={"or","xor","and"};
static char *dct[]={"",".bit",".byte",".2byte",".2byte",".4byte",".8byte",".4byte",".8byte",".8byte",
				"(void)",".2byte",".34byte",".34byte"};
static char *idprefix="",*labprefix="l";
static int exit_label,have_frame;
static char *ret;
static int stackchecklabel;
static int frame_used,stack_valid;

static int cc_t;
static struct obj *cc;

static struct obj mobj;

// this will have to be modified
static char *ccs[]={"z","nz","c","nc","LessThanOrEqualTo","GreaterThan"}; // signed?
static char *uccs[]={"z","nz","c","nc","LessThan|EqualTo","GreaterThan"};// unsigned?

static long voff(struct obj *p)
{
	if(zm2l(p->v->offset)<0) 
		return loff-zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset;
	else
		return zm2l(p->v->offset)+zm2l(p->val.vmax)-stackoffset;
}
static void pop(long l)
{
	stackoffset+=l;
}
static void push(long l)
{
	stackoffset-=l;
	if(stackoffset<maxpushed) maxpushed=stackoffset;
	if(-maxpushed>stack) stack=-maxpushed;
}
int gen_already=0;
static void function_top(FILE *f,struct Var *v,long offset)
/*	erzeugt Funktionskopf											 */
{
	char* asm_top="0x10080"; /* leave 128 bytes for crt0.po */
	if(gen_already==0){
		if(g_flags[0]&USEDFLAG){
			asm_top=g_flags_val[0].p;
		}
		gen_already=1;
		emit(f, ".org %s\n", asm_top);
	}
	//emit(f,"\t;function_top\n");
	int i;
	//emit(f,"# offset=%ld\n",offset);
	have_frame=0;stack_valid=1;stack=0;
	//if(section!=CODE){emit(f,codename);if(f) section=CODE;}
	if(v->storage_class==EXTERN){
		if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
			emit(f,"\t.global\t%s\n",v->identifier);
		emit(f,"_%s:\n",v->identifier);
	}else{
		emit(f,".%ld:\n",zm2l(v->offset));
	}
	if(offset){
		if(offset==1)
			emit(f,"\tinc\tsp\n");
		else if(offset==2)
			emit(f,"\tinc\tsp\n\tinc\tsp\n");
		else
			emit(f,"\tadd\tsp,-%d\n",offset);
		have_frame=1;
	}
}
// this function makes the stack offset zero.
static void zerostack(FILE* f, long off) {
	if(off==0)return;
	emit(f,"\tadd\tsp, %d\n",off);
	stackoffset+=off;
}
static void load_obj(FILE *f,struct obj *p,int t,int reg)
/*	Gibt Objekt auf Bildschirm aus											*/
{
	//emit(f,"\t;load_obj\n");
	//emit(f,"!");
	if((p->flags&(DREFOBJ|REG))==DREFOBJ) ;//emit(f,"[");
	else if((p->flags&(VAR|REG))==VAR){
		if(isextern(p->v->storage_class)){
			emit(f,"\tmov\tdi, _%s",p->v->identifier);
			emit(f,"+%d\n",zm2l(p->val.vmax));
			emit(f,"\tmov\t%s, [di]\n",regnames[reg]);
		}else if(isstatic(p->v->storage_class)){
			emit(f,"\tmov\tdi, l%d\n",p->v->offset);
			emit(f,"\tmov\t%s, [di]\n",regnames[reg]);
		}else
			emit(f,"\tmov\t%s, [sp+%d]\n",regnames[reg],voff(p));
	}
	else if(p->flags&REG){
		emit(f,"\tmov\t%s, %s\n",regnames[reg],regnames[p->reg]);
	}
	else if(p->flags&KONST){
		/*if(ISFLOAT(t)){
			emit(f,"%s%d",labprefix,addfpconst(p,t));
		}else{*/
			emit(f,"\tmov\t%s,%ld\n",regnames[reg],p->val);
		//}
	}
	//p->v->identifier
}
static void store_obj(FILE *f,struct obj *p,int t, int r)
/*	Gibt Objekt auf Bildschirm aus											*/
{
	//emit(f,"\t;store_obj\n");
	//if((p->flags&(DREFOBJ|REG))==DREFOBJ) emit(f,"[");
	if((p->flags&(VAR|REG))==VAR){
		if(isextern(p->v->storage_class)){
			emit(f,"\tmov\tdi, _%s",p->v->identifier);
			emit(f,"+%d\n",zm2l(p->val.vmax));
			emit(f,"\tmov\t[di], %s\n",regnames[r]);
		}else if(isstatic(p->v->storage_class)){
			emit(f,"\tmov\tdi, %u\n",p->v->offset);
			emit(f,"\tmov\t[di], %s\n",regnames[r]);
		}else
			emit(f,"\tmov\t[sp+%d], %s\n",voff(p),regnames[r]);
	}
	if(p->flags&REG){
		emit(f,"\tmov\t%s, %s\n",regnames[p->reg],regnames[r]);
	}
	if(p->flags&KONST){
		ierror(0);
	}
	if((p->flags&(DREFOBJ|REG))==DREFOBJ){
		//if(p->v->storage_class==EXTERN||p->v->storage_class==STATIC)
			//emit(f,",pc");
		//emit(f,"]");
	}
}
static void function_bottom(FILE *f,struct Var *v,long offset)
/*	erzeugt Funktionsende											 */
{
	//emit(f,"\t;function_bottom\n");
	int i;
	if(offset){
		if(offset==1)
			emit(f,"\tdec sp\n");
		else if(offset==2)
			emit(f,"\tdec sp\n\tdec sp\n");
		else
			emit(f,"\tadd\tsp,%d\n",offset-stackoffset);
	}
	emit(f,"\tret\n");
	if(v->storage_class==EXTERN){
		//emit(f,"\t.type\t%s%s,@function\n",idprefix,v->identifier);
		//emit(f,"\t.size\t%s%s,$-%s%s\n",idprefix,v->identifier,idprefix,v->identifier);
	}else{
		//emit(f,"\t.type\t%s%ld,@function\n",labprefix,zm2l(v->offset));
		//emit(f,"\t.size\t%s%ld,$-%s%ld\n",labprefix,zm2l(v->offset),labprefix,zm2l(v->offset));
	}
	if(stack_check)
		emit(f,"\t.equ\t%s%d,%ld\n",labprefix,stackchecklabel,offset-maxpushed);
	if(stack_valid){
		if(!v->fi) v->fi=new_fi();
		v->fi->flags|=ALL_STACK;
		v->fi->stack1=l2zm(stack+offset);
		//emit(f,"# stacksize=%ld\n",stack+offset);
		emit(f,"\t.equ\t%s__stack_%s,%ld\n",idprefix,v->identifier,stack+offset);
	}
}

int freturn(struct Typ *t)
/*	Returns the register in which variables of type t are returned. */
/*	If the value cannot be returned in a register returns 0.				*/
{
	return 1;
}
/* pushed on the stack by a callee, no pop needed */
static void callee_push(long l)
{
	if(l-stackoffset>stack)
		stack=l-stackoffset;
}
// I think that l is the number of bytes to pop.
static void gen_pop(FILE *f,long l)
{
	if(l==0) return;
	if((l&3)>0){
		ierror(0); // can only pop 4 bytes at a time.	No more, no less.
	}else if(l==4&&!regs[1]){
		emit(f,"\tpop ax\n");
		BSET(regs_modified,1);
	}else if(l==4&&!regs[2]){
		emit(f,"\tpop bx\n");
		BSET(regs_modified,2);
	}else if(l==4&&!regs[3]){
		emit(f,"\tpop cx\n");
		BSET(regs_modified,3);
	}else if(l==4&&!regs[4]){
		emit(f,"\tpop dx\n");
		BSET(regs_modified,4);
	}else{
		emit(f,"\tadd\tsp, %ld\n",l);
		return;
	}
	pop(l);
}
static void load_reg(FILE *f,int r,struct obj *o,int t)
{
	if((o->flags&(REG|DREFOBJ))==REG){
		if(o->reg==r) return;
		emit(f,"\tmov\t%s,%s\n",regnames[r],regnames[o->reg]);
		return;
	}
	if(r==1&&(o->flags&(KONST|DREFOBJ))==KONST){
		eval_const(&o->val,t);
		if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&r==1){
			emit(f,"\txor\tax\n");
			return;
		}
	}
	load_obj(f,o,t,r);
}
static void store_reg(FILE *f,int r,struct obj *o,int t)
{
	if((o->flags&(REG|DREFOBJ))==REG){
		if(o->reg==r) return;
		emit(f,"\tmov\t%s,%s\n",regnames[o->reg],regnames[r]);
	}else{
		store_obj(f,o,t,r);
	}
}

/*

end private functions

*/


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
	t_min[INT]=t_min[LONG];
	t_min[LLONG]=t_min[LONG];
	t_min[MAXINT]=t_min[LLONG];
	
	t_max[CHAR]=ul2zum(127L);
	t_max[SHORT]=ul2zum(32767UL);
	t_max[LONG]=ul2zum(2147483647UL);
	t_max[INT]=t_max[LONG];
	t_max[LLONG]=t_max[LONG];
	t_max[MAXINT]=t_max[LLONG];
	
	tu_max[CHAR]=ul2zum(255UL);
	tu_max[SHORT]=ul2zum(65535UL);
	tu_max[LONG]=ul2zum(4294967295UL);
	tu_max[INT]=tu_max[LONG];
	tu_max[LLONG]=tu_max[LONG];
	tu_max[MAXINT]=tu_max[LLONG];
	
	for(int i=0;i<MAXR+1;i++){
		regsize[i]=l2zm(4L);
		regtype[i]=&regType;
	}
	return 1;
}
void cleanup_cg(FILE *f){
	
}
int regok(int r, int t, int mode){
	if(r>6||r==1||r==2)return 0; // special registers
	if(t==DOUBLE||t==LDOUBLE)return 0; // not 64 bit
	return 1; // all good!
}

int dangerous_IC(struct IC *p){
	return 0;
}
int must_convert(int from,int to,int const_expr){
	int it=from&NQ,ot=to&NQ;
	if(it==ot)return 0; // same type besides signed/unsigned
	if((it==DOUBLE||it==LDOUBLE)&&(ot==DOUBLE||ot==LDOUBLE)) return 0; // both doubles
	if((it==LONG||it==LLONG||it==INT)&&(it==LONG||it==LLONG||it==INT)) return 0; // same size (32 bits)
	if(it==SHORT&&ot==SHORT) return 0;
	if(it==POINTER&&ot==POINTER) return 0;
	if(it==CHAR&&ot==CHAR) return 0;
	return 1;
}
int shortcut(int code, int t){
	return 0;
}

/*	The main code-generation routine.					*/
/*	f is the stream the code should be written to.		*/
/*	p is a pointer to a doubly linked list of ICs		*/
/*	containing the function body to generate code for.	*/
/*	v is a pointer to the function.						*/
/*	offset is the size of the stackframe the function	*/
/*	needs for local variables.							*/
void gen_code(FILE *f,struct IC *p,struct Var *v,zmax offset)
{
	int c,t,i,lastcomp=0,reg,short_add,bit_reverse,need_return=0;
	struct obj *bit_obj;char *bit_reg;
	static int idone;
	struct obj o;
	struct IC *p2;
	for(c=1;c<=MAXR;c++) regs[c]=regsa[c];	// regs[] not well defined in docs.
	
	for(c=1;c<=MAXR;c++){ // seems like this code checks for register modification and notifies the rest of the backend.
		if(regscratch[c]&&(regsa[c]||regused[c])){
			BSET(regs_modified,c);
		}
	}
	
	loff=zm2l(offset); // offset as a long
	if(loff&3!=0)
		loff+=4-loff&3;
	
	function_top(f,v,loff);
	stackoffset=notpopped=dontpop=maxpushed=0;
	for(;p;p=p->next){
		c=p->code;t=p->typf;
		short_add=0;
		if(c==NOP) continue;
		if(c==ALLOCREG){
			regs[p->q1.reg]=1;
			continue;
		}
		if(c==FREEREG){
			regs[p->q1.reg]=0;
			continue;
		}
		if(c==LABEL) {
			emit(f,"l%d:\n",t);
			continue;
		}
		emit(f,"\t; type=%d\n",c);
		if(c>=BEQ&&c<=BGT&&t==exit_label)
			need_return=1;
		if(c==BRA){
			emit(f,"\tjmp\tl%d\n",t);
			continue;
		}
		if(c>=BEQ&&c<BRA){			
			if(t&UNSIGNED)
				emit(f,"\tj%s\tl%d\n",uccs[c-BEQ],t);
			else
				emit(f,"\tj%s\tl%d\n",ccs[c-BEQ],t);
			continue;
		}
		if(c==MOVETOREG){
			load_reg(f,p->z.reg,&p->q1,INT);
			continue;
		}
		if(c==MOVEFROMREG){
			store_reg(f,p->q1.reg,&p->z,INT);
			continue;
		}
		if((t&NQ)==BIT){
			ierror(0);
		}

		if(c==TEST){
			load_reg(f,acc,&p->q1,p->typf2&NU);
			emit(f,"\tor\tax\n");
			continue;
		} 
		if(c==COMPARE&&isconst(q1)){
			if(isreg(q2)){
				emit(f,"\tsub\t%s, %d",regnames[p->q2.reg],p->q1.val.vmax);
				continue;
			}
		}
		if(c==COMPARE&&isconst(q2)){
			if(isreg(q1)){
				emit(f,"\tsub\t%s, %d",regnames[p->q1.reg],p->q2.val.vmax);
				continue;
			}
		}
		switch_IC(p);
		if(c==CONVERT){
			int to=p->typf2&NU;
			if(to==POINTER) to=(UNSIGNED|INT);
			if((to&NQ)<=LONG&&(t&NQ)<=LONG){
				if((to&NQ)!=(t&NQ)){
					load_reg(f,acc,&p->q1,to);
					store_reg(f,acc,&p->z,t);
					continue;
				}else{
					c=ASSIGN;
					p->q2.val.vmax=sizetab[t&NQ];
				}
			}
		}
		if(c==KOMPLEMENT){
			load_reg(f,acc,&p->q1,t);
			emit(f,"\tnot ax\n");
			store_reg(f,acc,&p->z,t);
			continue;
		}
		if(c==MINUS){
			load_reg(f,acc,&p->q1,t);
			emit(f,"\tneg ax\n");
			store_reg(f,acc,&p->z,t);
			continue;
		}
		if(c==SETRETURN){
			if(p->z.reg){
				load_reg(f,p->z.reg,&p->q1,t);
				BSET(regs_modified,p->z.reg);
			}
			continue;
		}
		if(c==GETRETURN){
			if(p->q1.reg){
				store_reg(f,p->q1.reg,&p->z,t);
			}
			continue;
		}
		if(c==CALL){
			int reg,jmp=0;
			if((p->flags&(DREFOBJ|REG))==DREFOBJ) ierror(0);
			else if((p->flags&(VAR|REG))==VAR){
				emit(f,"\tmov\tax, [sp+%d]\n",voff(&p->q1));
				//pop(4);
			}
			else if(p->flags&REG){
				emit(f,"\tcall\t%s\n",regnames[(&p->q1)->reg]);
			}
			else if(p->flags&KONST){
				/*if(ISFLOAT(t)){
					emit(f,"%s%d",labprefix,addfpconst(p,t));
				}else{*/
					emit(f,"\tcall %d\n",(&p->q1)->val);
				//}
			}else{
				if(isextern(p->q1.v->storage_class)){
					emit(f,"\tcall _%s\n",p->q1.v->identifier);
				}else{
					load_obj(f,&p->q1,POINTER, acc);
					emit(f,"\tcall ax\n");
				}
			}
			//if(p->arg_cnt)
			continue;
		}if(c==ASSIGN){
			if((&p->q1)->flags&(VAR|REG|DREFOBJ)==REG&&(&p->z)->flags&(VAR|REG|DREFOBJ)==REG&&(&p->q1)->reg==(&p->z)->reg){
				continue;
			}else{
				load_reg(f,acc,&p->q1,t);
				store_reg(f,acc,&p->z,t);
			}
			continue;
		}
		if(c==PUSH){
			load_reg(f,acc,&p->q1,t);
			emit(f,"\tpush\tax\n");
			push(4);
			continue;
		}
		if(c==ADDRESS){
			int px=0;
			if(isreg(z)){
				reg=p->z.reg;
			}else if(!regs[3]){
				reg=3;
			}else if(!regs[4]){
				reg=4;
			}else{
				/*FIXME: test if x used in q1 */
				px=1;
				emit(f,"\tpush dx\n");
				reg=4;
				push(4);
			}
			//load_addr(f,reg,&p->q1);
			if(!(p->z.flags&REG)||p->z.reg!=reg)
				store_reg(f,reg,&p->z,p->typf2);
			if(px){
				emit(f,"\tpop dx\n");
				pop(4);
			}
			continue;
		}
		if((c==MULT||c==DIV||(c==MOD&&(p->typf&UNSIGNED)))&&isconst(q2)){
			/*long ln;
			eval_const(&p->q2.val,t);
			if(zmleq(l2zm(0L),vmax)&&zumleq(ul2zum(0UL),vumax)){
				if((ln=pof2(vumax))&&ln<5){
					if(c==MOD){
						vmax=zmsub(vmax,l2zm(1L));
						c=p->code=c=AND;
					}else{
						vmax=l2zm(ln-1);
						if(c==DIV) p->code=c=RSHIFT; else p->code=c=LSHIFT;
					}
					c=p->code;
					gval.vmax=vmax;
					eval_const(&gval,MAXINT);
					if(c==AND){
						insert_const(&p->q2.val,t);
					}else{
						insert_const(&p->q2.val,t);
						p->typf2=INT;
					}
				}
			}*/
		}
		if(c==MOD||c==DIV){
			ierror(0);
			continue;
		}
		if(c==LSHIFT){
			if(isconst(q2)&&p->q2.val.vmax>2){
				// in this case multiply is faster than lshift (mul ax, * => ~12 cycles, 8 cycles per lshift)
				p->q2.val.vmax=(1<<p->q2.val.vmax);
				c=MULT;
			}else if(isconst(q2)){
				if(p->q2.val.vmax==0) continue; // optimize out; does nothing
				load_reg(f,acc,&p->q1,t);
				if(p->q2.val.vmax==1){
					emit(f,"\tshl ax\n");
				}
				if(p->q2.val.vmax==2){
					emit(f,"\tshl ax\n");
				}
				store_reg(f,acc,&p->z,t);
				continue;
			}else
				printf("left shift by variable not yet supported.");
		}
		if(c==RSHIFT){
			if(isconst(q2)){
				if(p->q2.val.vmax==0) continue; // optimize out; does nothing
				load_reg(f,acc,&p->q1,t);
				// todo: check for unsigned (idk how rn)
				for(i=0;i<p->q2.val.vmax;i++){
					emit(f,"\tsar ax\n");
				}
				store_reg(f,acc,&p->z,t);
				continue;
			}else
				printf("right shift by variable not yet supported.");
		}
		if((c>=OR&&c<=AND)||c==ADD||c==SUB||c==MULT||c==ADDI2P||c==SUBIFP||c==SUBPFP||c==COMPARE){
			// don't do shift operations here because they are a bit more... difficult.
			//  > Even worse with floats and division (not yet supported by the CPU)
			if(c==ADDI2P)c=ADD;
			if(c==SUBIFP||c==SUBPFP||c==COMPARE)c=SUB;
			char* s="add";
			if(c==SUB)s="sub";
			else if(c==OR)s="or";
			else if(c==XOR)s="xor";
			else if(c==AND)s="and";
			else if(c==MULT)s="mul";
			/*	this could be more easily written at the expense of less optimization.
				without this if block this kind of unoptimized code could be generated:
			
					mov bx, 22
					add ax, bx
			
				Could be replaced with:
				
					add ax, 22
					
				Also, this
					
					mov bx, 1
					add ax, bx
				
				can be replaced with
				
					add ax, 1
				
				then be replaced with
					
					inc ax
				
				to save 5 bytes and up to 8 cycles.
			*/
			if(isconst(q2)){
				if(c==ADD&&p->q2.val.vmax==1){ // increment, not add
					load_reg(f,acc,&p->q1,t);
					emit(f,"\tinc\tax\n");
					store_reg(f,acc,&p->z,t);
				}else if(c==SUB&&p->q2.val.vmax==1){ // decrement, not subtract
					load_reg(f,acc,&p->q1,t);
					emit(f,"\tdec\tax\n");
					store_reg(f,acc,&p->z,t);
				}else if(c==MULT&&p->q2.val.vmax==2){
					load_reg(f,acc,&p->q1,t);
					emit(f,"\tshl\tax\n"); // better to shift left
					store_reg(f,acc,&p->z,t);
				}else if(c==MULT&&p->q2.val.vmax==1);else{ // don't multiply by one.  It's a waste.
					load_reg(f,acc,&p->q1,t);
					emit(f,"\t%s\tax, %d\n",s,p->q2.val);
					store_reg(f,acc,&p->z,t);
				}
			}else if(isconst(q1)){
				if(c==ADD&&p->q1.val.vmax==1){ // increment, not add
					load_reg(f,acc,&p->q2,t);
					emit(f,"\tinc\tax\n");
					store_reg(f,acc,&p->z,t);
				}else if(c==SUB&&p->q1.val.vmax==1){ // decrement, not subtract
					load_reg(f,acc,&p->q2,t);
					emit(f,"\tdec\tax\n");
					store_reg(f,acc,&p->z,t);
				}else if(c==MULT&&p->q1.val.vmax==2){
					load_reg(f,acc,&p->q2,t);
					emit(f,"\tshl\tax\n"); // better to shift left
					store_reg(f,acc,&p->z,t);
				}else if(c==MULT&&p->q2.val.vmax==1);else{
					load_reg(f,acc,&p->q2,t); // load q2 bc q1 is a constant.
					emit(f,"\t%s\tax, %d\n",s,p->q1.val);
					store_reg(f,acc,&p->z,t);
				}
			}else{
				load_reg(f,acc,&p->q1,t);
				load_reg(f,2,&p->q2,t); // load into BX
				emit(f,"\t%s\tax, bx\n",s);
				store_reg(f,acc,&p->z,t);
			}
			continue;
		}
		printf("Error: norec #%d\n",c);
		pric2(stdout,p);
		ierror(0);
	}
	if(notpopped){
		gen_pop(f,notpopped);
		notpopped=0;
	}
	function_bottom(f,v,loff);
	/*if(debug_info){
		emit(f,"%s%d:\n",labprefix,++label);
		dwarf2_function(f,v,label);
		if(f) section=-1;
	}*/		 
}

void gen_ds(FILE *f, zmax size, struct Typ *t){
	emit(f,".zero %ld\n",zm2l(size));
}
void gen_align(FILE *f, zmax align){
	
	emit(f,".align %ld\n",zm2l(align));
}
void gen_var_head(FILE *f, struct Var *v){
	
	emit(f,".section DATA\n");
	if(v->storage_class==STATIC){
		if(ISFUNC(v->vtyp->flags)) return;
		emit(f,"\tl%ld:",zm2l(v->offset));
		newobj=1;
	}
	if(v->storage_class==EXTERN){
		emit(f,"\t.global\t_%s\n",v->identifier);
		if(v->flags&(DEFINED|TENTATIVE)){
			gen_align(f,4);
			emit(f,"_%s:\n",v->identifier);
		}
	}
}
void gen_dc(FILE *f, int t, struct const_list *p){
	
	int type=t&NQ;
	char *str;
	int data=p->val.vmax;
	if(type==CHAR){
		str="\tdb\t";
		data&=255;
	}
	else if(type==SHORT){
		str="\tdw\t";
		data&=65535;
	}
	else str="\tdd\t";
	/*if(!p->tree){
		if(ISFLOAT(t)){
			//	auch wieder nicht sehr schoen und IEEE noetig	 
			unsigned char *ip;char *s;
			ip=(unsigned char *)&p->val.vdouble;
			emit(f,"$%02x%02x%02x%02x",ip[0],ip[1],ip[2],ip[3]);
			if((t&NQ)!=FLOAT){
				emit(f,",$%02x%02x%02x%02x",ip[4],ip[5],ip[6],ip[7]);
			}
		}else{
			emitval(f,&p->val,t&NU);
		}
	}else{
		int m,m2,m3;
		p->tree->o.am=0;
		m=p->tree->o.flags;
		p->tree->o.flags&=~VARADR;
		m2=g_flags[5];
		g_flags[5]&=~USEDFLAG;
		emit_obj(f,&p->tree->o,t&NU);
		p->tree->o.flags=m;
	}*/
	emit(f,"\t%s %d\n",str,data);
}
void init_db(FILE *f){
//If debug-information is requested, this functions is called after init_cg(), but before any code is generated. See also Debug Information.
// do nothing
}
void cleanup_db(FILE *f){
//If debug-information is requested, this functions is called prior to cleanup_cg(). See also Debug Information.
// Tell the PIC to stop handling interrupts and crash the OS.
}


int reg_parm(struct reg_handle *p,struct Typ *t,int mode,struct Typ *fkt)
{
	if(p->gpr) return 0;
	if(ISSCALAR(t->flags)&&!ISFLOAT(t->flags)&&!ISWORD(t->flags)){
		p->gpr=1;
		return 1;
	}
	return 0;
}

int reg_pair(int r,struct rpair *p)
/* Returns 0 if the register is no register pair. If r	*/
/* is a register pair non-zero will be returned and the */
/* structure pointed to p will be filled with the two	 */
/* elements.																						*/
{
	return 0;
}