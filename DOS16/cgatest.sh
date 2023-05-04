#!/bin/sh
rm CGAC.RAW CGA.SIF
python PNGtoCGA_NTSC.py OG.PNG CGAC.RAW
lzsa -r -f1 CGAC.RAW CGAC.lz1
./convert_tool.elf CGAC.lz1 CGA.SIF 320 200 320 200
dosbox-x -set machine=cga_composite main.exe 
