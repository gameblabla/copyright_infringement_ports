wine PictConv.exe -f6 -o2 final.png bakura.raw
wine PictConv.exe -f6 -o2 title.png title.raw
lzsa -r -f2 bakura.raw bakura.lz
lzsa -r -f2 title.raw title.lz
wine PictConv.exe -f6 -o2 oric1.png frame1.raw
wine PictConv.exe -f6 -o2 oric2.png frame2.raw
wine PictConv.exe -f6 -o2 oric3.png frame3.raw
wine PictConv.exe -f6 -o2 oric4.png frame4.raw
wine PictConv.exe -f6 -o2 oric5.png frame5.raw
cat frame1.raw frame2.raw frame3.raw frame4.raw frame5.raw > frames.raw
bin2c frames.raw img.h img
