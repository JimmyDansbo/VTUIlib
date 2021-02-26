# VTUI library Programmers Reference

Version 0.5

*Author: Jimmy Dansbo*

**This is preliminary documentation and can change at any point.**

This document describes the **V**ERA **T**ext **U**ser **I**nterface library.

**Table of Contents**

* [Overview](#overview)
* [Loading the library](#loading)
* [Including the library](#including)
* [Initializing the library](#initialization)
* [Registers](#registers)
* [Functions](#functions)
	* [initialize](#function-name-initialize)
	* [screen_set](#function-name-screen_set)
	* [set_bank](#function-name-set_bank)
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
* ACME include file
* CA65 include file

The generic library is meant to be compiled into a binary by it self and loaded by the users
program. The choice of compiler/assembler is entirely up to the user as long as it is possible
to store values to zeropage and in registers before calling subroutines.

For examples, look at the exampleXX.asm files

The other two flavors are include files for their respective assemblers but will function the same way as the generic library.

## Loading

The generic VTUI library is designed to be loaded by standard CBM kernal functions [SETLFS](https://cx16.dk/c64-kernal-routines/setlfs.html), [SETNAM](https://cx16.dk/c64-kernal-routines/setnam.html) and [LOAD](https://cx16.dk/c64-kernal-routines/load.html).

In several assemblers it is possible to load a a binary file directly with the sourcecode. for ACME it is done something like this `VTUI !BIN "VTUI0.5.BIN"` and for CA65 it would be done like this `VTUI .INCBIN "VTUI0.5.BIN"`.

If an assembler is used to include the binary file, be aware that the first two bytes are a loading address so base address of the actual library will be: `VTUILIB=VTUI+2`.

When using the CBM kernal functions to load the library, the LOAD command will remove the first two bytes before writing the library to memory.

When the library is loaded and VTUILIB is pointing to the memory address where the library starts (load address + 2 if loaded by assembler, otherwise just load address) it needs to be initalized by calling the initialization subroutine at VTUILIB address. All functions of the library are called by reference to the base VTUILIB address.

## Including

Include files are provided for the ACME and the CA65 assemblers. Obviously it is not necessary to to load the library separately if it is included. The include files are also missing the initialization routine as it is not needed when included.

One advantage of using include files instead of the generic library is that the include files provide constants for the 16 default colors, constants for VERA addresses and names for the zeropage registers used by the library.

Another advantage is that all functions are provided both as macros and routines. All macro names are all upper case and all start with VTUI_ followed by the function name. Functionas are all lower case, start with vtui_ followed by the function name.

Example: `VTUI_GOTOXY` is the macro name and `vtui_gotoxy` is the function name.

For examples look at acme-exXX.asm or ca65-exXX.asm file(s)

## Initialization

As the generic VTUI library is built without knowledge of where in memory it will be loaded, it is
necessary to initialize the library before use. The initialization ensures that the jumptable at the beginning of the library is updated to point to the correct address where functions are loaded.

After initialization, all functions can be called by referencing the base address of the library `VTUILIB`

## Registers

Several zeropage addresses are used by the library for temporary storage space as well as parameter passing. Addresses used are `r0 - r6` (`$02 - $0F`). These are the same registers as is used by the new
kernal functions in the Commander X16.

The VTUI library mostly uses r0, r1l and r2l for parameter passing, r0-r6 are also used for temporary storage. All zeropage registers can be discarded as soon as a routine has returned.

In addition to the zeropage memory, the VTUI library uses CPU registers for transferring arguments to the functions as well as temporary space and indexing.

## Functions

### Function name: initialize
Purpose: Initialize jump table in preparation for library use.<br>
Call address: `VTUILIB + 0`<br>
Macro name: none<br>
Routine name: none<br>
Communication registers: none<br>
Preparatory routines: none<br>
Registers affected: .A, .X & .Y<br>
ZP registers affected: r0, r1, r2 & r3 ($02-$09)<br>

**Description:** The routine, initialize, writes a very small subroutine in zeropage memory $02-$09 and calls it to get the return address off of the stack. This is the way the library figures out the correct addresses and updates the builtin jumptable.

### Function name: screen_set
Purpose: Set the screen mode to supported text mode<br>
Call address: `VTUILIB+2`<br>
Macro name: `VTUI_SCREEN_SET`<br>
Routine name: `vtui_screen_set`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: .A, .X & .Y<br>
ZP registers affected: none<br>

**Description** This function sets or toggles the screenmode. Supported modes are 0 = 40x30 & 2 80x60. Mode 255 ($FF) will toggle between the two modes. Any other mode will fail silently.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Screen mode ($00, $02 or $FF) |

## Function name: set_bank
Purpose: Set the VERA bank to 0 or 1<br>
Call address: `VTUILIB+5`<br>
Macro name: `VTUI_SET_BANK`<br>
Routine name: `vtui_set_bank`<br>
Communication registers: .C<br>
Preparatory routines: none<br>
Registers affected: .A<br>
ZP registers affected: none<br>

**Description** Set the VERA bank (high address bit). The value in .C can be either 0 or 1.

|Registers | Purpose               |
|------|-----------------------|
|  .C  | Single bit bank number (0 or 1) |

## Function name: set_stride
Purpose: Set the VERA stride value<br>
Call address: `VTUILIB+8`<br>
Macro name: `VTUI_SET_STRIDE`<br>
Routine name: `vtui_set_stride`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: .A<br>
ZP registers affected: r0l ($02)<br>

**Description** Set the VERA stride value. Stride is the amount the VERA address is incremented or decremented on each access. Stride is a 4 bit value and the routine will ensure that the number is converted to fit in VERA_ADDR_H. For more information about VERA stride, see the [VERA Documentation](https://github.com/commanderx16/x16-docs/blob/master/VERA%20Programmer's%20Reference.md#video-ram-access) about 'Address Increment'

|Registers | Purpose               |
|------|-----------------------|
|  .A  | 4 bit stride value |

## Function name: set_decr
Purpose: Set the VERA decrement bit<br>
Call address: `VTUILIB+11`<br>
Macro name: `VTUI_SET_DECR`<br>
Routine name: `vtui_set_decr`<br>
Communication registers: .C<br>
Preparatory routines: none<br>
Registers affected: .A
ZP registers affected: none<br>

**Description** Set the VERA decrement bit. The decrement bit decides if the stride value is added to- or subtracted from the current VERA address. Carry Clear (.C=0) means increment by stride value. Carry Set (.C=1) means decrement by stride value.

|Registers | Purpose               |
|------|-----------------------|
|  .C  | 0 = Increment, 1 = Decrement |


## Function name: gotoxy
Purpose: Set VERA address to point to specific coordinates on screen.<br>
Call address: `VTUILIB+14`<br>
Macro name: `VTUI_GOTOXY`<br>
Routine name: `vtui_gotoxy`<br>
Communication registers: .A & .Y<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Point the VERA address to a specific set of coordinates on screen. This works in both 80x60 mode and 40x30 mode. If the point is outside of visible area and character is plotted, it will not be visible. There is no error handling. .Y is the y-coordinate (0-29/59) and .A is the x-coordinate (0-39/79). This function does not actually display anything on screen.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | x coordinate |
|  .X  | y coordinate |


## Function name: plot_char
Purpose: Write a screencode character and color to screen.<br>
Call address: `VTUILIB+17`<br>
Macro name: `VTUI_PLOT_CHAR`<br>
Routine name: `vtui_plot_char`<br>
Communication registers: .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Write the screencode character in .A to the screen at current address. The routine expects VERA to increment by one as it writes the background-/foreground-color in .X to VERA without touching VERA addresses.<br>

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Character to write to screen |
|  .X  | bg-/fg-color to write to screen |

**VERA screencodes**<br>
![VERA charactermap](https://cx16.dk/veratext/verachars.jpg)<br>
**VERA colors**<br>
![VERA colors](https://cx16.dk/veratext/veracolors.jpg)

## Function name: scan_char
Purpose: Read a screencode character and color from screen memory<br>
Call address: `VTUILIB+20`<br>
Macro name: `VTUI_SCAN_CHAR`<br>
Routine name: `vtui_scan_char`<br>
Communication registers: .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Read the screencode character at current VERA address into .A. The routine expects VERA to increment by one as it reads the background-/foreground-color into .X without touching VERA addresses.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Character read from screen memory |
|  .X  | Colorcode read from screen memory |

## Function name: hline
Purpose: Draw a horizontal line from left to right.<br>
Call address: `VTUILIB+23`<br>
Macro name: `VTUI_HLINE`<br>
Routine name: `vtui_hline`<br>
Communication registers: .A, .X & .Y<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A<br>
ZP registers affected: none<br>

**Description** Draw a horizontal line from left to right, starting at current position. Length of the line is provided in .Y register. Character to use for drawing the line is provided in .A register and the background-/foreground-color to use is provided in .X register.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Character for drawing line |
|  .X  | bg-/fg-color  |
|  .Y  | length of line  |

## Function name: vline
Purpose: Draw a vertical line from top to bottom.<br>
Call address: `VTUILIB+26`<br>
Macro name: `VTUI_VLINE`<br>
Routine name: `vtui_vline`<br>
Communication registers: .A, .X & .Y<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A<br>
ZP registers affected: none<br>

**Description** Draw a vertical line from top to bottom, starting at current position. Height of the line is provided in .Y register. Character to use for drawing the line is provided in .A and the background-/foreground-color to use is provided in .X register.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Character for drawing line |
|  .X  | bg-/fg-color  |
|  .Y  | Height of line  |

## Function name: print_str
Purpose: Print a string to screen.<br>
Call address: `VTUILIB+29`<br>
Macro name: `VTUI_PRINT_STR`<br>
Routine name: `vtui_print_str`<br>
Communication registers: r0 ($02-$03), .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A & .Y<br>
ZP registers affected: none<br>

**Description** Print a 0-terminated string to screen. If .A=0, The routine will convert PETSCII characters in the range $20-$59. Other characters will be converted to a large X-like character. If .A is set to $80, no conversion will take place. r0 ($02 & $03) is a 16bit zeropage pointer to the string. Background-/foreground color for the string must be provided in .X register.

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Convert string (0 = convert, $80 = no conversion) |
|  r0  | Pointer to start of string |
|  .X  | bg-/fg-color  |

## Function name: fill_box
Purpose: Draw a filled box<br>
Call address: `VTUILIB+32`<br>
Macro name: `VTUI_FILL_BOX`<br>
Routine name: `vtui_fill_box`<br>
Communication registers: r1l ($04), r2l ($06), .A & .X<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .Y<br>
ZP registers affected: r2l ($06)<br>

**Description** Draw a filled box starting at current position.<br>

|Registers | Purpose               |
|------|-----------------------|
|  .A  | Character for filling |
| r1l  | Width of box          |
| r2l  | Height of box         |
|  .X  | bg-/fg-color          |

## Function name: pet2scr
Purpose: Convert PETSCII to screencode<br>
Call address: `VTUILIB+35`<br>
Macro name: `VTUI_PET2SCR`<br>
Routine name: `vtui_pet2scr`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Convert the PETSCII character in .A to screencode. Supported range is $20-$59. Other characters will be converted to a large X-like character.

|Registers | Purpose               |
|------|-----------------------|
| .A   | Character to convert |

## Function name: scr2pet
Purpose: Convert screencode to PETSCII<br>
Call address: `VTUILIB+38`<br>
Macro name: `VTUI_SCR2PET`<br>
Routine name: `vtui_scr2pet`<br>
Communication registers: .A<br>
Preparatory routines: none<br>
Registers affected: none<br>
ZP registers affected: none<br>

**Description** Convert the screencode in .A to PETSCII. Supported range is $00-$39. Other characters will be converted to a large X-like character.

|Registers | Purpose               |
|------|-----------------------|
| .A   | Character to convert |

## Function name: border
Purpose: Draw a box with border<br>
Call address: `VTUILIB+41`<br>
Macro name: `VTUI_BORDER`<br>
Routine name: `vtui_border`<br>
Communication registers: .A, .X, r1l ($04) & r2l ($06)<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .Y
ZP registers affected: r0l ($02), r0h ($03), r3l - r6h ($08 - $0F)

**Description** Create a box with a specific or custom border.

Modes 0-5 are pre-defined, only works with the default character set and are shown below. If mode 6 is specified, the border characters can be defined as follows:<br>

|Register|Address|Border character|
|--------|-------|----------------|
| r3l    | $08   | Top right corner |
| r3h    | $09   | Top left corner |
| r4l    | $0A   | Bottom right corner |
| r4h    | $0B   | Bottom left corner |
| r5l    | $0C   | Top horizontal line character |
| r5h    | $0D   | Bottom horizontal line character |
| r6l    | $0E   | Left vertical line character |
| r6h    | $0F   | Right vertical line character |

***Parameters***<br>

|Registers|Purpose     |
|------|---------------|
|  .A  | Border mode   |
| r1l  | Width of box  |
| r2l  | Height of box |
|  .X  | bg-/fg-color  |

***Supported Modes***<br>

|Borders| | | | | | | |
|-------|-|-|-|-|-|-|-|
|Mode|0|1|2|3|4|5|6|
|Visual|![border0](images/border0.jpg)|![border1](images/border1.jpg)|![border2](images/border2.jpg)|![border3](images/border3.jpg)|![border4](images/border4.jpg)|![border5](images/border5.jpg)|Custom|

## Function name: save_rect
Purpose: Save an area from the screen to memory<br>
Call address: `VTUILIB+44`<br>
Macro name: `VTUI_SAVE_RECT`<br>
Routine name: `vtui_save_rect`<br>
Communication registers: .C, .A, r0 ($02-$03), r1l ($04), r2l ($06)<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A, .X & .Y
ZP registers affected: r0 ($02-$03), r2l ($06)

**Description** Save an area from screen to memory. Notice that each character on screen takes up 2 bytes of memory because a byte is used for color information.<br>

|Register|Purpose|
|--------|-------|
|   .A   |  Destination RAM (0=System RAM, 1=VRAM) |
|   .C   | VRAM bank if .A > 0 |
|   r0   | 16bit destination address |
|   r1l  | Width of area to save |
|   r2l  | Height of area to save |

## Function name: rest_rect
Purpose: Restore an area on screen from memory<br>
Call address: `VTUILIB+47`<br>
Macro name: `VTUI_REST_RECT`<br>
Routine name: `vtui_rest_rect`<br>
Communication registers: .C, .A, r0 ($02-$03), r1l ($04), r2l ($06)<br>
Preparatory routines: gotoxy (optional)<br>
Registers affected: .A, .X & .Y
ZP registers affected: r0 ($02-$03), r2l ($06)

**Description** Restore an area on screen from memory.<br>

|Register|Purpose|
|--------|-------|
|   .A   | Source RAM (0=System RAM, 1=VRAM) |
|   .C   | VRAM bank if .A > 0 |
|   r0   | 16bit destination address |
|   r1l  | Width of area to save |
|   r2l  | Height of area to save |
