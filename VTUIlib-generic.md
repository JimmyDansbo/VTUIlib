# VTUI library Programmers Reference

Version 0.1

*Author: Jimmy Dansbo*

**This is preliminary documentation and can change at any point.**

This document describes the **generic** **V**ERA **T**ext **U**ser **I**nterface library.

## Overview

The VTUI library is meant to provide a set of functions for creating Text User Interfaces using
VERA in the Commander X16 computer. The library is split up into 3 flavors.
* Generic (the one documented here)
* ACME include file
* CA65 include file

The generic library is meant to be compiled into a binary by it self and loaded by the users
program. The choice of compiler/assembler is entirely up to the user as long as it is possible
to store values to zeropage and in registers before calling subroutines.

The other two flavors are include files for their respective assemblers and will be documented
separately.

## Loading

The generic VTUI library is designed to be loaded by standard CBM kernal functions [SETLFS](https://cx16.dk/c64-kernal-routines/setlfs.html), [SETNAM](https://cx16.dk/c64-kernal-routines/setnam.html) and [LOAD](https://cx16.dk/c64-kernal-routines/load.html).

In several assemblers it is possible to load a a binary file directly with the sourcecode. for ACME it is done something like this `VTUI !BIN "VTUI.BIN"` and for CA65 it would be done like this `VTUI .INCBIN "VTUI.BIN"`.

If an assembler is used to include the binary file, be aware that the first two bytes are a loading address so base address of the actual library will be: `VTUILIB=VTUI+2`.

When using the CBM kernal functions to load the library, the LOAD command will remove the first two bytes before writing the library to memory.

When the library is loaded and VTUILIB is pointing to the memory address where the library starts (load address + 2 if loaded by assembler, otherwise just load address) it needs to be initalized by calling the initialization subroutine at VTUILIB address. All functions of the library are called by reference to the base VTUILIB address.

## Initialization

As the generic VTUI library is built without knowledge of where in memory it will be loaded, it is
necessary to initialize the library before use. The initialization ensures that the jumptable at the beginning of the library is updated to point to the correct address where functions are loaded.

After initialization, all functions can be called by referencing the base address of the library `VTUILIB`

## Registers

Several zeropage addresses are used by the library for temporary storage space as well as parameter passing. Addresses used are `$22 - $2D` this is to avoid using the ABI registers used by the new Commander X16 functions.
The ABI registers are named r0, r0l, r0h, r1 and so on. They start at address $02 and go all the way to $21. The debugger in the emulator displays registers 16bit registers x16, x17, x18 & x19. These are the registers mostly used by the VTUI library, but in some cases more space is needed and an additional 4 bytes of zerospace is used totalling 12 bytes of zeropage space used by the library.

The VTUI library only uses the zeropage addresses inside it's own function or as paramater passing so this space can be used for anything else as long as it is made available to the functions as they are called.

In addition to the zeropage memory, the VTUI library uses CPU registers for transferring arguments to the functions as well as temporary space and indexing.

## Functions
