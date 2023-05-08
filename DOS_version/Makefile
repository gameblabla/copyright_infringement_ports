CC = wcl
MODE = c

# -DTHIRTYTWOBITS
CFLAGS = -0 -bt=dos -bcl=dos -m${MODE} -I./src/generic -I./src/graphics -I./src -I./src/graphics/modex -I./src/input -Isrc/wave -Isrc/music -ze -zu

LDFLAGS = 
OUTPUT = GAME.exe

SOURCES =  src/main.c src/generic/generic.c src/graphics/cga/cga.c src/graphics/graph.c src/input/keyboard.c \
src/graphics/ega/ega.c src/graphics/hgc/hgc.c src/graphics/tandy/tandy.c ./src/wave/sbdma/sbdma.c ./src/wave/tandy/tandypsg.c  ./src/wave/pck/pck.c ./src/wave/adlib/adlibpcm.c \
./src/music/adlib/music_adlib.c src/graphics/mcga/mcga.c src/music/null/music_null.c src/wave/null/nullwav.c

OBJS = ./src/main.c ./src/generic/*.c ./src/graphics/cga/*.c ./src/graphics/*.c ./src/input/*.c ./src/graphics/ega/*.c ./src/music/null/*.c \
./src/graphics/hgc/*.c ./src/graphics/tandy/*.c ./src/wave/sbdma/*.c ./src/wave/tandy/*.c ./src/wave/pck/*.c ./src/wave/null/*.c  ./src/wave/adlib/*.c ./src/music/adlib/*.c src/graphics/mcga/*.c


all: asm ${OUTPUT}

#	wasm src/graphics/8514/CALLAFI.ASM 

asm:
	nasm -f obj decomp/lzsa1.asm -o lzsa1.o
	
${OUTPUT}: ${SOURCES}
	${CC} ${CFLAGS} ${LDFLAGS} -fe=${OUTPUT} ${OBJS} lzsa1.o
	
clean:
	rm *.o src/*.o $(OUTPUT)
