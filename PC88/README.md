# Copyright Infringement for the PC-88

This is a port of a gay porn game that i made for numerous platforms, including the PC-88.
This is sort of basic and only supports V1 and the Beeper, the Yamaha sound chip isn't being used as it should
mostly due to the lack of suitable music for it and also because z88dk seemingly lacks FILE I/O for it (along with lacking support for adding files to the D88 image).

# Usage

To play the game, you have several options :
- Floppies (.D88)
- Tape/Casette (.WAV/.T88)

The game loads entirely in RAM thus it should work from tape.
For the floppy version, it is self booting but you should boot it from N88, not N80 as
otherwise you'll only get B&W graphics.

# Other technical aspects

This uses Z88DK for the basic support of the PC-88 so many thanks to them !
I had to write my own python script for converting PNG images to the PC-88's V1 format.
I haven't tried to figure out V2 too much but the only difference appears to be the remappable palette.

Animating compressed graphics was a big problem on this.
In the end, i resorted to using another algo called LZ48 and it was slightly faster than even ZX1-Mega
while having decent compression ratios.

I also sort of cheated by using the Red and Green layers as doing so reducing the overall effective size
from 48kb to 32kb, which makes it more bearable and we still get 4 colors.
Sure, no white color but it was a small loss in this case.

In the end i sort of managed but you can still see some of the greenish remanants due to the green layer
still updating too slowly... :/
I actually did get that even with uncompressed memcpy.
The only way i could possibly reduce this is with an unrolled 640 LDI loop but i'm not even sure that would be enough.

I don't believe this kind of FMVs had been done on the PC-88 anyway.
