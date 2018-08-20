extern void out(char data, int adr);
extern char in(int adr);
extern void printf(char* str);

#define spi_sel_sd() out(16, 0)
/* select device 255 (doesn't exist) */
#define spi_desel() out(16, 255)

#define true 1
#define false 0

int main(){
	printf("Loading...\n");
	if(sd_init()){
		printf("Block device found: SD card\n");
	}else
		printf("Error initializing SD card.  No block devices, cannot boot.\n");
	while(true);
}

char sd_exec(char cmd, int args, char crc){
	int i=0, q;
	printf("- sd_exec -\n");
	spi_sel_sd();
	out(17, cmd|0x40);
	out(17, args>>24);
	out(17, args>>16);
	out(17, args>>8);
	out(17, args&255);
	out(17, crc);
	for(;i<64;i++){
		out(17, 0);
		q=in(17);
		if(q&0x80)return q;
	}
	return 0;
}

int sd_init(){
	int i=0;
	printf("- sd_init -\n");
	spi_desel();
	printf("clocking SD card...\n");
	for(;i<10;i++){
		out(17, 0); /* supply clocks to SD card */
	}
	printf("communicating...\n");
	for(i=0;i<10;i++){
        if(sd_exec(0,0,0x95)>0){
            break;
        }else if(i==9){
			return 0;
		}
	}
    return 1;
}

/*  Python code for test inside emulator :

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
    
*/