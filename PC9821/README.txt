Copyright Infringement PC-9821
===============================

This is the PC-9821 port of CI Regressive Right, the improved version over the previous PC-9801 version.
This one does require a PC-9821 unlike the PC-9801 version.

This initial release supports all major sound cards, with the exception of the Soundblaster 16.
(I still haven't figured that one out)

Like the PC-98 version, this one uses PMD for voices and music.
It also one features support for 86- based boards and Mate-X as well as the PC-9801-24K sound board.
Unlike the PC-98 version, this one makes use of the two planes to support double buffering !
So it should look pretty smooth.

Other than that, it's a typical porn game for the PC-9821 !

Requirements
=============

Minimum : 
- Any 386 processor
- Supports PEGC 640x400 256 colors mode (PC-9821 compliant)
- 3.6 MB of RAM

Recommended : 
- 486 processor (Should also work on Pentium-based PC-9821)

This game will not work on the very early PC-9821 that came with only 1.6mb.
If you own such a rarity, i would advise you to upgrade the memory on that !
Make sure that your PC-9821 actually comes with memory, 
as i've seen online auctions take out the memory from the PCs and sell it separately.

Note that this will not work on later PC-98 PCs that are DOS only and use software solutions.
These may actually work through their software solution but really, what's the point ???

CanBe PC-9821 laptops are also untested.

Usage
======

This game acts as a PC-Booter as to have the maximum amount of memory available as it is very demanding on memory.
Simply insert the floppy disk (make sure it's turned off) and power it on.
It should autoboot to its own text-based menu asking you which sound card setup you want to use.

If you only have a -86 soundcard, choose the first option.
If you only have a PC-9801-24K sound card, choose the 3rd option

Otherwise, choose the 2nd option.

Once you selected your option, press Enter and it should boot into the game.

Known issues
============

If you own a Mate-X board but also don't have a soundcard attached to it, 
PMDPCM will refuse to load the samples into memory even though PMD has been inited.

As a result, the game will end up with no sound even though you own a capable soundcard
capable of at least PCM playback.

Boot on HDD
===========

If you don't want to boot this from the floppy drive,
you can also copy the files to an HDD (there is no copy protection).

You can simply run "AUTOEXEC.BAT" to run the game as you would from a floppy.
Be careful however : the game requires plenty of conventional memory and expanded memory !

Changelog
=========

1.0 :
- Initial version with 24K soundboard support (unlike the initial PC-9801 version)
