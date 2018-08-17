class serial_port:
    def __init__(self):
        self.keys=[]
    def io_read(self, adr):
        if(adr==0):
            if(len(self.keys)==0):
                s=input()
                for i in s:
                    self.keys.insert(0,ord(i))
                self.keys.insert(0,10)
            return self.keys.pop() # inefficient, I know.
        elif(adr==1):
            return len(self.keys)
    def io_write(self, adr, data):
        print(chr(data%256), end='')

def ser_test():
    ser=serial_port()
    for i in b"Hello World!\n":
        ser.io_write(0,i)
    while(True):
        c=ser.io_read(0)
        ser.io_write(0, c)
        if(c==10):
            break;
