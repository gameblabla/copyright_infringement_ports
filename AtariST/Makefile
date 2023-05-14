CC=m68k-atari-mint-gcc
AS=m68k-atari-mint-gcc
AR=m68k-atari-mint-ar
RANLIB=m68k-atari-mint-ranlib

CFLAGS=-Isrc -m68000 -O2 -fomit-frame-pointer -Wextra -Wall -std=c99 -nostdlib
ASFLAGS=-m68000
ARFLAGS=cr
LDFLAGS= -Wl,--as-needed -s
TARGET=main.prg
ASM_SRC = ikbd.S jagpad.S atari_dma_sound.S lz4w.S
C_SRC = main.c keys.c psg.c dma_atari.c psgpcm.c atarist.c fonts.c

all: $(TARGET)

%.S.o: %.S
	$(AS) $(ASFLAGS) -o $@ -c $<

%.c.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<


$(TARGET): startup.S $(C_SRC:.c=.c.o) $(ASM_SRC:.S=.S.o)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) $(LDFLAGS)

clean:
	rm -f *.o *.d $(LIBS) $(TARGET)
