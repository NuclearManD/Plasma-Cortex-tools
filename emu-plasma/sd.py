STATE_STARTING=0
STATE_READY=1
STATE_FETCH=2
STATE_RETURN=3
STATE_USTRT=4
class SD_card:
    def __init__(self,disk):
        open(disk,'a').close(); # create if doesn't exist
        self.file=open(disk,"w+b")
        self.state=STATE_STARTING
        self.count=74
        self.data=[0,0,0,0,0,0]
        self.buffer=b""
    def spi_exx(self, data_in):
        if(self.state==STATE_STARTING):
            self.count+=8 # if we are starting AND selected, then don't start.
            pass
        elif(self.state==STATE_READY):
            self.data[0]=data_in
            if(data_in==0xFF):
                return 0xFF
            elif(data_in&0x40>0):
                self.state=STATE_FETCH
            self.count=0
        elif(self.state==STATE_FETCH):
            self.count+=1
            if self.count>5:
                #print("SD ERROR:  state=fetch and count=",self.count)
                return 0
            self.data[self.count]=data_in
            if(self.count==5):
                self.state=STATE_READY
                self.exec()
        elif(self.state==STATE_USTRT and data_in==0x40):
            self.data[0]=0x40
            self.state=STATE_FETCH
            self.count=0
        elif(self.state==STATE_RETURN):
            self.state=STATE_READY
            return self.count
        else:
            print("SD card warning: invalid state, no match. goto state READY")
            self.state=STATE_READY
        return 0
    def spi_clk8(self):
        if(self.state==STATE_STARTING):
            self.count-=8
            if(self.count<=0):
                self.state=STATE_USTRT
    def exec(self):
        #print("SD: exec: data=",self.data)
        crc=self.data[5]
        cmd=self.data[0]
        args=self.data[1:-1]
        args.reverse()
        retval=255
        self.count=0
        self.state=STATE_RETURN
        if(cmd==0x40 and crc==0x95):
            retval=0
        elif(cmd==0x48 and crc==0x87):
            pass
        else:
            print("SD card warning: unknown command or invalid crc:")
            print("cmd=",hex(cmd))
            print("crc=",hex(crc))
            print("arg=",args)
            print("data=",self.data)
            return 0 # on error
        self.count=retval
def sd_exec(mysd, cmd, args, crc):
    mysd.spi_exx(cmd|0x40)
    mysd.spi_exx(args[3])
    mysd.spi_exx(args[2])
    mysd.spi_exx(args[1])
    mysd.spi_exx(args[0])
    mysd.spi_exx(crc)
    for i in range(128):
        retval=mysd.spi_exx(0)
        if(retval&0x80)==0:
            return retval
    return 0
def sd_test(mysd):
    for i in range(10):
        mysd.spi_exx(0)
    for i in range(10):
        if sd_exec(mysd,0,[0,0,0,0],0x95)==1:
            break;
        elif(i==9):
            return False
    return True
def sd_read(mysd,block):
    for i in range(20):
        if mysd.spi_exec(0xff)==0xff:
            break
        elif i==19:
            return False
    result = sd_exec(17, block)
    if (result&0xC0 != 0):
        return False
    // Reset the block byte count
    blockByteCount = 0;
    blockReadState = ReadStateWait;
    blockReadMode = ReadModeSingleBlock;
    chipSelectEnd();
