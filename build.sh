echo "----- BUILDING TOOLS -----"
python3 pyinstaller.py as.py -F -n asplasma
#python3 pyinstaller.py cpp.py -F -n cppplasma
python3 pyinstaller.py ld.py -F -n ldplasma
rmdir bin-linux --ignore-fail-on-non-empty
mv dist bin-linux
echo "----- BUILDING CROSS COMPILER -----"
cd vbcc
make bin/vc
make TARGET=plasma bin/vbccplasma
cd ..
cp vbcc/bin/vc bin-windows/vc
cp vbcc/bin/vbccplasma bin-windows/vbccplasma
echo "----- BUILDING CSTDLIB -----"
./bin-linux/asplasma c_library/crt0.asm -po
./bin-linux/asplasma c_library/stdio.asm -po
echo "----- CLEANING UP -----"
rm ldplasma.spec
rm asplasma.spec

echo "----- BUILT -----"
