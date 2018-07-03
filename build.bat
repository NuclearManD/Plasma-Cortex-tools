@echo off
echo ----- BUILDING TOOLS -------
pyinstaller.py as.py -F
pyinstaller.py cpp.py -F
pyinstaller.py ld.py -F
rmdir bin /S /Q
move .\dist bin
echo ----- CLEANING UP ------
