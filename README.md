Copyright Infringement  ports
==========================================
**WARNING** : Don't play kids. Don't play it at work either.

This is a repo for all of the versions of Copyright Infringement : The regressive right,
a game where Rikuto has trapped you in one of his games and paired you with another mate.
It is a very short game that was initially made for the Colecovision to demonstrate that it is possible
to achieve FMVs on the Colecovision through the use of MegaROM bank switcher.

Later, another unrelated game for the TI-99/4a (Dragon's Lair) took it a step further
and did full motion video with PCM sound at 13khz and on a gigantic 512MB rom !

Planned ports
=============

- TI 82/83/84
- BK Electronika (Might as well piss off Russia on their own ground !)
- Tatung Einstein (WIP, on hold right now due to technical issues)
- Grundy NewBrain
- NES (Very unfamiliar with that one...)
- Amiga CD32 (eventually)


CBS Colecovision
=================

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/cbs.jpg)

This was the initial port of the game.
No speech, the image quality is decent and it wasn't too hard once i figured out that i could write to the VDP memory directly.
I tried to post the game on AtariAge and it did bring up a good discussion about FMVs and taking it further before it was taken down.

This is so far the only port to use sprites for the HUD as the other ones so far don't use it.

![Download Colecovision version here](https://github.com/gameblabla/copyright_infringement/raw/master/game_buk.rom)

MSX2
====

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/msx.jpg)

This was the next port years later after the Colecovision version.
I added speech for the intro, used screen 5 for the graphics and i think other than that, it's quite pleasing to look at.

Why the MSX ? Originally i wanted to do a straight port of the Colecovision as the two share the same VDP (at least for MSX1).
However i thought it would be a shame to restrict myself to MSX1 and i also encountered an issue with ASCII16 that is said to have a 512kb limit on real hardware.
I did not want to risk real hardware compatibility while i wait for my MSX2 to arrive so this version is MSX2 only.

I also did consider an MSX2+ version but pushing YJK frames at decent speeds on it will be very challenging
and i want to avoid requiring a boosted Z80 like on Panasonic's models or even a TurboR
so this version will have to wait for one.

This one does not use sprites unlike the Colecovision : only a part of the screen is being refreshed.
The other reason is that, while i could make sprites work on MSX1, they work differently on MSX2.

However if i work on an MSX1 port, i will have to use sprites again due to the color attribute potentially
clashing with the nature of the game.

[Download MSX2 version](https://github.com/gameblabla/copyright_infrigement_msx/releases/tag/1.0)


ZX Spectrum 48k
===============

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/spectrum.jpg)

I wanted to make something for the ZX spectrum (i did make a very simplistic game a long time ago for it called Red Square) so i decided i should experiment with this.
The ZX Spectrum is quite barebones : a CPU and a screen buffer arranged in a strange way.
However, unlike other platforms, it's pretty good at this job considering its specs and the 3D games on Spectrum performed better than their MSX counterpart.
The spectrum is essentially an upgraded ZX81 with a color attribute table and more memory.

This port was more challenging than i thought it would be.
Originally i wanted this to be a cartridge game but z88dk does not support bank switching and i'm not aware of anyone attempting that
except through some custom assembly. I estimated that i could only fit 3-5 compressed frames at best.
Needless to say, i gave up on that idea and instead went the casette route.

However, going the casette way means that you are being limited by the amount of memory that's on the computer.
Even on a ZX Spectrum 128, it's still 64kb unless you do bank switching.
For now, i decided to target the 48k model and use ZX1 compression as it is the fastest algo on the ZX spectrum.

I also attempted to combine it further with RCS but results show the two combinaison are too slow for FMVs
so went back to simply using ZX1 for animated frames and RCS+ZX1 for static screens.
I should point out that this still only saves about half of the space.

I also considered the use of Biforst2 (8x1 multicolor) or even Gigascreen but these would require more bandwidth and a lot more memory as well.
If i ever make use of these, it will have to be for a Spectrum 128k version.

[Download ZX Spectrum 48k version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0)


V.Smile and compatibles
=======================

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/vsmile.jpg)

As far as i know, this is the first V.smile homebrew game outside of experimental demos.

This is a really strange beast, especially it's CPU and the way controls work (through UART).
The V.Smile actually supports hicolor ! But i stuck with 8bpp due to bugs with the text layer when combined with hicolor.
I might upgrade this to hicolor after further hardware tests...

This port does not have any PCM sound for now as i've been told attempts so far failed on real hardware.
I may look into this eventually myself.

It's also not using sprites but the hardware was decently powerful enough for sprites not to be needed in my case.
I may want to use them eventually but this game does not need them.

Game is entirely 8bpp although i do have a 16-bits variant that's also provided for testing and if you guys are curious.

Lack of sound is a shame so i may revisit this eventually.

[Download VSmile version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-vsmile)


Exidy Sorcerer
=================

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/exidy.jpg)

This port was challenging due to technical contraints.
At first, i initially wanted to go the putsprite way as the hardware lacks a proper graphics mode so to speak.
However, i only ended up using this for static screens as it is way too slow otherwise.
This is in part to z88dk's drawing routines relying on the pixel plotting functions and doing a lot of checking...

I then had the idea of basically treating the 512x256 character-based display and defining each character as a pixel.
Doing so would achieve a resolution of 64x30. It is then a matter of simply uploading the bitmapped graphics to 0xF080, 
which contains the buffer for the text characters onscreen.

The amusing part is that the default font did not have a completely white block to achieve this.
However, it's possible to upload your own character set in the 127-255 range so that's what i did.

Other games went further with this and made their own proper character set and worked around this
but this would have been very annoying to deal with and probably not fast enough for my purpose.

Sadly z88dk doesn't support sound for this target so it has been disabled for this build.
The 64x30 mode also looks very chunky, and i thought the 128x60 would also be challenging to be honest hehe...
The results aren't pretty but it is smooth however.

You may also wonder how i managed to fit 5 frames of animation and still make it work on an 8KB machine ?
Well thanks to the ZX7 compressor this is doable now. Frames that would normally be 1920 bytes large are
compressed down to 280 bytes or so. The decompressor is also 240 bytes or so and plenty fast as well so it's worth it.

I might actually make a proper game for this someday but not a lot of people are interested in business machines like this one.
Perhaps i'll do a flappy bird like i did on the Laser 200.

[Download Exidy version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-exidy)

Sinclair QL
=================

![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/QL.jpg)

This port took longer than it should have been, mostly due to lack of support, tools and documentation.
I honestly wanted to give up but decided it would be a shame to give up since this system barely has any games !

This version performs pretty good and i think technically wise, it should hold up.

It's a shame the Sinclair QL only has a primitive beeper that you can only send commands to
via an intel microcontroller which rules out 1-bit PCM even.

The Sinclair QL is definitively a mixed bag : 
it has a much better CPU than the Zx Spectrum but the graphical upgrades are not really massive.
The only notable difference on the QL is the complete lack of color clashing and a higher resolution mode with 4 colors. 
This game uses the low resolution with 8 colors but with dithering, both modes look interesting i would say.
Dithering is not used in-game as it would increase the RAM consumption even further...
However it is used for the static screens.

I also borrowed OS/screen 2 memory for storing data as we're really running out of space...
Forget about using malloc on this thing ! 
Just push your data to a fixed location and follow the memory map.
Yes, that forces you to track your own memory usage but that can be a good thing honestly.

There was also initially a bug with the game overwriting some of the systems functions for text drawing etc...
resulting in the game outputting no text ! I had to move up the memory location slightly higher to avoid this issue.
There are ways around this but they all involve disabling the OS or moving it to another location,
both of which have their own disadvantages and complications. Therefore, i decided to do that instead.

[Download Sinclair QL version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-SinclairQL)
