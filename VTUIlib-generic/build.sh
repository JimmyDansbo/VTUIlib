#!/bin/bash
rm -rf *.PRG *.BIN
acme -f cbm -o VTUI0.1.BIN vtuilib-generic.asm
acme -f cbm -o EXAMPL01.PRG example01.asm
acme -f cbm -o EXAMPL02.PRG example02.asm

