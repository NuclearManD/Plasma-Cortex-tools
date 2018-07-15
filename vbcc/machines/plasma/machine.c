/*

	Assembly Generator for the Nuclaer Tech Plasma Cortex, a VHDL soft processor for FPGAs.
	Outputs NASM syntax, meant to be used with as.exe (on windows), or the respective program on Linux (not yet built).

	Assembly Generator für den Nuclaer Tech Plasma Cortex, einen VHDL Softprozessor für FPGAs.
	Gibt die NASM-Syntax aus, die mit as.exe (unter Windows) verwendet werden soll, oder das entsprechende Programm unter Linux (noch nicht erstellt).
	Bitte vergib Fehlübersetzungen.
*/

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
int regsa[MAXR+1]={0,0,0,0,0,1,1,1,1};
int reg_prio[MAXR+1]={0,1,1,1,1,1,1,1,1};
/*

private functions/data

much of this is from the hc12 backend.
*/

static long loff,stackoffset,notpopped,dontpop,maxpushed,stack;

// this will have to be modified
static char *ccs[]={"z","nz","c","nc","LessThanOrEqualTo","GreaterThan"}; // signed?
static char *uccs[]={"z","nz","c","nc","LessThan|EqualTo","GreaterThan"};// unsigned?

static void function_top(FILE *f,struct Var *v,long offset)
/*  erzeugt Funktionskopf                       */
// I am going to leave this function unconverted until I need to modify it.
{
  int i;
  emit(f,"# offset=%ld\n",offset);
  have_frame=0;stack_valid=1;stack=0;
  if(!special_section(f,v)&&section!=CODE){emit(f,codename);if(f) section=CODE;}
  if(v->storage_class==EXTERN){
    if((v->flags&(INLINEFUNC|INLINEEXT))!=INLINEFUNC)
      emit(f,"\t.global\t%s%s\n",idprefix,v->identifier);
    emit(f,"%s%s:\n",idprefix,v->identifier);
  }else{
    emit(f,"%s%ld:\n",labprefix,zm2l(v->offset));
  }
  if(stack_check){
    stackchecklabel=++label;
    emit(f,"\tldy\t#%s%d\n",labprefix,stackchecklabel);
    /* FIXME: banked */
    emit(f,"\tbsr\t%s__stack_check\n",idprefix);
  }
  if(offset){
    if(offset==1)
      emit(f,"\tpshb\n");
    else if(offset==2)
      emit(f,"\tpshd\n");
    else
      emit(f,"\tleas\t-%ld,%s\n",offset,regnames[sp]);
    have_frame=1;
  }
}
/* pushed on the stack by a callee, no pop needed */
static void callee_push(long l)
{
  if(l-stackoffset>stack)
    stack=l-stackoffset;
}
static void push(long l)
{
  stackoffset-=l;
  if(stackoffset<maxpushed) maxpushed=stackoffset;
  if(-maxpushed>stack) stack=-maxpushed;
}
static void pop(long l)
{
  stackoffset+=l;
}
// I think that l is the number of bytes to pop.
static void gen_pop(FILE *f,long l)
{
  if(l==0) return;
  if((l&3)>0){
    ierror(); // can only pop 4 bytes at a time.  No more, no less.
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
    emit(f,"\talloc %ld\n",l);
  }
  pop(l);
}
static void load_reg(FILE *f,int r,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==r) return;
    emit(f,"\ttfr\t%s,%s\n",regnames[o->reg],regnames[r]);
    return;
  }
  if(r==acc&&(o->flags&(KONST|DREFOBJ))==KONST){
    eval_const(&o->val,t);
    if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))){
      emit(f,"\tclra\n\tclrb\n");
      cc=o;cc_t=t;
      return;
    }
  }
  emit(f,"\tld%s\t",(r==acc&&(t&NQ)==CHAR)?"ab":regnames[r]);
  emit_obj(f,o,t);emit(f,"\n");
  cc=o;cc_t=t;
}
static void store_reg(FILE *f,int r,struct obj *o,int t)
{
  if((o->flags&(REG|DREFOBJ))==REG){
    if(o->reg==r) return;
    emit(f,"\ttfr\t%s,%s\n",regnames[r],regnames[o->reg]);
  }else{
    emit(f,"\tst%s\t",(r==acc&&(t&NQ)==CHAR)?"ab":regnames[r]);
    emit_obj(f,o,t);emit(f,"\n");
    cc=o;cc_t=t;
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
		regsize[i]=l2zm(4L);
	
	return 1;
}
void cleanup_cg(FILE *f){
	
}
int regok(int r, int t, int mode){
	if(r>6)return 0; // special registers
	if(t==DOUBLE||t==LDOUBLE)return 0; // not 64 bit
	if(mode<=0&&r>4)return 0; // can't do arithmetic
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
	int c,t,lastcomp=0,reg,short_add,bit_reverse,need_return=0;
	struct obj *bit_obj;char *bit_reg;
	static int idone;
	struct obj o;
	struct IC *p2;
	ret="ret";
	for(c=1;c<=MAXR;c++) regs[c]=regsa[c];  // regs[] not well defined in docs.
	
	for(c=1;c<=MAXR;c++){ // seems like this code checks for register modification and notifies the rest of the backend.
		if(regscratch[c]&&(regsa[c]||regused[c])){
			BSET(regs_modified,c);
		}
	}
	
	loff=zm2l(offset); // offset as a long
	
	function_top(f,v,loff);
	stackoffset=notpopped=dontpop=maxpushed=0;
	for(;p;pr(f,p),p=p->next){
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
		if(notpopped&&!dontpop){
			int flag=0;
			if(c==LABEL||c==COMPARE||c==TEST||c==BRA){
				gen_pop(f,notpopped);
				notpopped=0;
			}
		}
		if(c==LABEL) {cc=0;emit(f,"%s%d:\n",labprefix,t);continue;}
		if(c>=BEQ&&c<=BGT&&t==exit_label) need_return=1;
		if(c==BRA){
			if(p->typf==exit_label&&!have_frame){
				emit(f,"\t%s\n",ret);
			}else{
				if(t==exit_label) need_return=1;
				emit(f,"\tjmp\t%d\n",t); // idk yet, I think this is jmp
			}
			cc=0;continue;
		}
		if(c>=BEQ&&c<BRA){			
			if(t&UNSIGNED)
				emit(f,"\tj%s\t%s%d\n",uccs[c-BEQ],t);
			else
				emit(f,"\tj%s\t%s%d\n",ccs[c-BEQ],t);
			continue;
		}
		if(c==MOVETOREG){
			load_reg(f,p->z.reg,&p->q1,SHORT);
			continue;
		}
		if(c==MOVEFROMREG){
			store_reg(f,p->q1.reg,&p->z,SHORT);
			continue;
		}
		
		/*if(ISFLOAT(t)) {pric2(stdout,p);ierror(0);}*/

		if((t&NQ)==BIT){
			ierror(0);
		}

		if(c==TEST){
			lastcomp=t;
			p->code=c=COMPARE;
			gval.vmax=l2zm(0L);
			p->q2.flags=KONST;
			eval_const(&gval,MAXINT);
			insert_const(&p->q2.val,t);
		} 
		if(c==COMPARE&&isconst(q1)){
			struct IC *p2;
			o=p->q1;p->q1=p->q2;p->q2=o;
			p2=p->next;
			while(p2&&p2->code==FREEREG) p2=p2->next;
			if(!p2||p2->code<BEQ||p2->code>BGT) ierror(0);
			if(p2->code==BLT) p2->code=BGT;
			else if(p2->code==BGT) p2->code=BLT;
			else if(p2->code==BLE) p2->code=BGE;
			else if(p2->code==BGE) p2->code=BLE;
		}
		if(c==COMPARE&&isconst(q2)){
			eval_const(&p->q2.val,t);
			if(ISNULL()){
	if(cc&&(cc_t&NU)==(t&NU)&&compare_objects(cc,&p->q1)){
		lastcomp=t;continue;
	}
			}
		}

		if(c==SUBPFP){
			ierror(0);
		}

		if(c==ADDI2P||c==SUBIFP){
			if((p->typf2&NQ)!=HPOINTER){
	p->typf=t=(UNSIGNED|SHORT);
	short_add=2;
			}else if(ISHWORD(t)){
	p->typf=t=(UNSIGNED|LONG);
	short_add=1;
			}
		}

		switch_IC(p);

		if(c==CONVERT){
			int to=p->typf2&NU;
			if(to==INT) to=SHORT;
			if(to==(UNSIGNED|INT)||to==NPOINTER) to=(UNSIGNED|SHORT);
			if(to==FPOINTER||to==HPOINTER) to=(UNSIGNED|LONG);
			if((t&NU)==INT) t=SHORT;
			if((t&NU)==(UNSIGNED|INT)||(t&NU)==NPOINTER) t=(UNSIGNED|SHORT);
			if((t&NQ)==FPOINTER||(t&NQ)==HPOINTER) t=(UNSIGNED|LONG);
			/*if((t&NQ)>=LONG||(to&NQ)>=LONG) ierror(0);*/
			if((to&NQ)<=LONG&&(t&NQ)<=LONG){
	if((to&NQ)<(t&NQ)){
		if(!ISRACC(q1)&&!ISRACC(z))
			get_acc(f,p);
		load_reg(f,acc,&p->q1,to);
		if(to&UNSIGNED)
			emit(f,"\tclrb\n");
		else
			emit(f,"\tsex\tb,d\n");
		store_reg(f,acc,&p->z,t);
		cc=&p->z;cc_t=t;
		continue;
	}else if((to&NQ)>(t&NQ)){
		if(!ISRACC(q1)&&!ISRACC(z))
			get_acc(f,p);
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
			cc=0;
			if(compare_objects(&p->q1,&p->z)&&!isreg(q1)&&(p->q1.flags&(REG|DREFOBJ))!=DREFOBJ&&(!p->q1.am||p->q1.am->flags!=ACC_IND)){
	emit(f,"\tcom\t");
	emit_obj(f,&p->z,t);
	emit(f,"\n");
	if(ISHWORD(t)){
		mobj=p->z;
		inc_addr(&mobj,1);
		emit(f,"\tcom\t");
		emit_obj(f,&mobj,t);
		emit(f,"\n");
	}
	continue;
			}
			if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	get_acc(f,p);
			load_reg(f,acc,&p->q1,t);
			emit(f,"\tcoma\n\tcomb\n");
			store_reg(f,acc,&p->z,t);
			continue;
		}
		if(c==MINUS){
			if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
	get_acc(f,p);
			if(isreg(q1)){
	load_reg(f,acc,&p->q1,t);
	emit(f,"\tcoma\n\tcomb\n\taddd\t#1\n");
			}else{
	emit(f,"\tclra\n\tclrb\n");
	emit(f,"\tsubd\t");
	emit_obj(f,&p->q1,t);
	emit(f,"\n");
			}
			cc=&p->z;cc_t=t;
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
			cc=0;
			if(!calc_regs(p,f!=0)&&v->fi) v->fi->flags&=~ALL_REGS;
			if((p->q1.flags&VAR)&&p->q1.v->fi&&p->q1.v->fi->inline_asm){
	emit_inline_asm(f,p->q1.v->fi->inline_asm);
	jmp=1;
			}else{
	if(stackoffset==0&&!have_frame&&!strcmp(ret,"rts")){
		struct IC *p2;
		jmp=1;
		for(p2=p->next;p2;p2=p2->next){
			if(p2->code!=FREEREG&&p2->code!=ALLOCREG&&p2->code!=LABEL){
				jmp=0;break;
			}
		}
	}
	if(p->q1.flags&DREFOBJ){
		/*FIXME: test this*/
		if(jmp)
			emit(f,"\tjmp\t");
		else
			emit(f,"\tjsr\t");
		emit_obj(f,&p->q1,t);
		emit(f,"\n");
	}else{
		if(jmp){
			emit(f,"\tbra\t");
			if(!need_return) ret=0;
		}else{
			emit(f,"\tbsr\t");
		}
		emit_obj(f,&p->q1,t);
		emit(f,"\n");
	}
			}
			if(stack_valid){
				int i;
				if(p->call_cnt<=0){
					err_ic=p;if(f) error(320);
					stack_valid=0;
				}
				for(i=0;stack_valid&&i<p->call_cnt;i++){
					if(p->call_list[i].v->fi&&(p->call_list[i].v->fi->flags&ALL_STACK)){
			/*FIXME: size of return addr depends on mode */
			if(!jmp) push(2);
			callee_push(zm2l(p->call_list[i].v->fi->stack1));
			if(!jmp) pop(2);
					}else{
						err_ic=p;if(f) error(317,p->call_list[i].v->identifier);
						stack_valid=0;
					}
				}
			}
			if(!zmeqto(l2zm(0L),p->q2.val.vmax)){
	notpopped+=zm2l(p->q2.val.vmax);
	dontpop-=zm2l(p->q2.val.vmax);
	if(!(g_flags[2]&USEDFLAG)&&stackoffset==-notpopped){
		/*	Entfernen der Parameter verzoegern	*/
	}else{
		gen_pop(f,zm2l(p->q2.val.vmax));
		notpopped-=zm2l(p->q2.val.vmax);
	}
			}
			continue;
		}
		if(c==ASSIGN||c==PUSH){
			if(c==PUSH) dontpop+=zm2l(p->q2.val.vmax);
			if(!zmleq(p->q2.val.vmax,l2zm(2L))){
	unsigned long size;int qr=0,zr=0,px=0,py=0,pd=0,lq=0,lz=0;
	size=zm2l(p->q2.val.vmax);
	if(c==ASSIGN){
		if(!p->z.am&&(p->z.flags&(REG|DREFOBJ))&&ISIDX(p->z.reg))
			zr=p->z.reg;lz=1;
	}
	if(!p->q1.am&&(p->q1.flags&(REG|DREFOBJ))&&ISIDX(p->q1.reg)&&p->q1.reg!=zr){
		qr=p->q1.reg;lq=1;
	}
	if(!qr){
		if(zr==ix) qr=iy;
		else if(zr==iy) qr=ix;
		else{qr=ix;zr=iy;}
	}else if(!zr){
		if(qr==ix) zr=iy; else zr=ix;
	}
	if(c==PUSH){
		emit(f,"\tleas\t%ld,%s\n",-size,regnames[sp]);
		push(size);
	}
	if(regs[ix]&&!scratchreg(ix,p)){emit(f,"\tpshx\n");push(2);px=1;}
	if(regs[iy]&&!scratchreg(iy,p)){emit(f,"\tpshy\n");push(2);py=1;}
	if(!lq) load_addr(f,qr,&p->q1);
	if(c==PUSH)
		emit(f,"\ttfr\t%s,%s\n",regnames[sp],regnames[zr]);
	else
		if(!lz) load_addr(f,zr,&p->z);
	if(size<=6||(size<=16&&!optsize)){
		while(size>2){
			emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			size-=2;
		}
		emit(f,"\tmov%c\t0,%s,0,%s\n",size==2?'w':'b',regnames[qr],regnames[zr]);
	}else{
		int l=++label;
		if(regs[acc]&&!scratchreg(acc,p)){emit(f,"\tpshd\n");push(2);pd=1;}
		emit(f,"\tldd\t#%lu\n",optsize?size:size/8);
		cc=0;
		emit(f,"%s%d:\n",labprefix,l);
		if(optsize){
			emit(f,"\tmovb\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			emit(f,"\tdbne\td,%s%d\n",labprefix,l);
		}else{
			emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
			emit(f,"\tdbne\td,%s%d\n",labprefix,l);
			size=size&7;
			while(size>=2){
				emit(f,"\tmovw\t2,%s+,2,%s+\n",regnames[qr],regnames[zr]);
				size-=2;
			}
			if(size)
				emit(f,"\tmovb\t0,%s,0,%s\n",regnames[qr],regnames[zr]);
		}
	}
	if(pd){emit(f,"\tpuld\n");pop(2);}
	if(py){emit(f,"\tpuly\n");pop(2);}
	if(px){emit(f,"\tpulx\n");pop(2);}
	continue;
			}
			if(!ISSCALAR(t)) t=zmeqto(p->q2.val.vmax,l2zm(0L))?CHAR:INT;
			if((t&NQ)==CHAR&&!zmeqto(p->q2.val.vmax,l2zm(1L))) t=INT;	
			if(mov_op(&p->q1)&&(c==PUSH||mov_op(&p->z))){
	emit(f,"\tmov%c\t",ISHWORD(t)?'w':'b');
	emit_obj(f,&p->q1,t);
	if(c==ASSIGN){
		emit(f,",");
		emit_obj(f,&p->z,t);
		emit(f,"\n");
	}else{
		emit(f,",%d,-%s\n",ISHWORD(t)?2:1,regnames[sp]);
		push(ISHWORD(t)?2:1);
	}
	continue;
			}
			if((p->q1.flags&KONST)&&!isreg(z)){
	eval_const(&p->q1.val,t);
	if(zmeqto(vmax,l2zm(0L))&&zumeqto(vumax,ul2zum(0UL))&&((p->z.flags&(REG|DREFOBJ))!=DREFOBJ||(t&NQ)==CHAR)&&(!p->z.am||p->z.am->flags!=ACC_IND||(t&NQ)==CHAR)){
		emit(f,"\tclr\t");
		if(c==ASSIGN){
			emit_obj(f,&p->z,t);emit(f,"\n");
		}else
			emit(f,"1,-sp\n");
		if(!ISHWORD(t)) continue;
		emit(f,"\tclr\t");
		if(c==ASSIGN){
			mobj=p->z;
			inc_addr(&mobj,1);
			emit_obj(f,&mobj,t);emit(f,"\n");
		}else
			emit(f,"1,-sp\n");
		continue;
	}

			}
			if(c==PUSH){
	if(isreg(q1)){
		reg=p->q1.reg;
	}else{
		reg=get_reg(f,p,t);
		load_reg(f,reg,&p->q1,t);
	}
	if((t&NQ)==CHAR)
		emit(f,"\tpshb\n");
	else if(reg==ix)
		emit(f,"\tpshx\n");
	else if(reg==iy)
		emit(f,"\tpshy\n");
	else
		emit(f,"\tpshd\n");
	push(zm2l(p->q2.val.vmax));
	continue;
			}
			if(c==ASSIGN){
	if(isreg(q1)&&isreg(z)){
		if(p->q1.reg!=p->z.reg)
			emit(f,"\ttfr\t%s,%s\n",regnames[p->q1.reg],regnames[p->z.reg]);
	}else if(isreg(q1)){
		store_reg(f,p->q1.reg,&p->z,t);
	}else if(isreg(z)){
		load_reg(f,p->z.reg,&p->q1,t);
	}else{
		reg=get_reg(f,p,t);
		load_reg(f,reg,&p->q1,t);
		store_reg(f,reg,&p->z,t);
	}
	continue;
			}
			ierror(0);
		}
		if(c==ADDRESS){
			int px=0;
			if(isreg(z)){
	reg=p->z.reg;
			}else if(!regs[ix]){
	reg=ix;
			}else if(!regs[iy]){
	reg=iy;
			}else{
	/*FIXME: test if x used in q1 */
	px=1;
	emit(f,"\tpshx\n");
	reg=ix;
	push(2);
			}
			load_addr(f,reg,&p->q1);
			if(!(p->z.flags&REG)||p->z.reg!=reg)
	store_reg(f,reg,&p->z,p->typf2);
			if(px){
	emit(f,"\tpulx\n");
	pop(2);
			}
			continue;
		}

		if((c==MULT||c==DIV||(c==MOD&&(p->typf&UNSIGNED)))&&isconst(q2)){
			long ln;
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
			}
		}
		if(c==MOD||c==DIV){
			ierror(0);
			continue;
		}
		if((c>=LSHIFT&&c<=MOD)||c==ADDI2P||c==SUBIFP||c==SUBPFP||(c>=OR&&c<=AND)||c==COMPARE){
			char *s;
			/*FIXME: nicht immer besser*/
			if(ISLWORD(t)&&c==LSHIFT&&isconst(q2)){
	eval_const(&p->q2.val,t);
	if(zm2l(vmax)==1){
		p->code=c=ADD;
		p->q2=p->q1;
	}
			}
			if((c==ADD||c==ADDI2P||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&!isreg(q1)){
	o=p->q1;p->q1=p->q2;p->q2=o;
			}
			if((c==ADD||c==MULT||(c>=OR&&c<=AND))&&isreg(q2)&&p->q2.reg==acc){
	o=p->q1;p->q1=p->q2;p->q2=o;
			}
			if(c==MULT||c==MOD){
	if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
		get_acc(f,p);
	reg=acc;
	/*FIXME: y bzw. x-Register*/
			}else if(c==LSHIFT||c==RSHIFT||c==AND||c==OR||c==XOR){
	if((!isreg(z)||p->z.reg!=acc)&&!scratchreg(acc,p))
		get_acc(f,p);
	reg=acc;
			}else if(c==DIV){
	reg=ix;
	ierror(0);
			}else if(isreg(z)){
	reg=p->z.reg;
			}else if(isreg(q1)&&(c==COMPARE||scratchreg(p->q1.reg,p))){
	reg=p->q1.reg;
			}else{
	if(c==ADD||c==SUB||c==ADDI2P||c==SUBIFP||c==COMPARE){
		/*FIXME??*/
		reg=get_reg(f,p,t);
	}else{
		get_acc(f,p);
		reg=acc;
	}
			}
			if(c==ADD||c==ADDI2P||c==SUB||c==SUBIFP){
	int opdone=0;
	if(isreg(q1)){
		if(ISIDX(reg)&&ISIDX(p->q1.reg)&&isconst(q2)){
			eval_const(&p->q2.val,q2typ(p));
			if(p->q1.reg==reg&&zmeqto(vmax,l2zm(1L))&&zumeqto(vumax,ul2zum(1UL))){
				emit(f,"\t%s%s\n",c==SUB?"de":"in",regnames[reg]);
				/*FIXME: condition-codes for bne/beq could be used */
			}else{
				emit(f,"\tlea%s\t%s%ld,%s\n",regnames[reg],(c==SUB?"-":""),zm2l(vmax),regnames[p->q1.reg]);
			}
			opdone=1;
		}else	if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISIDX(p->q1.reg)&&ISRACC(q2)){
			emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[p->q1.reg]);
			opdone=1;
		}else if((c==ADD||c==ADDI2P)&&ISIDX(reg)&&ISACC(p->q1.reg)&&ISRIDX(q2)){
			emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[p->q2.reg]);
			opdone=1;
		}else if((c==ADD||c==ADDI2P)&&p->q1.reg==acc&&ISIDX(reg)){
			load_reg(f,reg,&p->q2,t);
			emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
			opdone=1;
		}else if((c==ADD||c==ADDI2P)&&ISACC(p->q1.reg)&&ISRACC(z)&&isreg(q2)&&ISIDX(p->q2.reg)){
			if(!scratchreg(p->q2.reg,p)) emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
			emit(f,"\tlea%s\t%s,%s\n",regnames[p->q2.reg],regnames[acc],regnames[p->q2.reg]);
			emit(f,"\texg\t%s,%s\n",regnames[acc],regnames[p->q2.reg]);
			opdone=1;
		}else	if(p->q1.reg!=reg)
			emit(f,"\ttfr\t%s,%s\n",regnames[p->q1.reg],regnames[reg]);
	}else{
		load_reg(f,reg,&p->q1,q1typ(p));
	}
	if(!opdone){
		if(reg==acc){
			if(ISRACC(q2)){
				if(c==ADD||c==ADDI2P){
		emit(f,"\tasld\n");
				}else{
		emit(f,"\tclrb\n");
		if((ztyp(p)&NQ)==CHAR) emit(f,"\tclra\n");
				}
			}else{
				if(ISRIDX(q2)){
		emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		push(2);pop(2);
		emit(f,"\t%sd\t2,%s+\n",(c==ADD||c==ADDI2P)?"add":"sub",regnames[sp]);
				}else{
		emit(f,"\t%s%s\t",(c==ADD||c==ADDI2P)?"add":"sub",(t&NQ)==CHAR?"b":"d");
		emit_obj(f,&p->q2,t);emit(f,"\n");
				}
			}
			cc=&p->z;cc_t=t;
		}else{
			if(isconst(q2)){
				long l;
				eval_const(&p->q2.val,t);
				l=zm2l(vmax);
				if(c==SUB) l=-l;
				/*FIXME: condition-codes for bne/beq could be used */
				if(l==1&&reg==ix){
		emit(f,"\tinx\n");
				}else if(l==1&&reg==iy){
		emit(f,"\tiny\n");
				}else if(l==-1&&reg==ix){
		emit(f,"\tdex\n");
				}else if(l==-1&&reg==iy){
		emit(f,"\tdey\n");
				}else{
		emit(f,"\tlea%s\t%ld,%s\n",regnames[reg],l,regnames[reg]);
				}
			}else{
				if(c!=ADD&&c!=ADDI2P){
		get_acc(f,p);
		emit(f,"\tclrb\n");
		if((t&NQ)!=CHAR) emit(f,"\tclra\n");
		emit(f,"\tsub%s\t",(q2typ(p)&NQ)==CHAR?"b":"d");
		emit_obj(f,&p->q2,t);
		emit(f,"\n");
				}else if(!ISRACC(q2)){
		get_acc(f,p);
		load_reg(f,acc,&p->q2,t);
				}
				emit(f,"\tlea%s\t%s,%s\n",regnames[reg],(t&NQ)==CHAR?"b":"d",regnames[reg]);
			}
		}
	}
	store_reg(f,reg,&p->z,ztyp(p));
	continue;
			}
			load_reg(f,reg,&p->q1,t);
			if(c==MULT){
	int py=0;
	if(reg!=acc) ierror(reg);
	if(!ISRY(q2)&&regs[iy]){
		emit(f,"\tpshy\n");
		push(2);
		py=1;
	}
	load_reg(f,iy,&p->q2,t);
	emit(f,"\temul\n");
	if(py){
		emit(f,"\tpuly\n");
		pop(2);
	}
	store_reg(f,acc,&p->z,t);
	continue;
			}
			if(c==LSHIFT||c==RSHIFT){
	if(isconst(q2)){
		int l,oldl;
		eval_const(&p->q2.val,t);
		oldl=l=zm2l(vmax);
		if(l>=16){ emit(f,"\tclra\n\tclrb\n");l=0;}
		if(l>=8){
			if(c==LSHIFT)
				emit(f,"\ttba\n\tclrb\n");
			else{
				if(t&UNSIGNED)
		emit(f,"\ttab\n\tclra\n");
				else
		emit(f,"\ttab\n\tsex\tb,d\n");
			}
			l-=8;
		}
		while(l--){
			if(c==RSHIFT){
				if(t&UNSIGNED)
		emit(f,"\tlsrd\n");
				else{
		if(oldl>8)
			emit(f,"\tasrb\n");
		else
			emit(f,"\tasra\n\trorb\n");
				}
			}else{
				emit(f,"\tasld\n");
			}
		}
	}else{
		int px;char *s;
		if(regs[ix]&&!scratchreg(ix,p)&&(!isreg(z)||p->z.reg!=ix)){
			emit(f,"\tpsh%s\n",regnames[ix]);
			push(2);px=1;
		}else
			px=0;
		if(c==LSHIFT) s="lsl";
		else if(t&UNSIGNED) s="lsr";
		else s="asr";
		load_reg(f,ix,&p->q2,t);
		emit(f,"\t.global\t%s__%s\n",idprefix,s);
		emit(f,"\tbsr\t%s__%s\n",idprefix,s);
		if(px){
			emit(f,"\tpul%s\n",regnames[ix]);
			pop(2);
		}
	}
	cc=0;
	store_reg(f,acc,&p->z,t);
	continue;
			}
			if(c>=OR&&c<=AND){
	s=logicals[c-OR];
	if(p->q2.am&&p->q2.am->flags==ACC_IND){
		mobj=p->q1;p->q1=p->q2;p->q2=mobj;
	}
	if(p->q2.flags&KONST){
		unsigned long l,h;
		eval_const(&p->q2.val,t);
		l=zum2ul(vumax);
		h=(l>>8)&255;
		if(c==AND&&h==0)
			emit(f,"\tclra\n");
		else if(c==XOR&&h==255)
			emit(f,"\tcoma\n");
		else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
			emit(f,"\t%sa\t#%lu\n",s,h);
		if((t&NQ)!=CHAR){
			h=l&255;
			if(c==AND&&h==0)
				emit(f,"\tclrb\n");
			else if(c==XOR&&h==255)
				emit(f,"\tcomb\n");
			else if((c==AND&&h!=255)||(c==OR&&h!=0)||(c==XOR&&h!=0))
				emit(f,"\t%sb\t#%lu\n",s,h);
		}
	}else{
		if(isreg(q2)){
			if(p->q2.reg==acc){
				if(c==XOR){
		emit(f,"\tclra\n");
		if((t&NQ)!=CHAR) emit(f,"\tclrb\n");
				}
			}else{
				if((t&NQ)==CHAR){
		emit(f,"\tpsha\n");
		push(1);
		emit(f,"\t%sa\t1,%s+\n",s,regnames[sp]);
		pop(1);
				}else{
		emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		push(2);
		emit(f,"\t%sa\t1,%s+\n",s,regnames[sp]);
		emit(f,"\t%sb\t1,%s+\n",s,regnames[sp]);
		pop(2);
				}
			}
		}else if((p->q2.flags&(REG|DREFOBJ))==DREFOBJ){
			int xr=0;
			if(!regs[ix]) xr=ix;
			else if(!regs[iy]) xr=iy;
			else{
				xr=ix;
				emit(f,"\tpsh%s\n",regnames[xr]);
				push(2);

			}
			BSET(regs_modified,xr);
			load_addr(f,xr,&p->q2);
			emit(f,"\t%sa\t0,%s\n",s,regnames[xr]);
			emit(f,"\t%sb\t1,%s\n",s,regnames[xr]);
			pop(2);
		}else{
			emit(f,"\t%sa\t",s);emit_obj(f,&p->q2,t);
			emit(f,"\n");
			if((t&NQ)!=CHAR){
				mobj=p->q2;
				inc_addr(&mobj,1);
				emit(f,"\t%sb\t",s);emit_obj(f,&mobj,t);
				emit(f,"\n");
			}
		}
	}
	cc=0;
	store_reg(f,reg,&p->z,t);
	continue;
			}else if(c==COMPARE){
	lastcomp=t;
	if(isreg(q2)){
		emit(f,"\tpsh%s\n",regnames[p->q2.reg]);
		push(2);
	}
	if(reg==acc){
		if((t&NQ)==CHAR)
			emit(f,"\tcmpb\t");
		else
			emit(f,"\tcpd\t");
	}else if(reg==ix){
		emit(f,"\tcpx\t");
	}else if(reg==iy){
		emit(f,"\tcpy\t");
	}else
		ierror(0);
	if(isreg(q2)){
		emit(f,"2,%s+\n",regnames[sp]);
		pop(2);
	}else{
		emit_obj(f,&p->q2,t);emit(f,"\n");
	}
	continue;
			}
			ierror(0);
		}
		pric2(stdout,p);
		ierror(0);
	}
	if(notpopped){
		gen_pop(f,notpopped);
		notpopped=0;
	}
	function_bottom(f,v,loff);
	if(debug_info){
		emit(f,"%s%d:\n",labprefix,++label);
		dwarf2_function(f,v,label);
		if(f) section=-1;
	}		 
}

void gen_ds(FILE *f, zmax size, struct Typ *t){
	title(f);
	for(int i=0;i<zm2l(size);i++)
		emit(f,"db 0"); // crude until assembler is updated.
}
void gen_align(FILE *f, zmax align){
	title(f);
	emit(f,"align %ld",zm2l(align));
}
void gen_var_head(FILE *f, struct Var *v){
	title(f);
	emit(f,"section DATA");
	if(v->storage_class==STATIC){
		if(ISFUNC(v->vtyp->flags)) return;
		emit(f,"\t.l%ld:",zm2l(v->offset));
		newobj=1;
	}
	if(v->storage_class==EXTERN){
		emit(f,"\tglobal\t_%s\n",v->identifier);
		if(v->flags&(DEFINED|TENTATIVE)){
			emit(f,"\talign\t4\n_%s:\n",v->identifier);
		}
	}
}
void gen_dc(FILE *f, int t, struct const_list *p){
	title(f);
	int type=t&NQ;
	char *str;
	if(type==CHAR) str="\tdb\t";
	else if(type==SHORT) str="\tdw\t";
	else str="\tdd\t";
	emit(f,"%s",str);
	if(!p->tree){
		if(ISFLOAT(t)){
			/*	auch wieder nicht sehr schoen und IEEE noetig	 */
			unsigned char *ip;char *s;
			ip=(unsigned char *)&p->val.vdouble;
			if(GAS) s="0x"; else s="$";
			emit(f,"%s%02x%02x%02x%02x",s,ip[0],ip[1],ip[2],ip[3]);
			if((t&NQ)!=FLOAT){
	emit(f,",%s%02x%02x%02x%02x",s,ip[4],ip[5],ip[6],ip[7]);
			}
		}else if((t&NQ)==LLONG){
			zumax tmp;
			eval_const(&p->val,t);
			tmp=vumax;
			vumax=zumand(zumrshift(vumax,ul2zum(32UL)),ul2zum(0xffffffff)); 
			gval.vulong=zum2zul(vumax);
			emitval(f,&gval,UNSIGNED|LONG);
			emit(f,",");
			vumax=zumand(tmp,ul2zum(0xffffffff)); 
			gval.vulong=zum2zul(vumax);
			emitval(f,&gval,UNSIGNED|LONG);
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
		m3=use_sd;
		use_sd=0;
		emit_obj(f,&p->tree->o,t&NU);
		p->tree->o.flags=m;
		g_flags[5]=m2;
		use_sd=m3;
	}
	emit(f,"\n");
}
void init_db(FILE *f){
//If debug-information is requested, this functions is called after init_cg(), but before any code is generated. See also Debug Information.
// do nothing
}
void cleanup_db(FILE *f){
//If debug-information is requested, this functions is called prior to cleanup_cg(). See also Debug Information.
// Tell the PIC to stop handling interrupts and crash the OS.
}