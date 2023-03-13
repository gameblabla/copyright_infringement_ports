Copyright Infringement  ports
==========================================
**WARNING** : Don't play kids. Don't play it at work either.

This is a repo for all of the versions of Copyright Infringement : The regressive right,
a game where Rikuto has trapped you in one of his games and paired you with another mate.
It is a very short game that was initially made for the Colecovision to demonstrate that it is possible
to achieve FMVs on the Colecovision through the use of MegaROM bank switcher.

Later, another unrelated game for the TI-99/4a (Dragon's Lair) took it a step further
and did full motion video with PCM sound at 13khz and on a gigantic 512MB rom !

CBS Colecovision
=================

This was the initial port of the game.
No speech, the image quality is decent and it wasn't too hard once i figured out that i could write to the VDP memory directly.
I tried to post the game on AtariAge and it did bring up a good discussion about FMVs and taking it further before it was taken down.

This is so far the only port to use sprites for the HUD as the other ones so far don't use it.

MSX2
====

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


ZX Spectrum 48k
===============

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
