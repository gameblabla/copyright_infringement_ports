;* * * * *  Small-C/Plus z88dk * * * * *
;  Version: 20671-7a54d1010-20230312
;
;	Reconstructed for z80 Module Assembler
;
;	Module compile time: Fri Mar 24 13:13:43 2023


	C_LINE	0,"main.c"

	MODULE	main_c


	INCLUDE "z80_crt0.hdr"


	EXTERN	saved_hl
	EXTERN	saved_de
	C_LINE	0,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	0,"/usr/local/share/z88dk//include/sys/compiler.h"
	C_LINE	4,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	0,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	0,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	17,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	26,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	35,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	40,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	45,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	50,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	55,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	60,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	65,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	70,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	75,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	80,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	85,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	89,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	90,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	91,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	93,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	94,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	95,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	101,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	106,"/usr/local/share/z88dk//include/sys/types.h"
	C_LINE	11,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	13,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	14,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	15,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	17,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	18,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	19,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	21,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	22,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	23,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	25,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	26,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	27,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	29,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	30,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	31,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	33,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	34,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	35,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	37,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	38,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	40,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	41,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	43,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	44,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	49,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	52,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	54,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	55,"/usr/local/share/z88dk//include/stdint.h"
	C_LINE	5,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	0,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	34,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	37,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	38,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	39,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	40,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	41,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	42,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	44,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	45,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	52,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	53,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	54,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	57,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	76,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	85,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	98,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	99,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	100,"/usr/local/share/z88dk//include/fcntl.h"
	C_LINE	78,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	91,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	141,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	160,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	165,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	166,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	175,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	178,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	182,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	183,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	184,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	185,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	186,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	187,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	191,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	192,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	194,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	225,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	227,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	228,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	230,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	231,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	232,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	235,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	236,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	237,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	238,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	253,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	254,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	257,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	260,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	261,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	268,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	271,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	272,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	273,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	274,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	275,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	276,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	282,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	289,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	290,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	291,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	292,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	293,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	303,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	308,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	313,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	316,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	319,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	322,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	324,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	327,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	329,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	330,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	332,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	334,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	338,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	340,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	344,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	347,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	356,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	358,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	361,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	363,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	365,"/usr/local/share/z88dk//include/stdio.h"
	C_LINE	1,"main.c"
	C_LINE	0,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	23,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	35,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	39,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	40,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	44,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	45,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	49,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	50,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	55,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	56,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	60,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	61,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	66,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	73,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	80,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	87,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	94,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	101,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	108,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	115,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	122,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	130,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	133,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	136,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	139,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	140,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	144,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	145,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	148,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	149,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	153,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	154,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	158,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	159,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	163,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	164,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	169,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	176,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	183,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	190,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	194,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	195,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	199,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	200,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	204,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	205,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	211,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	212,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	213,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	216,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	217,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	224,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	235,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	237,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	239,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	241,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	243,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	245,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	247,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	251,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	265,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	267,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	269,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	271,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	277,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	280,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	281,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	285,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	286,"/usr/local/share/z88dk//include/graphics.h"
	C_LINE	2,"main.c"
	C_LINE	0,"/usr/local/share/z88dk//include/compress/aplib.h"
	C_LINE	24,"/usr/local/share/z88dk//include/compress/aplib.h"
	C_LINE	25,"/usr/local/share/z88dk//include/compress/aplib.h"
	C_LINE	3,"main.c"
	C_LINE	0,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	58,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	59,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	63,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	64,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	68,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	69,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	73,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	74,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	78,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	79,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	83,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	84,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	89,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	90,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	94,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	95,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	99,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	100,"/usr/local/share/z88dk//include/compress/zx0.h"
	C_LINE	4,"main.c"
	C_LINE	0,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	58,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	59,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	63,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	64,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	68,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	69,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	73,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	74,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	78,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	79,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	83,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	84,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	89,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	90,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	94,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	95,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	99,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	100,"/usr/local/share/z88dk//include/compress/zx1.h"
	C_LINE	5,"main.c"
	C_LINE	0,"/usr/local/share/z88dk//include/string.h"
	C_LINE	7,"/usr/local/share/z88dk//include/string.h"
	C_LINE	8,"/usr/local/share/z88dk//include/string.h"
	C_LINE	12,"/usr/local/share/z88dk//include/string.h"
	C_LINE	14,"/usr/local/share/z88dk//include/string.h"
	C_LINE	19,"/usr/local/share/z88dk//include/string.h"
	C_LINE	20,"/usr/local/share/z88dk//include/string.h"
	C_LINE	24,"/usr/local/share/z88dk//include/string.h"
	C_LINE	25,"/usr/local/share/z88dk//include/string.h"
	C_LINE	29,"/usr/local/share/z88dk//include/string.h"
	C_LINE	30,"/usr/local/share/z88dk//include/string.h"
	C_LINE	34,"/usr/local/share/z88dk//include/string.h"
	C_LINE	35,"/usr/local/share/z88dk//include/string.h"
	C_LINE	39,"/usr/local/share/z88dk//include/string.h"
	C_LINE	41,"/usr/local/share/z88dk//include/string.h"
	C_LINE	46,"/usr/local/share/z88dk//include/string.h"
	C_LINE	47,"/usr/local/share/z88dk//include/string.h"
	C_LINE	52,"/usr/local/share/z88dk//include/string.h"
	C_LINE	53,"/usr/local/share/z88dk//include/string.h"
	C_LINE	57,"/usr/local/share/z88dk//include/string.h"
	C_LINE	59,"/usr/local/share/z88dk//include/string.h"
	C_LINE	64,"/usr/local/share/z88dk//include/string.h"
	C_LINE	65,"/usr/local/share/z88dk//include/string.h"
	C_LINE	69,"/usr/local/share/z88dk//include/string.h"
	C_LINE	72,"/usr/local/share/z88dk//include/string.h"
	C_LINE	75,"/usr/local/share/z88dk//include/string.h"
	C_LINE	78,"/usr/local/share/z88dk//include/string.h"
	C_LINE	79,"/usr/local/share/z88dk//include/string.h"
	C_LINE	83,"/usr/local/share/z88dk//include/string.h"
	C_LINE	85,"/usr/local/share/z88dk//include/string.h"
	C_LINE	90,"/usr/local/share/z88dk//include/string.h"
	C_LINE	92,"/usr/local/share/z88dk//include/string.h"
	C_LINE	97,"/usr/local/share/z88dk//include/string.h"
	C_LINE	99,"/usr/local/share/z88dk//include/string.h"
	C_LINE	109,"/usr/local/share/z88dk//include/string.h"
	C_LINE	111,"/usr/local/share/z88dk//include/string.h"
	C_LINE	116,"/usr/local/share/z88dk//include/string.h"
	C_LINE	118,"/usr/local/share/z88dk//include/string.h"
	C_LINE	123,"/usr/local/share/z88dk//include/string.h"
	C_LINE	125,"/usr/local/share/z88dk//include/string.h"
	C_LINE	133,"/usr/local/share/z88dk//include/string.h"
	C_LINE	135,"/usr/local/share/z88dk//include/string.h"
	C_LINE	139,"/usr/local/share/z88dk//include/string.h"
	C_LINE	141,"/usr/local/share/z88dk//include/string.h"
	C_LINE	145,"/usr/local/share/z88dk//include/string.h"
	C_LINE	146,"/usr/local/share/z88dk//include/string.h"
	C_LINE	150,"/usr/local/share/z88dk//include/string.h"
	C_LINE	152,"/usr/local/share/z88dk//include/string.h"
	C_LINE	157,"/usr/local/share/z88dk//include/string.h"
	C_LINE	158,"/usr/local/share/z88dk//include/string.h"
	C_LINE	162,"/usr/local/share/z88dk//include/string.h"
	C_LINE	163,"/usr/local/share/z88dk//include/string.h"
	C_LINE	167,"/usr/local/share/z88dk//include/string.h"
	C_LINE	168,"/usr/local/share/z88dk//include/string.h"
	C_LINE	172,"/usr/local/share/z88dk//include/string.h"
	C_LINE	173,"/usr/local/share/z88dk//include/string.h"
	C_LINE	177,"/usr/local/share/z88dk//include/string.h"
	C_LINE	178,"/usr/local/share/z88dk//include/string.h"
	C_LINE	182,"/usr/local/share/z88dk//include/string.h"
	C_LINE	183,"/usr/local/share/z88dk//include/string.h"
	C_LINE	187,"/usr/local/share/z88dk//include/string.h"
	C_LINE	188,"/usr/local/share/z88dk//include/string.h"
	C_LINE	192,"/usr/local/share/z88dk//include/string.h"
	C_LINE	193,"/usr/local/share/z88dk//include/string.h"
	C_LINE	197,"/usr/local/share/z88dk//include/string.h"
	C_LINE	200,"/usr/local/share/z88dk//include/string.h"
	C_LINE	203,"/usr/local/share/z88dk//include/string.h"
	C_LINE	204,"/usr/local/share/z88dk//include/string.h"
	C_LINE	208,"/usr/local/share/z88dk//include/string.h"
	C_LINE	210,"/usr/local/share/z88dk//include/string.h"
	C_LINE	215,"/usr/local/share/z88dk//include/string.h"
	C_LINE	217,"/usr/local/share/z88dk//include/string.h"
	C_LINE	222,"/usr/local/share/z88dk//include/string.h"
	C_LINE	225,"/usr/local/share/z88dk//include/string.h"
	C_LINE	228,"/usr/local/share/z88dk//include/string.h"
	C_LINE	230,"/usr/local/share/z88dk//include/string.h"
	C_LINE	235,"/usr/local/share/z88dk//include/string.h"
	C_LINE	237,"/usr/local/share/z88dk//include/string.h"
	C_LINE	242,"/usr/local/share/z88dk//include/string.h"
	C_LINE	244,"/usr/local/share/z88dk//include/string.h"
	C_LINE	249,"/usr/local/share/z88dk//include/string.h"
	C_LINE	251,"/usr/local/share/z88dk//include/string.h"
	C_LINE	256,"/usr/local/share/z88dk//include/string.h"
	C_LINE	258,"/usr/local/share/z88dk//include/string.h"
	C_LINE	263,"/usr/local/share/z88dk//include/string.h"
	C_LINE	264,"/usr/local/share/z88dk//include/string.h"
	C_LINE	268,"/usr/local/share/z88dk//include/string.h"
	C_LINE	269,"/usr/local/share/z88dk//include/string.h"
	C_LINE	273,"/usr/local/share/z88dk//include/string.h"
	C_LINE	274,"/usr/local/share/z88dk//include/string.h"
	C_LINE	278,"/usr/local/share/z88dk//include/string.h"
	C_LINE	279,"/usr/local/share/z88dk//include/string.h"
	C_LINE	283,"/usr/local/share/z88dk//include/string.h"
	C_LINE	284,"/usr/local/share/z88dk//include/string.h"
	C_LINE	288,"/usr/local/share/z88dk//include/string.h"
	C_LINE	289,"/usr/local/share/z88dk//include/string.h"
	C_LINE	293,"/usr/local/share/z88dk//include/string.h"
	C_LINE	296,"/usr/local/share/z88dk//include/string.h"
	C_LINE	297,"/usr/local/share/z88dk//include/string.h"
	C_LINE	301,"/usr/local/share/z88dk//include/string.h"
	C_LINE	304,"/usr/local/share/z88dk//include/string.h"
	C_LINE	305,"/usr/local/share/z88dk//include/string.h"
	C_LINE	309,"/usr/local/share/z88dk//include/string.h"
	C_LINE	310,"/usr/local/share/z88dk//include/string.h"
	C_LINE	314,"/usr/local/share/z88dk//include/string.h"
	C_LINE	315,"/usr/local/share/z88dk//include/string.h"
	C_LINE	319,"/usr/local/share/z88dk//include/string.h"
	C_LINE	322,"/usr/local/share/z88dk//include/string.h"
	C_LINE	323,"/usr/local/share/z88dk//include/string.h"
	C_LINE	327,"/usr/local/share/z88dk//include/string.h"
	C_LINE	329,"/usr/local/share/z88dk//include/string.h"
	C_LINE	334,"/usr/local/share/z88dk//include/string.h"
	C_LINE	337,"/usr/local/share/z88dk//include/string.h"
	C_LINE	339,"/usr/local/share/z88dk//include/string.h"
	C_LINE	343,"/usr/local/share/z88dk//include/string.h"
	C_LINE	344,"/usr/local/share/z88dk//include/string.h"
	C_LINE	348,"/usr/local/share/z88dk//include/string.h"
	C_LINE	349,"/usr/local/share/z88dk//include/string.h"
	C_LINE	352,"/usr/local/share/z88dk//include/string.h"
	C_LINE	353,"/usr/local/share/z88dk//include/string.h"
	C_LINE	6,"main.c"
	C_LINE	9,"main.c"
	C_LINE	21,"main.c"
	SECTION	code_compiler

; Function decompress_slz flags 0x00000200 __smallc 
; void decompress_slz(const unsigned char uint8_t* in, unsigned char uint8_t* out)
; parameter 'unsigned char uint8_t* out' at sp+2 size(2)
; parameter 'const unsigned char uint8_t* in' at sp+4 size(2)
	C_LINE	22,"main.c::decompress_slz::0::0"
._decompress_slz
	call	l_gint4sp	;
	ld	h,(hl)
	ld	l,0
	push	hl
	call	l_gint6sp	;
	inc	hl
	ld	l,(hl)
	pop	de
	ld	h,d
	ld	a,l
	or	e
	ld	l,a
	push	hl
	ld	hl,6	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	inc	hl
	pop	de
	call	l_pint
	dec	sp
	pop	hl
	ld	l,1
	push	hl
	dec	sp
.i_2
	pop	bc
	pop	hl
	push	hl
	push	bc
	ld	a,h
	or	l
	jp	z,ASMPC+4
	scf
	jp	nc,i_3	;
	ld	hl,1	;const
	add	hl,sp
	dec	(hl)
	pop	hl
	push	hl
	ld	a,h
	and	a
	jp	nz,i_4	;
	ld	hl,0	;const
	add	hl,sp
	push	hl
	ld	hl,10	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
	ld	hl,1	;const
	add	hl,sp
	ld	(hl),8
.i_4
	pop	hl
	push	hl
	ld	h,0
	ld	a,l
	and	128
	jp	z,i_5	;
	call	l_gint8sp	;
	ld	h,(hl)
	ld	l,0
	push	hl
	ld	hl,10	;const
	add	hl,sp
	call	l_gint	;
	inc	hl
	ld	l,(hl)
	pop	de
	ld	h,d
	ld	a,l
	or	e
	ld	l,a
	push	hl
	ld	a,l
	and	15
	ld	l,a
	ld	h,0
	dec	sp
	ld	a,l
	pop	hl
	ld	l,a
	push	hl
	ld	hl,1	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	ld	de,4
	call	l_asr_u_hl_by_e
	pop	de
	call	l_pint
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	inc	hl
	pop	de
	call	l_pint
	ld	hl,5	;const
	add	hl,sp
	push	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	de
	ld	hl,4	;const
	add	hl,sp
	ld	l,(hl)
	ld	h,0
	inc	hl
	inc	hl
	inc	hl
	pop	de
	call	l_sub
	pop	de
	call	l_pint
	ld	hl,9	;const
	call	l_gintspsp	;
	call	l_gint3sp	;
	pop	de
	call	l_sub
	dec	hl
	dec	hl
	dec	hl
	push	hl
	ld	hl,2	;const
	add	hl,sp
	ld	l,(hl)
	ld	h,0
.i_8
	ld	a,l
	cp	+(15% 256)
	jp	z,i_9	;
	cp	+(14% 256)
	jp	z,i_10	;
	cp	+(13% 256)
	jp	z,i_11	;
	cp	+(12% 256)
	jp	z,i_12	;
	cp	+(11% 256)
	jp	z,i_13	;
	cp	+(10% 256)
	jp	z,i_14	;
	cp	+(9% 256)
	jp	z,i_15	;
	cp	+(8% 256)
	jp	z,i_16	;
	cp	+(7% 256)
	jp	z,i_17	;
	cp	+(6% 256)
	jp	z,i_18	;
	cp	+(5% 256)
	jp	z,i_19	;
	cp	+(4% 256)
	jp	z,i_20	;
	cp	+(3% 256)
	jp	z,i_21	;
	cp	+(2% 256)
	jp	z,i_22	;
	cp	+(1% 256)
	jp	z,i_23	;
	and	a
	jp	z,i_24	;
	jp	i_7	;EOS
.i_9
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_10
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_11
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_12
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_13
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_14
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_15
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_16
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_17
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_18
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_19
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_20
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_21
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_22
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_23
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
.i_24
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
	ld	hl,11	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	d,h
	ld	e,l
	pop	hl
	inc	hl
	push	hl
	push	de
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
	ld	l,a
	ld	h,0
.i_7
	inc	sp
	pop	bc
	pop	bc
	jp	i_25	;EOS
.i_5
	ld	hl,6	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	push	hl
	ld	hl,10	;const
	add	hl,sp
	push	hl
	call	l_gint	;
	inc	hl
	pop	de
	call	l_pint
	dec	hl
	ld	a,(hl)
	pop	de
	ld	(de),a
	pop	de
	pop	hl
	dec	hl
	push	hl
	push	de
	inc	hl
.i_25
	ld	hl,0	;const
	add	hl,sp
	push	hl
	ld	e,(hl)
	ld	d,0
	ld	hl,2	;const
	add	hl,sp
	ld	l,(hl)
	ld	h,0
	add	hl,de
	pop	de
	ld	a,l
	ld	(de),a
	jp	i_2	;EOS
.i_3
	pop	bc
	pop	bc
	ret


	C_LINE	88,"main.c::decompress_slz::0::6"

; Function main flags 0x00000000 __stdc 
; int main()
	C_LINE	89,"main.c::main::0::7"
._main
	ld	hl,_img_data
	push	hl
	ld	hl,49152	;const
	push	hl
	call	_decompress_slz
	pop	bc
	pop	bc
.i_26
	jp	i_26	;EOS
.i_27
	ret


; --- Start of Optimiser additions ---


; --- Start of Static Variables ---

	SECTION	bss_compiler
	SECTION	code_compiler


; --- Start of Scope Defns ---

	GLOBAL	open
	GLOBAL	creat
	GLOBAL	close
	GLOBAL	read
	GLOBAL	write
	GLOBAL	lseek
	GLOBAL	readbyte
	GLOBAL	writebyte
	GLOBAL	getcwd
	GLOBAL	chdir
	GLOBAL	getwd
	GLOBAL	rmdir
	GLOBAL	__RND_BLOCKSIZE
	GLOBAL	rnd_loadblock
	GLOBAL	rnd_saveblock
	GLOBAL	rnd_erase
	GLOBAL	__FOPEN_MAX
	GLOBAL	__sgoioblk
	GLOBAL	__sgoioblk_end
	GLOBAL	fopen_zsock
	GLOBAL	fileno
	GLOBAL	fopen
	GLOBAL	freopen
	GLOBAL	fdopen
	GLOBAL	_freopen1
	GLOBAL	fmemopen
	GLOBAL	funopen
	GLOBAL	fclose
	GLOBAL	fflush
	GLOBAL	closeall
	GLOBAL	fgets
	GLOBAL	fputs
	GLOBAL	fputc
	GLOBAL	fputs_callee
	GLOBAL	fputc_callee
	GLOBAL	fgetc
	GLOBAL	ungetc
	GLOBAL	feof
	GLOBAL	ferror
	GLOBAL	puts
	GLOBAL	ftell
	GLOBAL	fgetpos
	GLOBAL	fseek
	GLOBAL	fread
	GLOBAL	fwrite
	GLOBAL	gets
	GLOBAL	printf
	GLOBAL	fprintf
	GLOBAL	sprintf
	GLOBAL	snprintf
	GLOBAL	vfprintf
	GLOBAL	vsnprintf
	GLOBAL	printn
	GLOBAL	scanf
	GLOBAL	fscanf
	GLOBAL	sscanf
	GLOBAL	vfscanf
	GLOBAL	vsscanf
	GLOBAL	getarg
	GLOBAL	fchkstd
	GLOBAL	fgetc_cons
	GLOBAL	fgetc_cons_inkey
	GLOBAL	fputc_cons
	GLOBAL	fgets_cons
	GLOBAL	puts_cons
	GLOBAL	fabandon
	GLOBAL	fdtell
	GLOBAL	fdgetpos
	GLOBAL	rename
	GLOBAL	remove
	GLOBAL	getk
	GLOBAL	getk_inkey
	GLOBAL	printk
	GLOBAL	perror
	GLOBAL	set_fputc_cons
	GLOBAL	fputc_cons_native
	GLOBAL	fputc_cons_generic
	GLOBAL	fputc_cons_ansi
	GLOBAL	fill
	GLOBAL	plot
	GLOBAL	plot_callee
	GLOBAL	unplot
	GLOBAL	unplot_callee
	GLOBAL	xorplot
	GLOBAL	xorplot_callee
	GLOBAL	point
	GLOBAL	point_callee
	GLOBAL	multipoint
	GLOBAL	multipoint_callee
	GLOBAL	draw
	GLOBAL	xordraw
	GLOBAL	undraw
	GLOBAL	drawr
	GLOBAL	xordrawr
	GLOBAL	undrawr
	GLOBAL	drawto
	GLOBAL	xordrawto
	GLOBAL	undrawto
	GLOBAL	pen_up
	GLOBAL	pen_down
	GLOBAL	move
	GLOBAL	setpos
	GLOBAL	setpos_callee
	GLOBAL	getx
	GLOBAL	gety
	GLOBAL	drawb
	GLOBAL	drawb_callee
	GLOBAL	xordrawb
	GLOBAL	xordrawb_callee
	GLOBAL	undrawb
	GLOBAL	undrawb_callee
	GLOBAL	xorborder
	GLOBAL	xorborder_callee
	GLOBAL	circle
	GLOBAL	uncircle
	GLOBAL	xorcircle
	GLOBAL	clg
	GLOBAL	clga
	GLOBAL	clga_callee
	GLOBAL	xorclga
	GLOBAL	xorclga_callee
	GLOBAL	fillb
	GLOBAL	fillb_callee
	GLOBAL	putc4x6
	GLOBAL	_x_4x6
	GLOBAL	_y_4x6
	GLOBAL	getmaxx
	GLOBAL	getmaxy
	GLOBAL	gfx_set_fatpix
	GLOBAL	stencil_init
	GLOBAL	stencil_add_point
	GLOBAL	stencil_add_liner
	GLOBAL	stencil_add_lineto
	GLOBAL	stencil_add_side
	GLOBAL	stencil_add_circle
	GLOBAL	stencil_render
	GLOBAL	draw_profile
	GLOBAL	c_plot
	GLOBAL	c_unplot
	GLOBAL	c_xorplot
	GLOBAL	c_point
	GLOBAL	cclg
	GLOBAL	cplot
	GLOBAL	cplot_callee
	GLOBAL	cpoint
	GLOBAL	cpoint_callee
	GLOBAL	aplib_depack
	GLOBAL	aplib_depack_callee
	GLOBAL	dzx0_standard
	GLOBAL	dzx0_standard_callee
	GLOBAL	dzx0_standard_back
	GLOBAL	dzx0_standard_back_callee
	GLOBAL	dzx0_turbo
	GLOBAL	dzx0_turbo_callee
	GLOBAL	dzx0_turbo_back
	GLOBAL	dzx0_turbo_back_callee
	GLOBAL	dzx0_mega
	GLOBAL	dzx0_mega_callee
	GLOBAL	dzx0_mega_back
	GLOBAL	dzx0_mega_back_callee
	GLOBAL	dzx0_smart_rcs
	GLOBAL	dzx0_smart_rcs_callee
	GLOBAL	dzx0_smart_rcs_back
	GLOBAL	dzx0_smart_rcs_back_callee
	GLOBAL	dzx0_agile_rcs
	GLOBAL	dzx0_agile_rcs_callee
	GLOBAL	dzx1_standard
	GLOBAL	dzx1_standard_callee
	GLOBAL	dzx1_standard_back
	GLOBAL	dzx1_standard_back_callee
	GLOBAL	dzx1_turbo
	GLOBAL	dzx1_turbo_callee
	GLOBAL	dzx1_turbo_back
	GLOBAL	dzx1_turbo_back_callee
	GLOBAL	dzx1_mega
	GLOBAL	dzx1_mega_callee
	GLOBAL	dzx1_mega_back
	GLOBAL	dzx1_mega_back_callee
	GLOBAL	dzx1_smart_rcs
	GLOBAL	dzx1_smart_rcs_callee
	GLOBAL	dzx1_smart_rcs_back
	GLOBAL	dzx1_smart_rcs_back_callee
	GLOBAL	dzx1_agile_rcs
	GLOBAL	dzx1_agile_rcs_callee
	GLOBAL	bcmp
	GLOBAL	bcmp_callee
	GLOBAL	bcopy
	GLOBAL	bcopy_callee
	GLOBAL	bzero
	GLOBAL	bzero_callee
	GLOBAL	index
	GLOBAL	index_callee
	GLOBAL	rindex
	GLOBAL	rindex_callee
	GLOBAL	strset
	GLOBAL	strset_callee
	GLOBAL	strnset
	GLOBAL	strnset_callee
	GLOBAL	rawmemchr
	GLOBAL	rawmemchr_callee
	GLOBAL	_memlwr_
	GLOBAL	_memlwr__callee
	GLOBAL	_memstrcpy_
	GLOBAL	_memstrcpy__callee
	GLOBAL	_memupr_
	GLOBAL	_memupr__callee
	GLOBAL	_strrstrip_
	GLOBAL	ffs
	GLOBAL	ffsl
	GLOBAL	memccpy
	GLOBAL	memccpy_callee
	GLOBAL	memchr
	GLOBAL	memchr_callee
	GLOBAL	memcmp
	GLOBAL	memcmp_callee
	GLOBAL	memcpy
	GLOBAL	memcpy_callee
	GLOBAL	memmove
	GLOBAL	memmove_callee
	GLOBAL	memrchr
	GLOBAL	memrchr_callee
	GLOBAL	memset
	GLOBAL	memset_callee
	GLOBAL	memset_wr
	GLOBAL	memset_wr_callee
	GLOBAL	memswap
	GLOBAL	memswap_callee
	GLOBAL	stpcpy
	GLOBAL	stpcpy_callee
	GLOBAL	stpncpy
	GLOBAL	stpncpy_callee
	GLOBAL	strcasecmp
	GLOBAL	strcasecmp_callee
	GLOBAL	strcat
	GLOBAL	strcat_callee
	GLOBAL	strchr
	GLOBAL	strchr_callee
	GLOBAL	strchrnul
	GLOBAL	strchrnul_callee
	GLOBAL	strcmp
	GLOBAL	strcmp_callee
	GLOBAL	strcoll
	GLOBAL	strcoll_callee
	GLOBAL	strcpy
	GLOBAL	strcpy_callee
	GLOBAL	strcspn
	GLOBAL	strcspn_callee
	GLOBAL	strdup
	GLOBAL	strerror
	GLOBAL	stricmp
	GLOBAL	stricmp_callee
	GLOBAL	strlcat
	GLOBAL	strlcat_callee
	GLOBAL	strlcpy
	GLOBAL	strlcpy_callee
	GLOBAL	strlen
	GLOBAL	strlwr
	GLOBAL	strncasecmp
	GLOBAL	strncasecmp_callee
	GLOBAL	strncat
	GLOBAL	strncat_callee
	GLOBAL	strnchr
	GLOBAL	strnchr_callee
	GLOBAL	strncmp
	GLOBAL	strncmp_callee
	GLOBAL	strncpy
	GLOBAL	strncpy_callee
	GLOBAL	strndup
	GLOBAL	strndup_callee
	GLOBAL	strnicmp
	GLOBAL	strnicmp_callee
	GLOBAL	strnlen
	GLOBAL	strnlen_callee
	GLOBAL	strpbrk
	GLOBAL	strpbrk_callee
	GLOBAL	strrchr
	GLOBAL	strrchr_callee
	GLOBAL	strrcspn
	GLOBAL	strrcspn_callee
	GLOBAL	strrev
	GLOBAL	strrspn
	GLOBAL	strrspn_callee
	GLOBAL	strrstrip
	GLOBAL	strsep
	GLOBAL	strsep_callee
	GLOBAL	strspn
	GLOBAL	strspn_callee
	GLOBAL	strstr
	GLOBAL	strstr_callee
	GLOBAL	strstrip
	GLOBAL	strtok
	GLOBAL	strtok_callee
	GLOBAL	strtok_r
	GLOBAL	strtok_r_callee
	GLOBAL	strupr
	GLOBAL	strxfrm
	GLOBAL	strxfrm_callee
	GLOBAL	strrstr
	GLOBAL	strrstr_callee
	GLOBAL	memopi
	GLOBAL	memopi_callee
	GLOBAL	memopd
	GLOBAL	memopd_callee
	GLOBAL	_img_data
	GLOBAL	_decompress_slz
	GLOBAL	_main


; --- End of Scope Defns ---


; --- End of Compilation ---
