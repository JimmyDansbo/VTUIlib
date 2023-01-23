#!/bin/bash
rm -rf *.o *.PRG *.BIN
acme -f cbm -o VTUI1.0.BIN -l vtuilib-generic.lst vtuilib-generic.asm
acme -f cbm -o EXAMPL01.PRG example01.asm
acme -f cbm -o EXAMPL02.PRG example02.asm
acme -f cbm -o ACME-EX1.PRG acme-ex01.asm
acme -f cbm -o ACME-EX2.PRG acme-ex02.asm
cl65 -t cx16 -o CA65-EX1.PRG ca65-ex01.asm
ca65 -t cx16 -o vtuilib-cc65.o vtuilib-cc65.asm
#ar65 a vtui-cc65.lib vtuilib-cc65.o
cl65 -t cx16 -o CC65-EX01.PRG cc65-ex01.c vtuilib-cc65.o
