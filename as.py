# based on old NGX series assembler.
print("Plasma Cortex assembler")
import sys
args=sys.argv
if(len(args)==1):
    args.append(input("input filename:"))
ofn=args[1]
try:
    ofn=ofn[:ofn.index('.')]
except:
    pass
file=open(args[1])
filedat=file.read()
file.close()
fdat_upper="""library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity biosram is
port (
		 clka  : IN STD_LOGIC;
		 ena   : IN STD_LOGIC;
		 wea   : IN STD_LOGIC;
		 addra : IN STD_LOGIC_VECTOR(13 DOWNTO 0);
		 dina  : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 douta : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
     );
end biosram;

architecture Behavioral of biosram is

	--Declaration of type and signal of a 512 element RAM
	--with each element being 8 bit wide.
	type ram_t is array (0 to """
fdat_mid=""") of std_logic_vector(7 downto 0);
	signal ram : ram_t := ("""
fdat_lower=""");
	signal data_o : std_logic_vector(7 downto 0);
	signal adr    : std_logic_vector(5 downto 0);
begin

	--process for read and write operation.
	ram(to_integer(unsigned(addra))) <= dina when wea='1' and ena='1';
	douta <= ram(to_integer(unsigned(addra))) when wea='0' and ena='1';

end Behavioral;"""
split=False
com=False
tmp=""
tokens=[]
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
        tmp=""
        if split:
            tokens.append('"')
    elif (i.isspace() or i==',')and tmp!="'" and not split:
        if(tmp!=""):
            tokens.append(tmp)
        tmp=""
    else:
        tmp+=i
tokens.append(tmp)

regs=['ax','bx','cx','dx','si','di','sp','pc']
math_ops={'add':0x80, 'sub':0x81, 'mul':0x82, 'div':0x83, 'xor':0x85, 'and':0x86, 'or':0x87, 'shl':0xC0,'shr':0xC1,'sar':0xC2,'abs':0xC3,'not':0xC4}

names={}
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
while i<len(tokens):
    if tokens[i].endswith(':'):
        names[tokens[i].replace(':','')]=location
        tokens.remove(tokens[i])
        i-=1
    else:
        tokens[i]=tokens[i].lower()
        if tokens[i]=="org":
            i=i+1
            location=to_int(tokens[i])
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
                if(not tokens[i+2] in regs):
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
location=0
coe=""
out=[]
def wr32(x):
    out.append(x>>24)
    out.append((x>>16)&255)
    out.append((x>>8)&255)
    out.append(x&255)
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
def evaluate(x):
    if is_int(x):
        wr32(to_int(x))
    elif("-po" in args):
        out.append(x)
    elif(x in names.keys()):
        wr32(names[x])
    elif x=='$':
        wr32(location)
    else:
        print("Error: '"+tokens[i]+"' is not a valid number, identifier, or symbol.")
i=0
while i<len(tokens):
    tokens[i]=tokens[i].lower()
    if tokens[i]=="org":
        i=i+1
        location=to_int(tokens[i])
    elif tokens[i]=="db":
        i=i+1
        out.append(to_int(tokens[i]))
        location+=1
    elif tokens[i]=='"':
        i+=1
        location+=len(tokens[i])
        for x in tokens[i]:
            out.append(ord(x))
    elif tokens[i]=="jmp":
        i=i+1
        out.append(0xFF)
        evaluate(tokens[i])
        location+=5
    elif tokens[i]=="jz":
        i=i+1
        out.append(0xE8)
        evaluate(tokens[i])
        location+=5
    elif tokens[i]=="jnz":
        i=i+1
        out.append(0xE9)
        evaluate(tokens[i])
        location+=5
    elif tokens[i]=="call":
        i+=1
        out.append(0x7C)
        evaluate(tokens[i])
        location+=5
    elif tokens[i] in math_ops.keys():
        i+=1
        try:
            out.append(math_ops[tokens[i-1]]|(regs.index(tokens[i])<<4))
        except:
            print("Error: '"+tokens[i]+"' is not a valid 32-bit register.")
        location+=1
    elif tokens[i]=="push":
        i+=1
        try:
            out.append(0xA8|(regs.index(tokens[i])))
        except:
            print("Error: '"+tokens[i]+"' is not a valid 32-bit register.")
        location+=1
    elif tokens[i]=="pop":
        i+=1
        try:
            out.append(0x28|(regs.index(tokens[i])))
        except:
            print("Error: '"+tokens[i]+"' is not a valid 32-bit register.")
        location+=1
    elif tokens[i]=="lodsb":
        out.append(0x7E)
        location+=1
    elif tokens[i]=="stosb":
        out.append(0x7F)
        location+=1
    elif tokens[i]=="ret":
        out.append(0x2F)
        location+=1
    elif tokens[i]=="nop":
        out.append(0x00)
        location+=1
    elif tokens[i]=="djnz":
        out.append(0x6C)
        i+=1
        evaluate(tokens[i])
        location+=5
    elif tokens[i]=="mov":
        if('[' in tokens[i+1]):
            i+=2
            try:
                out.append(0x88|regs.index(tokens[i-1][1:3])|(regs.index(tokens[i])<<4))
            except:
                print("Error: '"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
            location+=1
        elif('[' in tokens[i+2]):
            i+=2
            try:
                out.append(0x80|regs.index(tokens[i][1:3])|(regs.index(tokens[i-1])<<4))
            except:
                print("Error: '"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
            location+=1
        elif(can_eval(tokens[i+2])):
            i+=1
            try:
                out.append(0xF8|regs.index(tokens[i]))
            except:
                print("Error: '"+tokens[i]+"' is not a valid 32-bit register.")
            i+=1
            evaluate(tokens[i])
            location+=5
        else:
            i+=2
            try:
                out.append(regs.index(tokens[i-1])|(regs.index(tokens[i+1])<<4))
            except:
                print("Error: '"+tokens[i-1]+'/'+tokens[i]+"' is not a valid 32-bit register.")
            location+=1
    elif tokens[i]=="out":
        if is_int(tokens[i+2]):
            i+=2
            out.append(39)
            out.append(2)
            out.append(to_int(tokens[i])&255)
            evaluate(tokens[i-1]);
        else:
            out.append(40)
            out.append(2)
            i+=1
            try:
                out.append(regs.index(tokens[i+1]))
            except:
                print("Error: '"+tokens[i+1]+"' is not a valid 32-bit register or number constant.")
            i+=1
            evaluate(tokens[i-1])
        location+=5
    elif tokens[i]=="in":
        out.append(38)
        out.append(2)
        i+=1
        try:
            out.append(regs.index(tokens[i]))
        except:
            print("Error: '"+tokens[i]+"' is not a valid 32-bit register or number constant.")
        i+=1
        evaluate(tokens[i])
        location+=5
    else:
        print("error: invalid:"+tokens[i])
    i+=1
if("-po" in args):
    print("Making python object file...")
    obj={"code":out,"lbl":names}
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
