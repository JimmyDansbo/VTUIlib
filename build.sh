#!/bin/bash
rm -rf *.PRG *.BIN
acme -f cbm -o VTUITEST.PRG vtuitest.asm
acme -f cbm -o VTUI.BIN vtuilib-generic.inc

