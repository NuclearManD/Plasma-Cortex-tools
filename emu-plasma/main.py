import spi, sd, cpu, kbd, ser
class computer:
    def __init__(self, cpu, deviceMap,ramq,rom=b'\x77'):
        self.cpu=cpu
        cpu.setup(self)
        self.devs=deviceMap
        self.ram=bytearray(int(ramq))
        for i in range(len(rom)):
            self.ram[i]=rom[i]
        self.ramlen=int(ramq)
    def tick(self):
        try:
            err=self.cpu.tick()
        except KeyboardInterrupt:
            err=-1
        return err
    def io_write(self, adr, data):
        dev=adr>>4
        if(dev in self.devs.keys()):
            self.devs[dev].io_write(adr&16,data)
        #print("io_wr "+hex(adr)+" <= "+hex(data))
    def io_read(self, adr, data):
        dev=adr>>4
        if(dev in self.devs.keys()):
            return self.devs[dev].io_read(adr&16)
        print("io_rd "+hex(adr))
        return 255
    def mem_write(self, adr, data):
        self.ram[adr%self.ramlen]=data%256
    def mem_read(self, adr):
        return self.ram[adr%self.ramlen]

print("Initializing...")
dev_spi=spi.SPI_controller()
dev_spi.add_spi_device(0,sd.SD_card("sysdisk.bin"))
dev_ser=ser.serial_port()
dev_kbd=kbd.PS2_keyboard()
device_map={0:dev_ser, 1:dev_spi, 2:dev_kbd}
plasma=cpu.plasma_cortex()
file=open("bios.bin",'rb')
boot_code=file.read()
file.close()
comp=computer(plasma, device_map, 1024*1024, boot_code)
super_verbose=True#False#True#False#True
while True:
    err=comp.tick()
    if(err==-1):
        if input("Nonfatal CPU error, continue? [Y/n]")!='Y':
            break
    elif err==-2:
        print("Fatal CPU error, force stop.")
        break
    if(super_verbose):
        plasma.print_details()
        input("Enter for next tick")
