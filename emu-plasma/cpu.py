top_val=2**32
class plasma_cortex:
    def setup(self, computer):
        self.regs=[0,0,0,0,0,0,0xFFFFFF,0]
        self.comp=computer
        self.s=-1
        self.c=0
    def tick(self):
        try:
            return self._tick_raw()
        except KeyboardInterrupt:
            print("cpu: STOP initiated by user;")
            if input("Print details? [Y/n]")=='Y':
                self.print_details()
                print("[End Details]")
            return -1
    def _tick_raw(self):
        opcode=self.comp.mem_read(self.regs[7])
        if(opcode&0x88)==0:
            # mov r, r
            if(opcode&7)<7:
                self.regs[7]+=1
            self.regs[opcode&7]=self.regs[opcode>>4];
        elif(opcode&0xF8)==0xF8:
            # mov r, *
            self.regs[7]+=5
            self.regs[opcode&7]=self.read_q(self.regs[7]-4)
        elif(opcode&0xF8)==0x28:
            # pop r
            self.regs[7]+=1
            self.regs[opcode&7]=self.pop()
        elif(opcode&0xF8)==0xA8:
            # push r
            self.regs[7]+=1
            self.push(self.regs[opcode&7])
        elif(opcode==0x7E):
            # lodsb
            self.regs[0]=self.comp.mem_read(self.regs[4])
            self.regs[4]+=1
            self.regs[7]+=1
        elif(opcode==0x7F):
            # stosb
            self.comp.mem_write(self.regs[5],self.regs[0])
            self.regs[5]+=1
            self.regs[7]+=1
        elif opcode==0xE8 :
            # jz *
            if(self.s==0):
                self.regs[7]=self.read_q(self.regs[7]+1)
            else:
                self.regs[7]+=5
        elif opcode==0xE9 :
            # jnz *
            if(self.s!=0):
                self.regs[7]=self.read_q(self.regs[7]+1)
            else:
                self.regs[7]+=5
        elif opcode==0xEA :
            # jc *
            if(self.c!=0):
                self.regs[7]=self.read_q(self.regs[7]+1)
            else:
                self.regs[7]+=5
        elif opcode==0xEB :
            # jnc *
            if(self.c==0):
                self.regs[7]=self.read_q(self.regs[7]+1)
            else:
                self.regs[7]+=5
        elif opcode==0x6C :
            # djnz *
            if(self.regs[2]==0):
                self.regs[7]+=5
            else:
                self.regs[7]=self.read_q(self.regs[7]+1)
                self.regs[2]-=1
        elif(opcode==0x7C):
            # call *
            self.push(self.regs[7]+5)
            self.regs[7]=self.read_q(self.regs[7]+1)
        elif opcode==0x69:
            # out *, ax
            self.comp.io_write(self.read_q(self.regs[7]+1),self.regs[0]&255)
            self.regs[7]+=5
        elif opcode==0x6B:
            # out dx, ax
            self.comp.io_write(self.regs[3], self.regs[0]&255)
            self.regs[7]+=1
        elif opcode==0x68:
            # in *, ax
            self.regs[0]=self.comp.io_read(self.read_q(self.regs[7]+1))
            self.regs[7]+=5
        elif opcode==0x6A:
            # in dx, ax
            self.regs[0]=self.comp.io_read(self.regs[3])
            self.regs[7]+=1
        elif (opcode&0x88)==0x80:
            # math
            r=self.alu(opcode&15,self.regs[0],self.regs[7&(opcode>>4)])
            if (opcode&0x40)>0:
                self.regs[7&(opcode>>4)]=r
            else:
                self.regs[0]=r
            self.regs[7]+=1
        elif (opcode&0xFC)==0x08:
            # mov r, [sp+*]
            #print(hex(self.regs[6]+1+self.read_ds(self.regs[7]+1)))
            self.regs[opcode&3]=self.read_q(self.regs[6]+1+self.read_ds(self.regs[7]+1))
            self.regs[7]+=3
        elif (opcode&0xFC)==0x88:
            # mov [sp+*], r
            self.write_q(self.regs[6]-3+self.read_ds(self.regs[7]+1),self.regs[opcode&3])
            self.regs[7]+=3
        elif (opcode&0x08)==8 and ((opcode&0xF0)>=0x30) and ((opcode&0xF0)<=0x50):
            # mov r, [r]
            self.regs[opcode&7]=self.read_q(self.regs[(opcode>>4)&7])
            self.regs[7]+=1
        elif (opcode&0x08)==8 and ((opcode&0xF0)>=0xB0) and ((opcode&0xF0)<=0xD0):
            # mov [r], r
            self.write_q(self.regs((opcode>>4)&7),self.regs[opcode&7])
            self.regs[7]+=1
        elif (opcode==0x7d):
            self.regs[6]+=self.read_q(self.regs[7]+1)
            self.regs[7]+=5
        else:
            print("cpu: error: invalid opcode "+hex(opcode))
            self.print_details()
            print("[Error details end]")
            return -1
        return 0
    def print_details(self):
        print("Registers:")
        print("  ax = "+hex(self.regs[0]))
        print("  bx = "+hex(self.regs[1]))
        print("  cx = "+hex(self.regs[2]))
        print("  dx = "+hex(self.regs[3]))
        print("  si = "+hex(self.regs[4]))
        print("  di = "+hex(self.regs[5]))
        print("  sp = "+hex(self.regs[6]))
        print("  pc = "+hex(self.regs[7]))
        print("Stack (Latest push first) (last 5 elements):")
        sp=self.regs[6]
        for i in range(5):
            print(hex(self.regs[6]+1)+" : "+hex(self.pop()))
        self.regs[6]=sp
    def read_q(self, adr):
        num = self.comp.mem_read(adr  )<<24
        num|=(self.comp.mem_read(adr+1))<<16
        num|=(self.comp.mem_read(adr+2))<<8
        num|=(self.comp.mem_read(adr+3))
        return num
    def write_q(self, adr, num):
        self.comp.mem_write(adr,  num>>24)
        self.comp.mem_write(adr+1,num>>16)
        self.comp.mem_write(adr+2,num>>8)
        self.comp.mem_write(adr+3,num)
    def read_ds(self, adr):
        num =(self.comp.mem_read(adr))<<8
        num|=(self.comp.mem_read(adr+1))
        if num>32767:
              num-=65536
        return num
    def push(self, num):
        self.write_q(self.regs[6]-3,num)
        self.regs[6]-=4
    def pop(self):
        num=self.read_q(self.regs[6]+1)
        self.regs[6]+=4
        return num
    def alu(self, op, a, b):
        result=2**32-1
        if(op<8):
            if op==0:
                result=a+b
            elif op==1:
                result=a-b
            elif op==2:
                result=a*b
            elif op==5:
                result=a^b
            elif op==6:
                result=a&b
            elif op==7:
                result=a|b
        else:
            if op==0:
                result=b<<1
            elif op==1:
                result=b>>1
            elif op==2:
                if b>=(2**31):
                    result=(b>>1)|(2**31)
                else:
                    result=b>>1
            elif op==3:
                result=int(abs(b))
            elif op==4:
                result=top_val-1-b
            elif op==6:
                result=b+1
            elif op==7:
                result=b-1
        if result<0:
            result+=top_val
        self.c=result>>32
        result=result%top_val
        self.s=result
        return result
