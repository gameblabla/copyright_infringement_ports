# Set the compiler
CC=arm-none-eabi-gcc
OC=arm-none-eabi-objcopy
FIX=gbafix

# Set the flags
CFLAGS=-Os -fomit-frame-pointer -fdata-sections -ffunction-sections -mcpu=arm7tdmi -nostartfiles 
CFLAGS+= -mthumb -mno-tpcs-frame -mno-thumb-interwork

LDFLAGS = -Wl,--as-needed -Wl,--gc-sections -s

# Set the source and output files
SOURCES=crt0.s main.c gbalib.c frames16.c
OUTFILE=game.out
BINFILE=game.gba

# Set the linker script
LNKSCRIPT=lnkscript

all: $(BINFILE)

$(BINFILE): $(OUTFILE)
	$(OC) -O binary $< $@
	$(FIX) $@
	
$(OUTFILE): $(SOURCES)
	$(CC) $^ -T$(LNKSCRIPT) $(CFLAGS) -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(OUTFILE) $(BINFILE) *.raw *.o
