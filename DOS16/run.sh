rm CGA.RAW CGA.SIF
python PNGtoCGA_NTSC.py CGA_COMP.PNG CGA.RAW
./convert_tool.elf CGA.RAW CGA.SIF 320 200 320 200
./convert_tool.elf EGA.ega EGA.SIF 640 350 640 350
./convert_tool.elf HGC.raw HGC.SIF 720 348 720 348
