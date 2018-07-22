@echo off
echo ----- BUILDING TOOLS -----
python pyinstaller.py as.py -F -n asplasma
rem python pyinstaller.py cpp.py -F -n cppplasma
python pyinstaller.py ld.py -F -n ldplasma
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
bin-windows\asplasma c_library\crt0.asm -po
bin-windows\asplasma c_library\stdio.asm -po
echo ----- CLEANING UP -----
del ldplasma.spec
del asplasma.spec

echo ----- BUILT -----
pause