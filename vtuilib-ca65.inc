; *********************** VERA Addresses *************************************
VERA_ADDR_L		= $9F20
VERA_ADDR_M		= $9F21
VERA_ADDR_H		= $9F22
VERA_DATA0		= $9F23
VERA_DATA1		= $9F24
VERA_CTRL		= $9F25

; ************************ Zeropage registers ********************************
r0	= $02
r0l	= r0
r0h	= r0+1
r1	= $04
r1l	= r1
r1h	= r1+1
r2	= $06
r2l	= r2
r2h	= r2+1
r3	= $08
r3l	= r3
r3h	= r3+1
r4	= $0A
r4l	= r4
r4h	= r4+1
r5	= $0C
r5l	= r5
r5h	= r5+1
r6	= $0E
r6l	= r6
r6h	= r6+1
r7	= $10
r7l	= r7
r7h	= r7+1
r8	= $12
r8l	= r8
r8h	= r8+1
r9	= $14
r9l	= r9
r9h	= r9+1
r10	= $16
r10l	= r10
r10h	= r10+1
r11	= $18
r11l	= r11
r11h	= r11+1
r12	= $1A
r12l	= r12
r12h	= r12+1

; *************************** Color constants ********************************
BLACK		= 0
WHITE		= 1
RED		= 2
CYAN		= 3
PURPLE		= 4
GREEN		= 5
BLUE		= 6
YELLOW		= 7
ORANGE		= 8
BROWN		= 9
LIGHTRED	= 10
DARKGRAY	= 11
MIDGRAY		= 12
LIGHTGREEEN	= 13
LIGHTBLUE	= 14
LIGHTGRAY	= 15

; ******************************* Functions ***********************************
VTUI_LIB=VTUI+2

vtui_initialize	= VTUI_LIB+0
vtui_screen_set	= VTUI_LIB+2
vtui_set_bank	= VTUI_LIB+5
vtui_set_stride	= VTUI_LIB+8
vtui_set_decr	= VTUI_LIB+11
vtui_clr_scr	= VTUI_LIB+14
vtui_gotoxy	= VTUI_LIB+17
vtui_plot_char	= VTUI_LIB+20
vtui_scan_char	= VTUI_LIB+23
vtui_hline	= VTUI_LIB+26
vtui_vline	= VTUI_LIB+29
vtui_print_str	= VTUI_LIB+32
vtui_fill_box	= VTUI_LIB+35
vtui_pet2scr	= VTUI_LIB+38
vtui_scr2pet	= VTUI_LIB+41
vtui_border	= VTUI_LIB+44
vtui_save_rect	= VTUI_LIB+47
vtui_rest_rect	= VTUI_LIB+50
vtui_input_scr	= VTUI_LIB+53
vtui_get_bank	= VTUI_LIB+56
vtui_get_stride	= VTUI_LIB+59
vtui_get_decr	= VTUI_LIB+62

	jsr	vtui_initialize
	jmp	VTUI_LIB_END

; *****************************************************************************
; Use KERNAL API to set screen to 80x60 or 40x30 or swap between them.
; *****************************************************************************
; INPUT:		.A = Screenmode ($00, $02 or $FF)
; USES:			.A, .X & ,Y
; RETURNS:		.C = 1 in case of error.
; *****************************************************************************
.macro VTUI_SCREEN_SET
	jsr	vtui_screen_set
.endmacro

; *****************************************************************************
; Set VERA bank (High memory) without touching anything else
; *****************************************************************************
; INPUTS:	.C = Bank number, 0 or 1
; USES:		.A
; *****************************************************************************
.macro VTUI_SET_BANK
	jsr	vtui_set_bank
.endmacro

; *****************************************************************************
; Get current VERA bank (high memory bit)
; *****************************************************************************
; USES:		.A
; RETURNS:	.C = Bank number, 0 or 1
; *****************************************************************************
.macro VTUI_GET_BANK
	jsr	vtui_get_bank
.endmacro

; *****************************************************************************
; Set the stride without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.A = Stride value
; USES:			r0l
; *****************************************************************************
.macro VTUI_SET_STRIDE
	jsr	vtui_set_stride
.endmacro

; *****************************************************************************
; Get current VERA stride value
; *****************************************************************************
; RETURNS:		.A = stride value
; *****************************************************************************
.macro VTUI_GET_STRIDE
	jsr	vtui_get_stride
.endmacro

; *****************************************************************************
; Set the decrement value without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.C (1 = decrement, 0 = increment)
; USES:			.A
; *****************************************************************************
.macro VTUI_SET_DECR
	jsr	vtui_set_decr
.endmacro

; *****************************************************************************
; Get the current VERA decrement value
; *****************************************************************************
; USES:			.A
; RETURNS:		.C (1 = decrement, 0 = increment)
; *****************************************************************************
.macro VTUI_GET_DECR
	jsr	vtui_get_decr
.endmacro

; *****************************************************************************
; Write character and possibly color to current VERA address
; If VERA stride = 1 and decrement = 0, colorcode in X will be written as well.
; *****************************************************************************
; INPUTS:	.A = character
;		.X = bg-/fg-color
; USES:		.A
; *****************************************************************************
.macro VTUI_PLOT_CHAR
	jsr	vtui_plot_char
.endmacro

; *****************************************************************************
; Read character and possibly color from current VERA address
; If VERA stride = 1 and decrement = 0, colorcode will be returned in X
; *****************************************************************************
; OUTPUS:	.A = character
;		.X = bg-/fg-color
; USES		.X
; *****************************************************************************
.macro VTUI_SCAN_CHAR
	jsr	vtui_scan_char
.endmacro

; *****************************************************************************
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Length of the line
;		.X	= bg- & fg-color
; *****************************************************************************
.macro VTUI_HLINE
	jsr	vtui_hline
.endmacro

; *****************************************************************************
; Create a vertical line going from top to bottom.
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Height of the line
;		.X	= bg- & fg-color
; USES:		r1h & r2h
; *****************************************************************************
.macro VTUI_VLINE
	jsr	vtui_vline
.endmacro

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	.A = x coordinate
;		.Y = y coordinate
; *****************************************************************************
.macro VTUI_GOTOXY
	jsr	vtui_gotoxy
.endmacro

; *****************************************************************************
; Convert PETSCII codes between $20 and $5F to screencodes.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $56 if invalid input
; *****************************************************************************
.macro VTUI_PET2SCR
	jsr	vtui_pet2scr
.endmacro

; *****************************************************************************
; Convert screencodes between $00 and $3F to PETSCII.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $76 if invalid input
; *****************************************************************************
.macro VTUI_SCR2PET
	jsr	vtui_scr2pet
.endmacro

; *****************************************************************************
; Print PETSCII/Screencode string.
; *****************************************************************************
; INPUTS	.A = Convert string (0 = Convert from PETSCII, $80 = no conversion)
;		r0 = pointer to string
;		.Y = length of string
;		.X  = bg-/fg color (only used if stride=0,decr=0&bank=0)
; USES:		.A, .Y & r1
; *****************************************************************************
.macro VTUI_PRINT_STR
	jsr	vtui_print_str
.endmacro

; *****************************************************************************
; Create a filled box drawn from top left to bottom right
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		r1l	= Width of box
;		r2l	= Height of box
;		.X	= bg- & fg-color
; *****************************************************************************
.macro VTUI_FILL_BOX
	jsr	vtui_fill_box
.endmacro

; *****************************************************************************
; Clear the entire screen with specific character and color
; *****************************************************************************
; INPUTS:	.A	= Character to use for filling
;		.X	= bg- & fg-color
; USES:		.Y, r1l & r2l
; *****************************************************************************
.macro VTUI_CLR_SCR
	jsr	vtui_clr_scr
.endmacro

; *****************************************************************************
; Create a box with a specific border
; *****************************************************************************
; INPUTS:	.A	= Border mode (0-6) any other will default to mode 0
;		r1l	= width
;		r2l	= height
;		.X	= bg-/fg-color
; USES		.Y, r0, r1h & r2h
; *****************************************************************************
.macro VTUI_BORDER
	jsr	vtui_border
.endmacro

; *****************************************************************************
; Copy contents of screen from current position to other memory area in
; either system RAM or VRAM
; *****************************************************************************
; INPUTS:	.C	= VRAM Bank (0 or 1) if .A=$80
;		.A	= Destination RAM (0=system RAM, $80=VRAM)
;		r0 	= Destination address
;		r1l	= width
;		r2l	= height
; USES:		r1h
; *****************************************************************************
.macro VTUI_SAVE_RECT
	jsr	vtui_save_rect
.endmacro

; *****************************************************************************
; Restore contents of screen from other memory area in either system RAM
; or VRAM starting at current position
; *****************************************************************************
; INPUTS:	.C	= VRAM Bank (0 or 1) if .A=$80
;		.A	= Source RAM (0=system RAM, $80=VRAM)
;		r0 	= Source address
;		r1l	= width
;		r2l	= height
; *****************************************************************************
.macro VTUI_REST_RECT
	jsr	vtui_rest_rect
.endmacro

; *****************************************************************************
; Show a cursor and get a string input from keyboard.
; *****************************************************************************
; INPUTS:	r0 = pointer to buffer to hold string (must be pre-allocated)
;		.Y = maximum length of string
;		.X = color information for input characters
; OUPUTS:	.Y = actual length of input
; USES:		.A & r1
; *****************************************************************************
.macro VTUI_INPUT_STR
	jsr	vtui_input_str
.endmacro

VTUI:	.incbin	"VTUI1.0.BIN"

VTUI_LIB_END:
