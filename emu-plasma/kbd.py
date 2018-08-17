class PS2_keyboard:
    def __init__(self):
        self.avail=0
        self.keys=[]
    def io_read(self, adr):
        if(adr==0):
            return self.keys.remove(self.keys[0]) # inefficient, I know.
        elif(adr==1):
            return self.avail
    def io_write(self, adr, data):
        return
