class plasma_cortex:
    def setup(self, computer):
        self.regs=[0,0,0,0,0,0,0xFFFFFF,0]
        self.comp=computer
    def tick(self):
        opcode=self.comp.mem_read(self.regs[7])
        if(opcode&0x88)==0:
            # mov r, r
            self.regs[7]+=1
            self.regs[opcode&3]=self.regs[opcode>>4];
        elif(opcode&0xF8)==0xF8:
            # mov r, *
            self.regs[7]+=5
            self.regs[opcode&3]=self.read_q(self.regs[7]-4)
        elif(opcode&0xF8)==0x28:
            # pop r
            self.regs[7]+=1
            self.regs[opcode&3]=self.pop()
        elif(opcode&0xF8)==0xA8:
            # push r
            self.regs[7]+=1
            self.push(self.regs[opcode&3])
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
            self.comp.io_write(self.read_q(self.regs[7]+1),self.regs[0]&255)
            self.regs[7]+=5
        elif opcode==0x6B:
            self.comp.io_write(self.regs[3], self.regs[0]&255)
            self.regs[7]+=1
        elif opcode==0x68:
            self.regs[0]=self.comp.io_read(self.read_q(self.regs[7]+1))
            self.regs[7]+=5
        elif opcode==0x6A:
            self.regs[0]=self.comp.io_read(self.regs[3])
            self.regs[7]+=1
        else:
            print("cpu: error: invalid opcode "+hex(opcode))
    def read_q(self, adr):
        num = self.comp.mem_read(adr  )<<24
        num|=(self.comp.mem_read(adr+1)<<16)&255
        num|=(self.comp.mem_read(adr+2)<<8 )&255
        num|=(self.comp.mem_read(adr+3)    )&255
        return num
    def write_q(self, adr, num):
        self.comp.mem_write(adr,  num>>24)
        self.comp.mem_write(adr+1,num>>16)
        self.comp.mem_write(adr+2,num>>8)
        self.comp.mem_write(adr+3,)
    def push(self, num):
        self.write_q(self.regs[7]-3,num)
        self.regs[7]-=4
    def pop(self):
        rum=self.read_q(self.regs[7]+1)
        self.regs[7]+=4
        return num
