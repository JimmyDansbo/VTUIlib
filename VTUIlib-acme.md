# VTUI library Programmers Reference

Version 0.1

*Author: Jimmy Dansbo*

**This is preliminary documentation and can change at any point.**

This document describes the **Acme** **V**ERA **T**ext **U**ser **I**nterface library.

**Table of Contents**

* [Overview](#overview)
* [Compatibility](#compatibility)
* [Registers](#registers)
* [Functions](#functions)
	* [screen_set](#function-name-screen_set)
	* [clear](#function-name-clear)
	* [set_stride](#function-name-set_stride)
	* [set_decr](#function-name-set_decr)
	* [gotoxy](#function-name-gotoxy)
	* [plot_char](#function-name-plot_char)
	* [scan_char](#function-name-scan_char)
	* [hline](#function-name-hline)
	* [vline](#function-name-vline)
	* [print_str](#function-name-print_str)
	* [fill_box](#function-name-fill_box)
	* [pet2scr](#function-name-pet2scr)
	* [scr2pet](#function-name-scr2pet)
	* [border](#function-name-border)
	* [save_rect](#function-name-save_rect)
	* [rest_rect](#function-name-rest_rect)

## Overview

The VTUI library is meant to provide a set of functions for creating Text User Interfaces using
VERA in the Commander X16 computer. The library is split up into 3 flavors.
* Generic
* ACME include file (the one documented here)
* CA65 include file

The Acme library is meant to be included into the source by the !source or !src pseudo opcode.
The library consists solely of macros and constants for VERA addresses and ZP variables.

For examples, please visit [VTUIlib Acme](VTUIlib-acme/)

The other two flavors are the generic which is a binary file that is loaded separately and the CA65 which is an include file for the CA65 assembler.

## Compatibility

All macros can be called without arguments in which case they will function exactly like the functions in the generic library. See the [VTUI generic](VTUIlib-generic.md) library documentation.

To call functions in the exact same way as with the generic library, each macro should be called from a subroutine like this:<br>
	vtui_gotoxy:
		+VTUI_GOTOXY
		rts

These functions are not included in the Acme library to ensure that the library does not take up any unnecessary space.

## Registers

Several zeropage addresses are used by the library for temporary storage space as well as parameter passing. Addresses used are `$22 - $2D` this is to avoid using the ABI registers used by the new Commander X16 functions.
The ABI registers are named r0, r0l, r0h, r1 and so on. They start at address $02 and go all the way to $21. The debugger in the emulator displays 16bit registers x16, x17, x18 & x19 which start from `$22`. These are the registers mostly used by the VTUI library, but in some cases more space is needed and an additional 4 bytes of zerospace is used totalling 12 bytes of zeropage space used by the library.

The VTUI library only uses the zeropage addresses inside it's own macros or as parameter passing so this space can be used for anything else as long as it is made available to the macros as they are called.

In addition to the zeropage memory, the VTUI library uses CPU registers for transferring arguments to the functions as well as temporary space and indexing.

## Functions

### Function name: screen_set
Purpose: Set the screen mode to supported text mode<br>
Call address: `VTUILIB+2`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: .A, .X & .Y<br>
ZP registers affected: none<br>

**Description** This function sets or toggles the screenmode. Supported modes are 0 = 40x30 & 2 80x60. Mode 255 ($FF) will toggle between the two modes. Any other mode will fail silently.

## Function name: clear
Purpose: Clear screen with specific background-/foreground-color<br>
Call address: `VTUILIB+5`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: .X & .Y<br>
ZP registers affected: none<br>

**Description** Clear the screen with specific background-/foreground-color in .A. high-nibble is the background color $0-$F, low-nibble is the foreground color $0-$F. The routine fills the screen with spaces (character $20) and sets each characters color.

## Function name: set_stride
Purpose: Set the VERA stride value<br>
Call address: `VTUILIB+8`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: .X<br>
ZP registers affected: none<br>

**Description** Set the VERA stride value. Stride is the amount the VERA address is incremented or decremented on each access. Stride is a 4 bit value and the routine will ensure that the number is converted to fit in VERA_ADDR_H. For more information about VERA stride, see the [VERA Documentation](https://github.com/commanderx16/x16-docs/blob/master/VERA%20Programmer's%20Reference.md#video-ram-access) about 'Address Increment'

## Function name: set_decr
Purpose: Set the VERA decrement bit<br>
Call address: `VTUILIB+11`<br>
Communication registers: .C<br>
Preparatory routines: none<br>
Registers affected: .A
ZP registers affected: none<br>

**Description** Set the VERA decrement bit. The decrement bit decides if the stride value is added to- or subtracted from the current VERA address. Carry Clear (.C=0) means increment by stride value. Carry Set (.C=1) means decrement by stride value.

## Function name: gotoxy
Purpose: Set VERA address to point to specific coordinates on screen.<br>
Call address: `VTUILIB+14`<br>
Communication registers: .A & .Y<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Point the VERA address to a specific set of coordinates on screen. This works in both 80x60 mode and 40x30 mode. If the point is outside of visible area and character is plotted, it will not be visible. There is no error handling. .Y is the y-coordinate (0-29/59) and .A is the x-coordinate (0-39/79). This function does not actually display anything on screen.

## Function name: plot_char
Purpose: Write a screencode character and color to screen.<br>
Call address: `VTUILIB+17`<br>
Communication registers: .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Write the screencode character in .A to the screen at current address. The routine expects VERA to increment by one as it writes the background-/foreground-color in .X to VERA without touching VERA addresses.<br>
**VERA screencodes**<br>
![VERA charactermap](https://cx16.dk/veratext/verachars.jpg)<br>
**VERA colors**<br>
![VERA colors](https://cx16.dk/veratext/veracolors.jpg)

## Function name: scan_char
Purpose: Read a screencode character and color from screen memory<br>
Call address: `VTUILIB+20`<br>
Communication registers: .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Read the screencode character at current VERA address into .A. The routine expects VERA to increment by one as it reads the background-/foreground-color into .X without touching VERA addresses.

## Function name: hline
Purpose: Draw a horizontal line from left to right.<br>
Call address: `VTUILIB+23`<br>
Communication registers: .A, .X & .Y<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A<br>
ZP registers affected: none<br>

**Description** Draw a horizontal line from left to right, starting at current position. Length of the line is provided in .Y register. Character to use for drawing the line is provided in .A register and the background-/foreground-color to use is provided in .X register.

## Function name: vline
Purpose: Draw a vertical line from top to bottom.<br>
Call address: `VTUILIB+26`<br>
Communication registers: .A, .X & .Y<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A<br>
ZP registers affected: none<br>

**Description** Draw a vertical line from top to bottom, starting at current position. Height of the line is provided in .Y register. Character to use for drawing the line is provided in .A and the background-/foreground-color to use is provided in .X register.

## Function name: print_str
Purpose: Print a string to screen.<br>
Call address: `VTUILIB+29`<br>
Communication registers: x16 & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A & .Y<br>
ZP registers affected: none<br>

**Description** Print a 0-terminated PETSCII encoded string to screen. The routine will convert PETSCII characters in the range $20-$59. Other characters will be converted to a large X-like character. x16 ($22 & $23) is a 16bit zeropage pointer to the string. Background-/foreground color for the string must be provided in .X register.

## Function name: fill_box
Purpose: Draw a filled box<br>
Call address: `VTUILIB+32`<br>
Communication registers: x16h, x17l, x17h & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A & .Y<br>
ZP registers affected: none<br>

**Description** Draw a filled box starting at current position.<br>

|Registers | Purpose               |
|------|-----------------------|
| x16h | Character for filling |
| x17l | Width of box          |
| x17h | Height of box         |
|  .X  | bg-/fg-color          |

## Function name: pet2scr
Purpose: Convert PETSCII to screencode<br>
Call address: `VTUILIB+35`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Convert the PETSCII character in .A to screencode. Supported range is $20-$59. Other characters will be converted to a large X-like character.

## Function name: scr2pet
Purpose: Convert screencode to PETSCII<br>
Call address: `VTUILIB+38`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Convert the screencode in .A to PETSCII. Supported range is $00-$39. Other characters will be converted to a large X-like character.

## Function name: border
Purpose: Draw a box with border<br>
Call address: `VTUILIB+41`<br>
Communication registers: .A, .X, x17l & x17h<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .Y
ZP registers affected: x16l, x16h, x18l-x19h + 4 more addresses ($22,$23,$26-$2D)

**Description** Create a box with a specific border.<br>

|Registers|Purpose     |
|------|---------------|
|  .A  | Border mode   |
| x17l | Width of box  |
| x17h | Height of box |
|  .X  | bg-/fg-color  |

***Supported Modes***<br>

|Borders| | | | | | |
|-------|-|-|-|-|-|-|
|Mode|0|1|2|3|4|5|
|Visual|![border0](images/border0.jpg)|![border1](images/border1.jpg)|![border2](images/border2.jpg)|![border3](images/border3.jpg)|![border4](images/border4.jpg)|![border5](images/border5.jpg)|

## Function name: save_rect
Purpose: Save an area from the screen to memory<br>
Call address: `VTUILIB+44`<br>
Communication registers: .C, .A, x16, x17l, x17h<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A, .X & .Y
ZP registers affected: x16, x17h

**Description** Save an area from screen to memory. Notice that each character on screen takes up 2 bytes of memory because a byte is used for color information.<br>

|Register|Purpose|
|--------|-------|
|   .C   |  Destination RAM (0=System RAM, 1=VRAM) |
|   .A   | VRAM bank if .C = 1 |
|  x16   | 16bit destination address |
|  x17l  | Width of area to save |
|  X17h  | Height of area to save |

## Function name: rest_rect
Purpose: Restore an area on screen from memory<br>
Call address: `VTUILIB+47`<br>
Communication registers: .C, .A, .x16, x17l, x17h<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A, .X & .Y
ZP registers affected: x16, x17h

**Description** Restore an area on screen from memory.<br>

|Register|Purpose|
|--------|-------|
|   .C   |  Destination RAM (0=System RAM, 1=VRAM) |
|   .A   | VRAM bank if .C = 1 |
|  x16   | 16bit destination address |
|  x17l  | Width of area to save |
|  X17h  | Height of area to save |
