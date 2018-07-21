@echo off
echo ----- BUILDING TOOLS -----
python pyinstaller.py as.py -F
rem python pyinstaller.py cpp.py -F
python pyinstaller.py ld.py -F
rmdir bin-windows /S /Q
move .\dist bin-windows
echo ----- BUILDING CROSS COMPILER -----
cd vbcc
make bin/vc
make TARGET=plasma bin/vbccplasma
cd ..
copy vbcc\bin\vc.exe bin-windows\vc.exe
copy vbcc\bin\vbccplasma.exe bin-windows\vbccplasma.exe
echo ----- BUILDING CSTDLIB -----
bin-windows\as c_library\crt0.asm -po
bin-windows\as c_library\stdio.asm -po
echo ----- CLEANING UP -----
del ld.spec
del as.spec

echo ----- BUILT -----
pause