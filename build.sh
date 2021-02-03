#!/bin/bash
rm -rf *.PRG *.BIN
acme -f cbm -o VTUI.BIN -l vtui.sym vtuilib-generic.inc
acme -f cbm -o VTUITEST.PRG -l vtuitest.sym vtuitest.asm

