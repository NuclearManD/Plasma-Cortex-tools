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

names={}
location=0

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
location=0
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
