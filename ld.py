# based on old NGX series assembler, then Plasma Cortex assembler, now it is the linker.
import argparse, sys, os
#sys.path.append("C:\\PlasmaCortex\\c_lib")
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
out_names={}
out_code=[]
location=0
def wr32(x):
    global location
    out_code[location]=(((x>>24)+256)&255)
    out_code[location+1]=((x>>16)&255)
    out_code[location+2]=((x>>8)&255)
    out_code[location+3]=(x&255)
    location+=4
print("Reading files...")
in_names=[]
in_code=[]
in_size=[]
num_files=0
for i in args.fns:
    try:
        j=i
        if(not os.path.exists(i)):
            j="C:\\PlasmaCortex\\c_lib\\"+j
        f=open(j)
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
            fatal=True
    """if(in_size[i][1]!=len(in_code[i])):
        print("Fatal: PO File "+args.fns[i]+" code size mismatch.")
        fatal=True"""
    if(in_size[i][1]>0):
        minspot=min(minspot, in_size[i][0])
        maxspot=max(maxspot, sum(in_size[i]))
if(fatal):
    print("Quitting due to above errors!")
    sys.exit(-2)
totalSize=maxspot-minspot
if pyObj:
    out_code=[-1]*totalSize
else:
    out_code=[0]*totalSize
print("code size: "+str(totalSize)+" bytes.")
print("offset: "+str(minspot))
print("Merging code...")
for i in range(num_files):
    location=in_size[i][0]
    location-=minspot
    j=0
    off=0
    while j < in_size[i][1]-off:
        bte=in_code[i][j]
        if(not pyObj and type(bte)==str):
            try:
                wr32(out_names[bte])
            except:
                print("Error: label '"+bte+"' not found in provided object files.")
                fatal=True
        else:
            if(location>=len(out_code)):
                print("ERROR: Linker bug found!")
                print("out_code length is "+str(len(out_code)))
                print("location: "+str(location))
                print("Current file: #"+str(i)+", aka "+args.fns[i])
                print("Start location: "+str(in_size[i][0]-minspot))
                fatal=True
                break
            out_code[location]=(bte)
            location+=1
        j+=1
        if( type(bte)==str):
            off+=3
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
    for i in out_code:
        COE+=(hex(i)[2:]).zfill(2)+',\n'
    fout=COE[:len(COE)-2]+';'
    f=open(ofn,'w')
    f.write(fout)
    f.close()
    print("Wrote "+ofn+" ...")
else:
    print("Writing binary file...")
    f=open(ofn,'wb')
    for i in out_code:
        if i<0 or i>255:
            print("Error: code contains invalid value "+str(i))
    f.write(bytes(out_code))
    f.close()
    print(str(len(out_code))+" bytes in size.")
print("done.")
