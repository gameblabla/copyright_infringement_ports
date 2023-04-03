#!/bin/sh
rm hello.prg
mos-c64-clang -Oz -fomit-frame-pointer -flto -o main.prg main.c disp.asm -s
