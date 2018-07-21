# based on old NGX series assembler, then Plasma Cortex assembler, now it is the linker.
import argparse, sys
args = argparse.ArgumentParser(description='Plasma Cortex Linker copyright Dylan Brophy 2018.', prog='ld')
args.add_argument('-po', dest='pyObj', action='store_const', const=True, default=False, help='Generate a Python Object file instead of a binary file')
args.add_argument('-coe', dest='coe', action='store_const', const=True, default=False, help='Generate a COE file')
args.add_argument(metavar='filename', type=str, nargs='+', help='file(s) to link', dest='fns')
args.add_argument('-o', metavar='outfile', nargs=1, help='output file', default='a.bin')
args=args.parse_args()
pyObj=args.pyObj
if(pyObj and args.coe):
    print("Error: can only generate one output file per command. Sorry.")
    sys.exit(-15)
ofn=args.o[0]
if(ofn=='a'):
    if(args.coe):
        ofn="a.coe"
    elif(pyObj):
        ofn="a.po"
    else:
        ofn="a.bin"

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

out_names={}
out_code=[]

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
def wr32(x):
    out_code.append(x>>24)
    out_code.append((x>>16)&255)
    out_code.append((x>>8)&255)
    out_code.append(x&255)
print("Reading files...")
in_names=[]
in_code=[]
in_size=[]
num_files=0
for i in args.fns:
    try:
        f=open(i)
        in_raw=eval(f.read())
        in_names.append(in_raw["lbl"])
        in_code.append(in_raw["code"])
        in_size.append(in_raw["size"])
        f.close()
    except:
        print("Fatal: file '"+i+"' not found. Aborting!")
        sys.exit(-1)
    num_files+=1
print("Merging sizes and offsets...")
fatal=False
for i in range(num_files):
    for j in in_names[i].keys():
        if(j in out_names.keys() and out_names[j]!=in_names[i][j]):
            print("Fatal: conflicting definitions for label '"+j+"' : "+str(out_names[j]) +" and "+str(in_names[i][j])+".")
            fatal=True
    out_names.update(in_names[i])
minspot=2**32
maxspot=0
for i in range(num_files):
    for j in range(i):
        if(min(sum(in_size[i])-in_size[j][0],sum(in_size[j])-in_size[i][0])>0):
            print("Fatal: code collision between files '"+args.fns[i]+"' and '"+args.fns[j]+"'.")
    if(in_size[i][1]>0):
        minspot=min(minspot, in_size[i][0])
        maxspot=max(maxspot, sum(in_size[i]))
if(fatal):
    print("Quitting due to above errors!")
    sys.exit(-2)
totalSize=maxspot-minspot
print("Merging code...")
for i in range(num_files):
    start=in_size[i][0]
    for j in range(start, start+in_size[i][1]):
        bte=in_code[i][j-start]
        if(not pyObj and type(bte)==str):
            try:
                wr32(out_names[bte])
            except:
                print("Error: label '"+bte+"' not found in provided object files.")
                fatal=True
        else:
            out_code.append(bte)
if(fatal):
    print("Quitting due to above errors!")
    sys.exit(-3)
if(pyObj):
    print("Making python object file '"+ofn+"'...")
    obj={"code":out_code,"lbl":out_names,"size":[minspot,totalSize]}
    f=open(ofn,'w')
    f.write(str(obj))
    f.close()
elif(args.coe):
    COE="memory_initialization_radix=16;\nmemory_initialization_vector=\n"
    for i in out:
        COE+=(hex(i)[2:]).zfill(2)+',\n'
    fout=COE[:len(COE)-2]+';'
    f=open(ofn+".coe",'w')
    f.write(fout)
    f.close()
    print("Wrote "+ofn+" ...")
else:
    print("Writing binary file...")
    f=open(ofn,'wb')
    f.write(bytes(out))
    f.close()
    print(str(len(out))+" bytes in size.")
print("done.")
