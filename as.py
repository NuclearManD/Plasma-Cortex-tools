# based on old NGX series assembler.
print("Plasma Cortex assembler, Copyright 2018 Dylan Brophy")
import sys
args=sys.argv
if(len(args)==1):
    args.append(input("input filename:"))
    sys.argv.append('-po')
ofn=args[1]
try:
    ofn=ofn[:ofn.index('.')]
except:
    pass
print("Assembling "+args[1]+"...")
file=open(args[1])
filedat=file.read()
file.close()
split=False
com=False
tmp=""
tokens=[]
tklines=[]
linenum=1
for i in filedat:
    if i==';':
        com=True
    elif(com):
        if i=='\n' or i=='\r':
            com=False
    elif i=='"':
        split=not split
        if(tmp!=""):
            tokens.append(tmp)
            tklines.append(linenum)
        tmp=""
        if split:
            tokens.append('"')
            tklines.append(linenum)
    elif (i.isspace() or i==',')and tmp!="'" and not split:
        if(tmp!=""):
            tokens.append(tmp)
            tklines.append(linenum)
        tmp=""
    else:
        tmp+=i
    if i=='\n' or i=='\r':
        linenum+=1
if len(tmp)>0:
    tokens.append(tmp)
    tklines.append(linenum)

regs=['ax','bx','cx','dx','si','di','sp','pc']
math_ops={'add':0x80, 'sub':0x81, 'mul':0x82, 'div':0x83, 'xor':0x85, 'and':0x86, 'or':0x87, 'shl':0xC0,'shr':0xC1,'sar':0xC2,'abs':0xC3,'not':0xC4}

names={}
glbls=[]
sizes={}

location=0
i=0
def to_int(s):
    #exec("y="+s)
    try:
        return eval(s)
    except:
        if(s[0]=="'" and s[2]=="'"):
            return ord(s[1])
def is_int(s):
    try: 
        if(s[0]=="'" and s[2]=="'"):
            return True
        return type(eval(s))==int
    except:
        return False
def can_eval(x):
    if("-po" in args):
        return True
    if(x in names.keys()):
        return True
    elif is_int(x):
        return True
    elif x=='$':
        return True
    else:
        return False
while i<len(tokens):
    if tokens[i].endswith(':'):
        names[tokens[i].replace(':','')]=location
    else:
        tokens[i]=tokens[i].lower()
        if tokens[i]==".org":
            i=i+1
            location=to_int(tokens[i])
        elif tokens[i]==".section":
            i+=1
            #ignore for now
        elif tokens[i]==".global":
            i+=1
            glbls.append(tokens[i])
        elif tokens[i]==".equ":
            i+=1
            qzx=tokens[i]
            i+=1
            m=tokens[i]
            ln=tklines[i]
            while len(tklines)>i+1 and tklines[i+1]==ln:
                i+=1
                m+=tokens[i]
            m=m.replace('$',str(location))
            for j in names:
                m=m.replace(j,str(names[j]))
            names[qzx]=eval(m)
        elif tokens[i]==".size":
            i+=1
            qzx=tokens[i]
            i+=1
            m=tokens[i]
            ln=tklines[i]
            while len(tklines)>i+1 and tklines[i+1]==ln:
                i+=1
                m+=tokens[i]
            m=m.replace('$',str(location))
            for j in names:
                m=m.replace(j,str(names[j]))
            #print(eval(m))
        elif tokens[i]=='"':
            i+=1
            location+=len(tokens[i])
        elif tokens[i]=="jmp":
            i=i+1
            location+=5
        elif tokens[i]=="jz":
            i=i+1
            location+=5
        elif tokens[i]=="jnz":
            i=i+1
            location+=5
        elif tokens[i]=="call":
            location+=5
            i+=1
        elif tokens[i]=="lodsb":
            location+=1
        elif tokens[i]=="stosb":
            location+=1
        elif tokens[i]=="ret":
            location+=1
        elif tokens[i]=="nop":
            location+=1
        elif tokens[i] in math_ops.keys():
            if(tokens[i+1]=='sp' and can_eval(tokens[i+2]) and tokens[i]=='add'):
                location+=4 # adding constant to sp
                i+=1
            location+=1
            i+=1
        elif tokens[i] in ["push","pop"]:
            location+=1
            i+=1
        elif tokens[i]=="djnz":
            location+=5
            i+=1
        elif tokens[i]=="mov":
            location+=1
            if(not('[' in tokens[i+1] or '[' in tokens[i+2])):
                pass #location+=4
            elif(not tokens[i+2] in regs):
                location+=4
            i+=2
        elif tokens[i]=="out":
            location+=5
            i+=2
        elif tokens[i]=="in":
            location+=5
            i+=2
        elif tokens[i]=="db":
            location+=1
            i+=1
        else:
            print("preprocessor:  invalid token: "+tokens[i])
    i+=1
lsloc=location
location=0
coe=""
out=[]
lswrt=0
def emit(s):
    global lswrt
    out.append(s)
    lswrt=location
def wr32(x):
    emit(((x>>24)+256)&255)
    emit((x>>16)&255)
    emit((x>>8)&255)
    emit(x&255)
def evaluate(x):
    if is_int(x):
        wr32(to_int(x))
    elif x=='$':
        wr32(location)
    elif("-po" in args):
        emit(x)
    elif(x in names.keys()):
        wr32(names[x])
    else:
        errormsg("'"+tokens[i]+"' is not a valid number, identifier, or symbol.")
i=0
def errormsg(string):
    print("Error on line "+str(tklines[i])+": "+string)
org_done=False
base=0
while i<len(tokens):
    tokens[i]=tokens[i].lower()
    if tokens[i].endswith(':'):
        pass
    elif tokens[i]==".org":
        i=i+1
        if(to_int(tokens[i])<location):
            errormsg(" .org CANNOT go backwards.")
            errormsg("   > reorganize your code so that lower offsets in memory are first.")
        elif len(out)!=0 and org_done:
            for dgyasg in range(location,to_int(tokens[i])):
                if("-po" in args):
                    emit(-1)
                else:
                    emit(0)
        location=to_int(tokens[i])
    elif tokens[i]==".section":
        i+=1
        #ignore for now
    elif tokens[i]==".global":
        i+=1
    elif tokens[i]==".equ":
        i+=1
        qzx=tokens[i]
        i+=1
        m=tokens[i]
        ln=tklines[i]
        while len(tklines)>i+1 and tklines[i+1]==ln:
            i+=1
    elif tokens[i]==".size":
        i+=1
        qzx=tokens[i]
        i+=1
        m=tokens[i]
        ln=tklines[i]
        while len(tklines)>i+1 and tklines[i+1]==ln:
            i+=1
            m+=tokens[i]
        m=m.replace('$',str(location))
        for j in names:
            m=m.replace(j,str(names[j]))
        #print(eval(m))
    else:
        if not org_done:
            base=location
            org_done=True
        if tokens[i]=="db":
            i=i+1
            emit(to_int(tokens[i]))
            location+=1
        elif tokens[i]=='"':
            i+=1
            location+=len(tokens[i])
            for x in tokens[i]:
                emit(ord(x))
        elif tokens[i]=="jmp":
            i=i+1
            emit(0xFF)
            evaluate(tokens[i])
            location+=5
        elif tokens[i]=="jz":
            i=i+1
            emit(0xE8)
            evaluate(tokens[i])
            location+=5
        elif tokens[i]=="jnz":
            i=i+1
            emit(0xE9)
            evaluate(tokens[i])
            location+=5
        elif tokens[i]=="call":
            i+=1
            emit(0x7C)
            evaluate(tokens[i])
            location+=5
        elif tokens[i] in math_ops.keys():
            if(tokens[i+1]=='sp' and can_eval(tokens[i+2]) and tokens[i]=='add'):
                emit(0x7D)
                location+=5
                i+=2
                evaluate(tokens[i])
            else:
                i+=1
                try:
                    emit(math_ops[tokens[i-1]]|(regs.index(tokens[i])<<4))
                except:
                    errormsg("'"+tokens[i]+"' is not a valid 32-bit register.")
                location+=1
        elif tokens[i]=="push":
            i+=1
            try:
                emit(0xA8|(regs.index(tokens[i])))
            except:
                errormsg("'"+tokens[i]+"' is not a valid 32-bit register.")
            location+=1
        elif tokens[i]=="pop":
            i+=1
            try:
                emit(0x28|(regs.index(tokens[i])))
            except:
                errormsg("'"+tokens[i]+"' is not a valid 32-bit register.")
            location+=1
        elif tokens[i]=="lodsb":
            emit(0x7E)
            location+=1
        elif tokens[i]=="stosb":
            emit(0x7F)
            location+=1
        elif tokens[i]=="ret":
            emit(0x2F)
            location+=1
        elif tokens[i]=="nop":
            emit(0x00)
            location+=1
        elif tokens[i]=="djnz":
            emit(0x6C)
            i+=1
            evaluate(tokens[i])
            location+=5
        elif tokens[i]=="mov":
            if('[' in tokens[i+1]):
                i+=2
                try:
                    emit(0x88|regs.index(tokens[i-1][1:3])|(regs.index(tokens[i])<<4))
                except:
                    errormsg("'"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
                location+=1
            elif('[' in tokens[i+2]):
                i+=2
                try:
                    emit(0x80|regs.index(tokens[i][1:3])|(regs.index(tokens[i-1])<<4))
                except:
                    errormsg("'"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
                location+=1
            elif tokens[i+2] in regs:
                i+=2
                try:
                    emit(regs.index(tokens[i-1])|(regs.index(tokens[i])<<4))
                except:
                    errormsg("'"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
                location+=1
            elif(can_eval(tokens[i+2])):
                i+=1
                try:
                    emit(0xF8|regs.index(tokens[i]))
                except:
                    errormsg("'"+tokens[i]+"' is not a valid 32-bit register.")
                i+=1
                evaluate(tokens[i])
                location+=5
        elif tokens[i]=="out":
            if is_int(tokens[i+2]):
                i+=2
                emit(39)
                emit(2)
                emit(to_int(tokens[i])&255)
                evaluate(tokens[i-1]);
            else:
                emit(40)
                emit(2)
                i+=1
                try:
                    emit(regs.index(tokens[i+1]))
                except:
                    errormsg("'"+tokens[i+1]+"' is not a valid 32-bit register or number constant.")
                i+=1
                evaluate(tokens[i-1])
            location+=5
        elif tokens[i]=="in":
            emit(38)
            emit(2)
            i+=1
            try:
                emit(regs.index(tokens[i]))
            except:
                errormsg("'"+tokens[i]+"' is not a valid 32-bit register or number constant.")
            i+=1
            evaluate(tokens[i])
            location+=5
        else:
            errormsg("invalid:"+tokens[i])
    i+=1
if(lsloc!=location):
    print("WARNING: stages 1 and 2 found differing byte counts "+str(lsloc)+" and "+str(location)+"!!")
if("-po" in args):
    print("Making python object file: "+ofn+".po ...")
    obj={"code":out,"lbl":names,"size":[base,location-base]}
    f=open(ofn+'.po','w')
    f.write(str(obj))
    f.close()
else:
    print("Writing binary file...")
    f=open(ofn+'.bin','wb')
    f.write(bytes(out))
    f.close()
    print(str(len(out))+" bytes in size.")
    if("-V" in args):
        print("generating output...")
        COE=""
        for i in out:
            COE+='X"'+(hex(i)[2:]).zfill(2)+'",'
        fout=fdat_upper+str(len(out)-1)+fdat_mid+COE[:len(COE)-1]+fdat_lower
        f=open(ofn+".vhd",'w')
        f.write(fout)
        f.close()
        COE="memory_initialization_radix=16;\nmemory_initialization_vector=\n"
        for i in out:
            COE+=(hex(i)[2:]).zfill(2)+',\n'
        fout=COE[:len(COE)-2]+';'
        f=open(ofn+".coe",'w')
        f.write(fout)
        f.close()
        print("Wrote "+ofn+".vhd and "+ofn+".coe ...")
    if("-DW" in args):
        print("attempting write on port 3...")
        import serial
        x=serial.Serial(port="COM3",baudrate=115200)
        import time
        time.sleep(3)
        x.write(b'0')
        for i in range(0,len(out)):
            x.write(b'3')
            x.write(bytes([0,0,0,i,out[i]]))
            time.sleep(0.1)
            print(str(int((i+1)/(len(out)/100)))+"% complete")
        x.write(b'0')
        print("verifying...")
        for i in range(0,len(out)):
            while x.in_waiting:
                x.read()
            x.write(b'1')
            x.write(bytes([0,0,0,i]))
            tmp=x.read()
            if(ord(tmp)!=out[i]):
                print("Error while verifying.")
                print("@"+str(i))
                print("="+str(out[i]))
                print(ord(tmp))
            time.sleep(0.1)
            print(str(int((i+1)/(len(out)/100)))+"% complete")
        print("wrote output format SERIAL...")
print("done.")
