class SPI_controller:
    def __init__(self):
        self.devices={}
        self.sel=-1
        self.read=255
    """ SPI addressing
          0 : read/write select
          1 : read/write SPI bus

        Note: Reading port 1 gives the return of the previous write.
              Writing port 1 initiates a 1-byte write.
    """
    def io_read(self, adr):
        if(adr==0):
            return self.sel
        elif(adr==1):
            return self.read
        return 255
    def io_write(self, adr, data):
        #print("SPI write : "+hex(adr))
        if(adr==0):
            self.sel=data
        elif(adr==1):
            if(self.sel in self.devices.keys()):
                self.read=self.devices[self.sel].spi_exx(data)
            else:
                for i in self.devices.values():
                    i.spi_clk8()  # for devices like SD cards that need clocks without being selected.
    def add_spi_device(self,index,device):
        self.devices[index]=device
def sd_exec(spi, cmd, args, crc):
    spi.io_write(0,0) # ensure SD is selected
    spi.io_write(1,cmd|0x40)
    spi.io_write(1,args[3])
    spi.io_write(1,args[2])
    spi.io_write(1,args[1])
    spi.io_write(1,args[0])
    spi.io_write(1,crc)
    for i in range(128):
        spi.io_write(1,0)
        retval=spi.io_read(1)
        if(retval&0x80)>0:
            return retval
    return 0
def test_spi_with_sd(spi,sd):
    spi.add_spi_device(0,sd)
    spi.io_write(0,-1)
    for i in range(10):
        spi.io_write(1,0)
    for i in range(10):
        if(sd_exec(spi,0,[0,0,0,0],0x95)>0):
            break;
        elif(i==9):
            return False
    return True
    
