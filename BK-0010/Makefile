CC = pdp11-aout-gcc
OBJCOPY = pdp11-aout-objcopy
OPTS = -O0 -ftree-vectorize -fvect-cost-model=very-cheap -fomit-frame-pointer -ffast-math -mno-int32 -fno-stack-protector -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm
OPTS += -fauto-inc-dec -fbranch-count-reg -fcombine-stack-adjustments -fcompare-elim -fcprop-registers -fdce -fdefer-pop  -fdse -fguess-branch-probability -fif-conversion2 -fif-conversion -fipa-pure-const -fipa-profile -fipa-reference -fmerge-constants -fmove-loop-invariants -fomit-frame-pointer -freorder-blocks -fshrink-wrap -fsplit-wide-types -fssa-backprop -fssa-phiopt -ftree-bit-ccp -ftree-ccp -ftree-ch -ftree-copy-prop -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-pta -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra -ftree-ter
OPTS += -fallow-store-data-races -fcompare-elim
CFLAGS = -nostdlib -Ttext 0x200 -m10 $(OPTS) -ffreestanding -N -e _start
SRC = crt0.s main.c font.c dzx1.s BAKURA_FACE.c BKFRAME1_CUT.BKS.c BKFRAME3_CUT.BKS.c BKFRAME4_CUT.BKS.c BKFRAME5_CUT.BKS.c
OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:.s=.o)
BIN2BIN = ./bkbin2bin.elf
BINCONVERT_SRC = bkbin2bin.c

all: clean main.bin

main.bin: main
	pdp11-aout-strip --strip-all main
	$(OBJCOPY) -O binary main temp.bin
	gcc $(BINCONVERT_SRC) -o $(BIN2BIN)
	$(BIN2BIN) temp.bin main.bin
	rm temp.bin

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main -s

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main.bin *.o
	
run:
	./bk.elf -gmain.bin
