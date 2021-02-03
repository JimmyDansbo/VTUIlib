#!/bin/bash
rm -rf *.PRG *.BIN
acme -f cbm -o VTUI.BIN vtuilib-generic.inc
acme -f cbm -o VTUITEST.PRG vtuitest.asm

