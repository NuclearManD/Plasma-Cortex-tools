import os
STATE_IDLE=0
STATE_READ=1
STATE_WRTE=2
STATE_RET=3
STATE_GETC=4
STATE_GETS=5
gig=2097152
class blockdev_file:
    def __init__(self, fn, cap=1*gig, meta="Virtual Block Device"):
        open(fn,"a+").close()
        with open(fn, "r+b") as out:
            out.seek((cap*512))
            out.write(b'\0')
        self.file=open(fn, 'r+')
        self.state=STATE_IDLE
        self.cnt=0
        self.blk=0
        self.seg=0
        self.meta=meta
        self.retdat=[]
        self.cap=cap
    def spi_exx(self, data):
        state=self.state
        if(state==STATE_IDLE):
            # accept command
            if data==109:
                self.retdat=list(self.meta.encode())+[0]
                self.state=STATE_RET
            elif data==114:
                self.cnt=512
                self.state=STATE_READ
                self.file.seek(512*self.blk)
            elif data==119:
                self.cnt=512
                self.state=STATE_WRITE
                self.file.seek(512*self.blk)
            elif data==67:
                self.retdat=[(self.cap>>24)&255,(self.cap>>16)&255,(self.cap>>8)&255,(self.cap)&255]
                self.state=STATE_RET
            elif data==99:
                self.state=STATE_GETC
                self.blk=0
                self.cnt=4
            return 0
        elif( state==STATE_READ ):
            self.cnt-=1
            if data==0x55:
                self.cnt=0 # CANCEL!
            r=chr(self.file.read(1))
            if self.cnt==0:
                self.state=STATE_IDLE
            return r
        elif( state==STATE_WRTE ):
            self.cnt-=1
            self.file.write(data)
            if self.cnt==0:
                self.state=STATE_IDLE
            return 0x55
        elif state==STATE_RET:
            r=self.retdat[0]
            if len(self.retdat)==1:
                self.state=STATE_IDLE
            else:
                self.retdat.remove(self.retdat[0])
            return r
        elif state==STATE_GETC:
            cnt-=1
            self.blk|=data<<(cnt*8)
            if cnt==0:
                state=STATE_IDLE
            return 0
    def spi_clk8(self):
        return

def test(bd):
    bd.spi_exx(109)
    string=""
    while True:
        c=bd.spi_exx(0)
        if c==0:
            break
        string+=chr(c)
    print(string)
        
