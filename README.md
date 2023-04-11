Copyright Infringement  ports
==========================================
**WARNING** : Don't play kids. Don't play it at work either.

This is a repo for all of the versions of Copyright Infringement : The regressive right,
a game where Rikuto has trapped you in one of his games and paired you with another mate.
It is a very short game that was initially made for the Colecovision to demonstrate that it is possible
to achieve FMVs on the Colecovision through the use of MegaROM bank switcher.

Later, another unrelated game for the TI-99/4a (Dragon's Lair) took it a step further
and did full motion video with PCM sound at 13khz and on a gigantic 512MB rom !


List
====
![CBS Colecovision](https://github.com/gameblabla/copyright_infringement_ports#cbs-colecovision)

![MSX2](https://github.com/gameblabla/copyright_infringement_ports#msx2)

![ZX Spectrum 48k](https://github.com/gameblabla/copyright_infringement_ports#zx-spectrum-48k)

![V.Smile](https://github.com/gameblabla/copyright_infringement_ports#vsmile-and-compatibles)

![Exidy Sorcerer](https://github.com/gameblabla/copyright_infringement_ports#exidy-sorcerer)

![Sinclair QL](https://github.com/gameblabla/copyright_infringement_ports#sinclair-ql)

![BK Elektronika BK-0010](https://github.com/gameblabla/copyright_infringement_ports#bk-elektronika-bk-0010)

![PMD-85](https://github.com/gameblabla/copyright_infringement_ports#pmd-85)

![LVIV/LVOV PK01](https://github.com/gameblabla/copyright_infringement_ports#lvivlvov-pk01)

![Excalibur 64](https://github.com/gameblabla/copyright_infringement_ports#excalibur-64)

![Oric 1/Oric Atmos](https://github.com/gameblabla/copyright_infringement_ports#oric-1oric-atmos)

![TI 83 CE](https://github.com/gameblabla/copyright_infringement_ports#ti-83-ce)

![Nintendo NES](https://github.com/gameblabla/copyright_infringement_ports#nes)

![Commodore 64](https://github.com/gameblabla/copyright_infringement_ports#commodore-64)

![NEC PC-88](https://github.com/gameblabla/copyright_infringement_ports#nec-pc-88)

![NEC PC-98](https://github.com/gameblabla/copyright_infringement_ports#nec-pc-98)


CBS Colecovision
=================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/cbs.jpg)

![Download Colecovision version here](https://github.com/gameblabla/copyright_infringement/raw/master/game_buk.rom)
<details>
This was the initial port of the game.
No speech, the image quality is decent and it wasn't too hard once i figured out that i could write to the VDP memory directly.
I tried to post the game on AtariAge and it did bring up a good discussion about FMVs and taking it further before it was taken down.

This is so far the only port to use sprites for the HUD as the other ones so far don't use it.
</details>

MSX2
====
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/msx.jpg)

[Download MSX2 version](https://github.com/gameblabla/copyright_infrigement_msx/releases/tag/1.0)

<details>
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
</details>

ZX Spectrum 48k
===============
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/spectrum.jpg)

[Download ZX Spectrum 48k version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0)

<details>
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
</details>

V.Smile and compatibles
=======================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/vsmile.jpg)

[Download VSmile version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-vsmile)

<details>
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
</details>

Exidy Sorcerer
=================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/exidy.jpg)

[Download Exidy version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-exidy)

<details>
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
</details>

Sinclair QL
=================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/QL.jpg)

[Download Sinclair QL version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-SinclairQL)

<details>
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
</details>

BK Elektronika (BK-0010)
=========================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/bk.jpg)

[Download BK-0010 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-BK)

<details>
This russian computer was notable back then as it was the cheapest computer most could afford. (still worth 3 months of work !)
I had to port this gay game to computer...

This port was extremely challenging for multiple reasons :
- I had to figure out how to even get a working binary on this as there was not toolkit to speak of
- Once i did that, i had to figure out the hardware, which required me to hop through multiple documents and blog posts, including in Russian.
- The PDP-11 GCC backend is extremely buggy and would produce broken executables at -O1 and higher.
- This forced me to rely a lot more on PDP-11 assembly and functions.
- Once i did know about all of this, trying to fit the game into 15.5Kb of RAM was extremely challenging.

The results are mixed but i still managed to fit the animations as well as beeps and all.
I may consider having an expanded version for later BK revisions but i would rather port this to more capable Soviet
computers like the Vector-06C or the Corvette.
</details>

PMD-85
=========================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/pmd.jpg)

[Download PMD-85 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-PMD)

<details>
This was a Czechoslovak computer that was, according to a Czechoslovak that covered soviet computers back then, well loved.
The initial versions were completely black & white but very late into its lifespan, color output was added.
Then the color ouput was later refined into ColorAce, which this game takes advantage of.
The nice thing about ColorAce is that it is also backwards compatible with non-ColorAce units, although it might look off on these machines.

This port was very simple to do. Unfortunately RAM is an issue and the 48k amount is misleading due to how the memory map is laid out.
In reality, this is more like a 32k machine and you have to load the games from casette so it's still pretty limiting.
It's only slightly better than the BK-0010 version because of the extra memory but that's it.
</details>

Lviv/Lvov PK01
===================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/lviv.jpg)

[Download LVIV version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-LVIV)

<details>
Another soviet computer that has a very strange memory layout.
If you disable Video RAM, you have full access to the 48kb of RAM.
Otherwise, you only get 16kb to play with... What a waste.

At least there's a wide range of 4 color palettes to choose from !
</details>

Excalibur 64
============
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/Excalibur_64.jpg)

[Download Excalibur 64 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-E64)

<details>
This is probably the worst port along with the Exidy Sorcerer version though this one
does at least have 16 colors (not all are used to the garish color palette).

This was very challenging to even make it work and the lack of documentation further hurt it.
It's a shame as according to dom (z88dk maintainer), you can actually emulate a 320x288 pixel mode by using the PCG banks together.

This game as it is only uses 160x72 for the story text and 80x25 ingame, resulting in a very chunky look.
Additionally, no sound due to lack of documentation on how to even output sound...

Cool name for a computer though, maybe i'll revisit it for my next game.
</details>

Oric 1/Oric Atmos
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/atmos.jpg)

[Download Oric version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-ORIC)

<details>
I haven't really dealt with any 6502-based machine for quite some time...
The Oric is quite interesting and shares a lot in similar with the ZX Spectrum.

I would argue it's quite better than the Spectrum due to the slightly superior graphics and 3 channel sound.
The first Oric sold 200,000 units in France and UK for these reasons along with the price matching the Spectrum.
However they never really evolved beyond that and soon faster computers came out...

As for the port itself, besides dealing with the RAM, it was mostly straight forward
tho unlike on the Z80, it turns out LZSA is too slow for decompressing frames on it !
I will have to think of something for the C128 and C64 ports...

The Oric, thankfully, has a decent amount of memory and not a retarded memory mapping either
so it was easy to get something to work. Not a bad version i would say.
</details>

TI-83 CE
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/ti83ce.jpg)

[Download TI83CE version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-TI83CE)

<details>
I found one by sheer luck for really cheap (15€) instead of the usual expensive prices so thought that would be an opportunity.
The C toolchain for it is really nice and uses Clang as the new backend (previously it was using a proprietary solution but that's now largely fixed).
ZX0/ZX7 are also directly integrated so that saved me some time.
The only issue was the size for archive files was small. It's possible i believe to go beyond the 65k limit by having several ones
but i only learned of this much later...
In any case, i didn't want this game to be too big so i stuck with 64kb and instead upscaled the 160x100 frames to 320x200 in short of a nasty way.
It works fine though, since this is decently powerful.

I implemented highscore saving (this is based upon the Oric version) but it seems not to work reliably so unsure...
It's still there in any case.
</details>

NES
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/nes.png)

[Download NES version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-NES)

<details>
The NES is something i always avoided due to the sheer complexity of it.
On the surface, it seems simple enough but doing anything complex becomes a huge chore.
If you want to make your first NES game, definitively start first with NROM.

This game is also illsuited for the NES as the PPU isn't designed for bitmapped graphics unlike most old computers are.
Updating the nametable takes too many cycles so the only proper way of getting close to that was to swap out the CHR banks, which is what i did.

The NES also has hardware DPCM support but the way it's laid out means that if you need more than 16kb, you'll have to switch banks.
This port does make extensive use of voices, even further than the MSX2 version as they are used ingame also.
Overall, it wasn't too difficult but this console isn't as easy as it may seem at first !

</details>

Commodore 64
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/c64.jpg)

[Download C64 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-C64)

<details>
The Commodore 64 is, much like the NES, illsuited for this kind of game although not as bad as the NES was in this regard.
The biggest issue is that it's simply too slow to update 8k + 2k of color fast enough on the puny 6502 that's in the C64.
In the end i resorted to B&W graphics in the bitmapped multicolor mode and that sort of works OK enough to get around 6 frames per second.
The only instance of the game having colors is in the Rikuto screens...

There's also some PCM samples in there although it's heavily downsampled, 4-bits and you can barely make out what he says.
I still wanted to include it however.
</details>

NEC PC-88
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/pc88.jpg)

[Download PC-88 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-PC88)

<details>
The NEC PC-88 (and the PC-98) is the home to numerous hentai games. 
Thus, this game should have perfectly suitable for this hardware.
Except not really, because outside of japanese documents (and even then, there's not a huge lot of it!),
they are surprisingly poorly documented.
I did get a VGM player working on it thanks to pc88-c but since i can't compose anything on it,
all you gonna get are simple beeps. Oh and forget about the ADPCM card, i have no idea how to use that either.
(Most PC-88 don't have it anyway)

The other big issue is the huge amount of VRAM it needs to update and the lackluster CPU for it.
I mean, updating 48kb multiple times per second on a puny Z80 is no small task !
I cheated a little bit by downgrading it to 4 colors in game to get it in a playable state.
It works but you can still see the graphics chip struggling to keep up.

I'm sure with delta compression, compressed code etc... i could get it further than this
but even if i could figure that out, z88dk's support for non-CP/M platforms is also lackluster as well !

I think i did a decent-ish job but it could have been better even considering the hardware.
</details>


NEC PC-98
==================
![](https://github.com/gameblabla/copyright_infringement_ports/raw/simp/img/pc98.jpg)

[Download PC-98 version](https://github.com/gameblabla/copyright_infringement_ports/releases/tag/1.0-PC98)

<details>
The sucessor to the PC-98 and it sold well, despite NEC releasing better computers like the PC-88VA during its lifetime..

Like NEC PC-88, it was a business machine for the most part and got plenty of adult games because of it.
On paper, the PC-98 should have enough grunts to have the game run well on this.
However, this only applies to the later models with 386s, as earlier models may struggle with this game.

Interestingly, the 8 colors 640x200 video mode is exactly the same as the PC-88.
In fact it's how i initially managed to make my PC-88 work on the PC-98!
The 16 colors add only adds an intensity plane on top of the RGB ones.

However, because of the greatly increased resolution (640x400) and color count,
you now have to push a whopping 128kb of VRAM stuff onscreen.

Unlike the PC-88, the address space is large enough to contain it so this isn't the problem...
The problem is that earlier models have CPUs that are just not fast enough to compensate for the
increased VRAM count.

There are ways around this, such as instead using the 640x200 16 colors mode.
However this mode actually won't work on later PC-9821 models as NEC completely dropped it (along with B&W modes).

According to my limited testing, it only becomes smooth at around 20mhz on a 386.
This version, like the PC-88, acts like a PC-Booter but uses an internal copy of FreeDOS along with DOS 2.0's open sourced
COMMAND.COM to reduce memory usage further.

The game may use more conventional memory than usual, especially with the P86 mode so be careful.

</details>
