# Copyright Infringement C64

This is a port for the C64, but it's somewhat basic as the whole game has to stay in RAM.
I decided to do it like so in the case i want to convert this to a cartridge game.
There is a provided PRG2CRT version but it's not designed to take advantage of the mapper, it's just a straight conversion.

The C64 version was quite challenging to do, and ironically the biggest challenge on this is certainly the RAM.
I initially wanted to use compression with lzsa for still frames.
However even if i had done that, i would have only saved 4kb as the animated frames have to be uncompressed
as the C64's CPU is barely fast enough for B&W graphics.

