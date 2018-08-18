asplasma bios_driver.asm -po
vbccplasma bios.c -o=bios.asm -off=0x80
asplasma bios.asm -po
ldplasma bios.po bios_driver.po -o bios.bin