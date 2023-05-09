==========================
COPYRIGHT INFRINGEMENT DOS
==========================

This is the DOS version of Copyright Infringement : The Regressive Right.
This has both the 8086/8088 Real mode executable as well as the 386 Protected mode executable.
The 386 version has more frames whereas the real mode only has a few frames (memory consumption issues).

Rikuto has decided to inflict a punishment on you and it's up to you to escape his "game".
Screw over your mate and score as many points as you can !

==========================
Supported devices
==========================

Video modes
- CGA 4 colors RGB
- CGA Composite
- CGA B&W (with Inverted mode for LCD DOS portables)
- EGA 320x200 16 colors
- Hercules
- MCGA/VGA 320x200 256 Colors (Mode 13h)
- Tandy/PCJr 160x200 and 320x200 16 colors mode

These video modes are also available for the 386+ version
- Hercules In-Color
- VGA 800x600 16 Colors
- VGA 800x600 16 Colors Special/SVGA variant (May be slower!)
- VGA 360x480 ModeX 256 Colors
- VGA 320x240 ModeX 256 Colors
- VGA 640x400 ModeX 256 Colors Special/SVGA variant (Has poor compatibility)

Music
For music, only Adlib OPL2 is available. 
The OPL2LPT should work but this was untested and the autodetection may fail.

Sound effects/PCM Voices
- SoundBlaster (DMA, Recommended. SB 1.0 may work but it's recommended you have a SB 2.0+)
- Adlib OPL2 (Has much higher CPU requirements)
- PC Speaker (Lower CPU requirements than adlib but poor quality)
- Tandy PSG (Does not support the parallel version out of the box. Use this for PCJr or Tandy)

Joysticks are also supported at port 0x201 (which is the default).

==========================
Minimum Requirements
==========================

RealMode version (8086/8088)

CPU : 286 8Mhz or Turbo XT 10 Mhz
Graphics card : CGA
Sound : None or Sound Blaster

Using CGA will be the fastest (any of the CGA options should do).
The second fastest option would be Hercules (Monochrome) but it's twice as slow as CGA.
A 4.77mhz IBM AT could run the game but you have to disable sound or use a soundblaster (without music) for it to be playable.

386 Enhanced version
CPU : 386SX 16 Mhz
Graphics card : CGA/VGA/Hercules
Sound : PC Speaker or SoundBlaster

==========================
Recommended Requirements
==========================

RealMode version (8086/8088)

CPU : 286 12~16 Mhz
Graphics card : CGA/Hercules/VGA
Sound : Sound Blaster

386 Enhanced version
CPU : 386DX 40Mhz, 486DX 66Mhz for the higher resolution modes (800x600, 640x400 etc...)
Graphics card : CGA/VGA/Hercules
Sound : Any sound option available


=================
Changelog
=================

1.22
- Added MIDI support (including for OPL2/3 and also General MIDI)
- Make the menu more robust (don't allow PC Speaker to be selected if MIDI/OPL is selected for instance)

1.21
- Fix keyboard issue with real mode version
- Fix issue with menu selection going crazy if you input a letter instead of a number
- Slightly reduced memory consumption
- Fix VGA 800x600 going too fast, turns out the removed vsync waits were not required at all as long as hardware can keep up (which Dosbox does not)

1.2
- Added 160x200 PCJr/Tandy graphics mode (in addition to the already existing 320x200)
- Reduced executable filesize and reduced memory consumption.
- Added joystick/gameport support.

1.1
Initial public release
