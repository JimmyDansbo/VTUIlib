#!/bin/bash
rm -rf *.PRG *.BIN
acme -f cbm -o VTUI0.1.BIN -l vtui0.1.sym vtuilib-generic.inc
acme -f cbm -o EXAMPLE.PRG -l example.sym example.asm

