@echo off
echo ----- BUILDING TOOLS -------
pyinstaller.py as.py -F
pyinstaller.py cpp.py -F
pyinstaller.py ld.py -F
rmdir bin-windows /S /Q
move .\dist bin-windows
echo ----- BUILDING CROSS COMPILER ------
cd vbcc
make bin/vc
make TARGET=plasma bin/vbccplasma
make TARGET=bi386 bin/vbcc386
cd ..
copy vbcc\bin\vc.exe bin-windows\vc.exe
copy vbcc\bin\vbccplasma.exe bin-windows\vbccplasma.exe
copy vbcc\bin\vbcc386.exe bin-windows\vbcc386.exe
echo ----- CLEANING UP ------
