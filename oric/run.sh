rm hello2.tap CI_Regressiveright.tap
#wine PictConv.exe -f6 -o3 final.png img.h
cl65 -Ors -t atmos -C ./oric.cfg main.c lzsaf.asm input.s draw.s -o CI_Regressiveright.tap -D__AUTORUN__=$C7
#oricutron -m atmos CI_Regressiveright.tap
#mame orica -cass $PWD/CI_Regressiveright.tap  -autoboot_delay 5 -autoboot_command cload\"\"\\n -window
