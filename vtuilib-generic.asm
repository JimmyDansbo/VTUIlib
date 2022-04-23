!cpu w65c02
; Program counter is set to 0 to make it easier to calculate the addresses
; in the jumptable as all that needs to be done is add the actual offset.
*=$0000

; ******************************* Jumptable ***********************************
INIT:	bra	initialize	; No inputs
SCRS:	jmp	vtui_screen_set	; .A = Screenmode
SETB:	jmp	vtui_set_bank	; .C = bank number (0 or 1)
SETS:	jmp	vtui_set_stride	; .A = Stride value
SETD:	jmp	vtui_set_decr	; .C (1 = decrement, 0 = increment)
CLRS:	jmp	vtui_clr_scr	; .A = Character, .X = bg-/fg-color
GOTO:	jmp	vtui_gotoxy	; .A = x coordinate, .Y = y coordinate
PLCH:	jmp	vtui_plot_char	; .A = character, .X = bg-/fg-color
SCCH:	jmp	vtui_scan_char	; like plot_char
HLIN:	jmp	vtui_hline	; .A = Character, .Y = length, .X = color
VLIN:	jmp	vtui_vline	; .A = Character, .Y = height, .X = color
PSTR:	jmp	vtui_print_str	; r0 = pointer to string, .X = color
FBOX:	jmp	vtui_fill_box	; .A=Char,r1l=width,r2l=height,.X=color
P2SC:	jmp	vtui_pet2scr	; .A = character to convert to screencode
SC2P:	jmp	vtui_scr2pet	; .A = character to convert to petscii
BORD:	jsr	vtui_border	; .A=border,r1l=width,r2l=height,.X=color
SREC:	jmp	vtui_save_rect	; .C=vrambank,.A=destram,r0=destaddr,r1l=width,r2l=height
RREC:	jmp	vtui_rest_rect	; .C=vrambank,.A=srcram,r0=srcaddr,r1l=width,r2l=height
INST:	jmp	vtui_input_str	; r0 = pointer to buffer, .Y=max length, X=color
	jmp	$0000		; Show that there are no more jumps

border_modes:;	 TR  TL  BR  BL TOP BOT  L   R
	!byte	$20,$20,$20,$20,$20,$20,$20,$20
	!byte	$66,$66,$66,$66,$66,$66,$66,$66
	!byte	$6E,$70,$7D,$6D,$40,$40,$42,$42
	!byte	$49,$55,$4B,$4A,$40,$40,$42,$42
	!byte	$50,$4F,$7A,$4C,$77,$6F,$74,$6A
	!byte	$5F,$69,$E9,$DF,$77,$6F,$74,$6A


; ******************************* Constants ***********************************
OP_PHA		= $48		; PHA opcode
OP_PLA		= $68		; PLA opcode
OP_PHY		= $5A		; PHY opcode
OP_PLY		= $7A		; PLY opcode
OP_RTS		= $60		; RTS opcode
OP_JMP_ABS	= $4C		; JMP absolute opcode

PLOT_CHAR	= $10		; zp jump to plot_char function
HLINE		= $13		; zp jump to hline function
VLINE		= $16		; zp jump to vline function

VERA_ADDR_L	= $9F20
VERA_ADDR_M	= $9F21
VERA_ADDR_H	= $9F22
VERA_DATA0	= $9F23
VERA_DATA1	= $9F24
VERA_CTRL	= $9F25

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

; *************************** Internal Macros *********************************

; *****************************************************************************
; Increment 16bit value
; *****************************************************************************
; INPUT:	.addr = low byte of the 16bit value to increment
; *****************************************************************************
!macro INC16 .addr {
	inc	.addr
	bne	.end
	inc	.addr+1
.end:
}

; ******************************* Functions ***********************************

; *****************************************************************************
; Initialize the jumptable with correct addresses calculated from the address
; where this code is loaded.
; *****************************************************************************
; USES:		.A, .X & .Y
;		r0, r1, r2 & r3 (ZP addresses $02-$09)
; *****************************************************************************
initialize:
@base	= r0
@ptr	= r1
	; Write code to ZP to figure out where the library is loaded.
	; This is done by jsr'ing to the code in ZP which in turn reads the
	; return address from the stack.
	lda	#OP_PLA
	sta	r0
	lda	#OP_PLY
	sta	r0+1
	lda	#OP_PHY
	sta	r0+2
	lda	#OP_PHA
	sta	r0+3
	lda	#OP_RTS
	sta	r0+4
	; Jump to the code in ZP that was just copied there by the code above.
	; This is to get the return address stored on stack
	jsr	r0		; Get current PC value
	sec
	sbc	#*-2		; Calculate start of our program
	sta	@base		; And store it in @base
	tya
	sbc	#$00
	sta	@base+1
	lda	@base		; Calculate location of first address in
	clc			; jump table
	adc	#$03
	sta	@ptr
	lda	@base+1
	adc	#$00
	sta	@ptr+1

	ldy	#1		; .Y used for indexing high byte of pointers
	lda	(@ptr),y
	beq	@loop		; If high byte of pointer is 0, we can continue
	rts			; Otherwise initialization has already been run

@loop:	lda	(@ptr)		; Check if lowbyte of address is 0
	bne	+		; If not, we have not reached end of jumptable
	lda	(@ptr),y	; Check if highbyte of address is 0
	beq	@end		; If it is, we have reaced end of jumptable
+	clc
	lda	(@ptr)		; Low part of jumptable address
	adc	@base		; Add start address of our program to the jumptable address
	sta	(@ptr)
	lda	(@ptr),y
	adc	@base+1
	sta	(@ptr),y

	lda	@ptr
	clc
	adc	#$03
	sta	@ptr
	bcc	@loop
	inc	@ptr+1
	bra	@loop
@end:	rts

; *****************************************************************************
; Use KERNAL API to set screen mode or swap between them.
; *****************************************************************************
; INPUT:		.A = Screenmode ($00-$06 & $80 or $FF)
; USES:			.A, .X & ,Y
; RETURNS:		.C = 1 in case of error.
; Supported screen modes as of ROM version R39:
; $00: 80x60 text
; $01: 80x30 text
; $02: 40x60 text
; $03: 40x30 text
; $04: 40x15 text
; $05: 20x30 text
; $06: 20x15 text
; $80: 320x200@256c (40x26 text)
; *****************************************************************************
vtui_screen_set:
	clc			; Clear carry to ensure screen mode is set
	jmp	$FF5F		; screen_set_mode X16 kernal API call.

; *****************************************************************************
; Set VERA bank (High memory) without touching anything else
; *****************************************************************************
; INPUTS:	.C = Bank number, 0 or 1
; USES:		.A
; *****************************************************************************
vtui_set_bank:
	lda	VERA_ADDR_H
	ora	#$01
	bcs	@end
	and	#$FE
@end:	sta	VERA_ADDR_H
	rts

; *****************************************************************************
; Set the stride without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.A = Stride value
; USES:			r0l
; *****************************************************************************
vtui_set_stride:
@tmp	= r0l
	asl			; Stride is stored in upper nibble
	asl
	asl
	asl
	sta	@tmp
	lda	VERA_ADDR_H	; Set stride value to 0 in VERA_ADDR_H
	and	#$0F
	ora	@tmp
	sta	VERA_ADDR_H
	rts

; *****************************************************************************
; Set the decrement value without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.C (1 = decrement, 0 = increment)
; USES:			.A
; *****************************************************************************
vtui_set_decr:
	lda	VERA_ADDR_H
	ora	#%00001000
	bcs	@end
	and	#%11110111
@end:	sta	VERA_ADDR_H
	rts

; *****************************************************************************
; Write character and possibly color to current VERA address
; If VERA stride = 1 and decrement = 0, colorcode in X will be written as well.
; *****************************************************************************
; INPUTS:	.A = character
;		.X = bg-/fg-color
; USES:		.A
; *****************************************************************************
!macro VTUI_PLOT_CHAR {
	sta	VERA_DATA0	; Store character
	lda	VERA_ADDR_H	; Isolate stride & decr value
	and	#$F8		; Ignore VRAM Bank
	cmp	#$10		; If stride=1 & decr=0 we can write color
	bne	+
	stx	VERA_DATA0	; Write color
+
}
vtui_plot_char:
	+VTUI_PLOT_CHAR
	rts

; *****************************************************************************
; Read character and possibly color from current VERA address
; If VERA stride = 1 and decrement = 0, colorcode will be returned in X
; *****************************************************************************
; OUTPUS:	.A = character
;		.X = bg-/fg-color
; USES		.X & .Y
; *****************************************************************************
vtui_scan_char:
	ldy	VERA_DATA0	; Read character
	lda	VERA_ADDR_H	; Isolate stride & decr value
	and	#$F8		; Ignore VRAM Bank
	cmp	#$10		; If stride=1 & decr=0 we can read color
	bne	+
	ldx	VERA_DATA0	; Read color
+	tya			; Move char to .A
	rts

; *****************************************************************************
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Length of the line
;		.X	= bg- & fg-color
; USES:		.Y & 1 byte on stack
; *****************************************************************************
vtui_hline:
	sta	VERA_DATA0
	pha			; Save .A so it can be used to check stride
	lda	VERA_ADDR_H
	and	#$F8		; Ignore VRAM Bank
	cmp	#$10		; If Stride=1 & Decr=0
	bne	+		; we can write the color
	stx	VERA_DATA0
+	pla			; Restore .A
	dey
	bne	vtui_hline
	rts

; *****************************************************************************
; Create a vertical line going from top to bottom.
; Function only works when stride is either 1 or 2 and decr = 0
; If stride is 1, color is expected in .X
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Height of the line
;		.X	= bg- & fg-color (if stride=1)
; USES:		Y & 1 byte on stack
; *****************************************************************************
vtui_vline:
	sta	VERA_DATA0	; Write character
	pha			; Save .A so it can be used to check stride
	lda	VERA_ADDR_H
	and	#$F8		; Ignore VRAM Bank
	cmp	#$10		; Store color if stride=1 & decr=0
	bne	+
	stx	VERA_DATA0	; Store colorcode
+	dec	VERA_ADDR_L	; Return to original X coordinate
	dec	VERA_ADDR_L
	inc	VERA_ADDR_M	; Increment Y coordinate
	pla			; Restore .A for next iteration
	dey
	bne	vtui_vline
	rts

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	.A = x coordinate
;		.Y = y coordinate
; *****************************************************************************
vtui_gotoxy:
	asl			; Multiply x coord with 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	tya
	adc	#$B0		; Add Y coord to base address
	sta	VERA_ADDR_M	; Set y coordinate
	rts

; *****************************************************************************
; Convert PETSCII codes between $20 and $5F to screencodes.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $56 if invalid input
; *****************************************************************************
!macro VTUI_PET2SCR {
	cmp	#$20
	bcc	.nonprintable	; .A < $20
	cmp	#$40
	bcc	.end		; .A < $40 means screen code is the same
	; .A >= $40 - might be letter
	cmp	#$60		; .A < $60 so it is a letter
	bcc	+
.nonprintable:
	lda	#$56+$40	; Load nonprintable char + value being subtracted.
+	sbc	#$3F		; subtract ($3F+1) to convert to screencode
.end:
}
vtui_pet2scr:
	+VTUI_PET2SCR
	rts


; *****************************************************************************
; Convert screencodes between $00 and $3F to PETSCII.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $76 if invalid input
; *****************************************************************************
vtui_scr2pet:
	cmp	#$40
	bcs	@nonprintable	; .A >= $40
	cmp	#$20
	bcs	@end		; .A >=$20 & < $40 means petscii is the same
	; .A < $20 and is a letter
	adc	#$40
	rts
@nonprintable:
	lda	#$76
@end:	rts

; *****************************************************************************
; Print PETSCII/Screencode string.
; *****************************************************************************
; INPUTS	.A = Convert string (0 = Convert from PETSCII, $80 = no conversion)
;		r0 = pointer to string
;		.Y = length of string
;		.X  = bg-/fg color (only used if stride=0,decr=0&bank=0)
; USES:		.A, .Y & r1
; *****************************************************************************
vtui_print_str:
@str	= r0
@conv	= r1l
@length	= r1h
	sta	@conv		; Store to check for conversion
	sty	@length		; Store Y for later use
	ldy	#0
@loop:	cpy	@length
	beq	@end
	lda	(@str),y	; Load character
	bit	@conv		; Check if we need to convert character
	bmi	@noconv
	+VTUI_PET2SCR		; Do conversion
@noconv:
	+VTUI_PLOT_CHAR
	iny
	bra	@loop		; Get next character
@end:	rts

; *****************************************************************************
; Clear the entire screen with specific character and color
; *****************************************************************************
; INPUTS:	.A	= Character to use for filling
;		.X	= bg- & fg-color
; USES:		.Y, r1l & r2l
; *****************************************************************************
vtui_clr_scr:
@width	= r1l
@height	= r2l
	ldy	#$B0
	sty	VERA_ADDR_M	; Ensure VERA address is at top left corner
	stz	VERA_ADDR_L
	ldy	#80		; Store max width = 80 columns
	sty	@width
	ldy	#60		; Store max height = 60 lines
	sty	@height
	; this falls through to vtui_fill_box

; *****************************************************************************
; Create a filled box drawn from top left to bottom right
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing
;		r1l	= Width of box
;		r2l	= Height of box
;		.X	= bg- & fg-color
; *****************************************************************************
vtui_fill_box:
@width	= r1l
@height	= r2l
@xcord	= r0l
	ldy	VERA_ADDR_L
	sty	@xcord
@vloop:	ldy	@xcord		; Load x coordinate
	sty	VERA_ADDR_L	; Set x coordinate
	ldy	@width
@hloop:	sta	VERA_DATA0
	stx	VERA_DATA0
	dey
	bne	@hloop
	inc	VERA_ADDR_M
	dec	@height
	bne	@vloop
	rts

; *****************************************************************************
; Create a box with a specific border
; *****************************************************************************
; INPUTS:	.A	= Border mode (0-6) any other will default to mode 0
;		r1l	= width
;		r2l	= height
;		.X	= bg-/fg-color
; USES		.Y, r0, r1h & r2h
; *****************************************************************************
vtui_border:
	; Define local variable names for ZP variables
	; Makes the source a bit more readable
@xcord		= r0l
@ycord		= r0h
@width		= r1l
@height		= r2l
@top_right	= r3l
@top_left	= r3h
@bot_right	= r4l
@bot_left	= r4h
@top		= r5l
@bottom		= r5h
@left		= r6l
@right		= r6h

	cmp	#6		; Skip border loading if mode is >= 6
	bcs	@find_funcs

	; Find address of border_modes lookup table
	sta	PLOT_CHAR+3	; Save Mode number
	pla			; Get low part of address and save in .Y
	tay

	; Calculate address of beginning of border_modes table
	clc
	adc	#(border_modes-BORD)-2
	sta	PLOT_CHAR	; Using PLOT_CHAR ZP variables temporarily
	pla			; Get high part of address
	pha
	adc	#$00
	sta	PLOT_CHAR+1
	phy			; Ensure entire address is pushed back on stack

	; Set the border drawing characters according to the border mode in .A
	phx			; Save color information on stack
	ldx	#0
	lda	PLOT_CHAR+3	; Restore mode number
	asl			; Multiply with 8 to get index to lookup table
	asl
	asl
	tay

-	lda	(PLOT_CHAR),y	; Load character from lookup table
	sta	@top_right,x	; Store it in ZP
	iny
	inx
	cpx	#8
	bne	-

	plx			; Restore color information from stack

	; Find jumptable address of needed functions
@find_funcs:
	pla			; Get low part of address and save in .Y
	tay
	sec
	sbc	#(BORD-PLCH)+2	; Caculate low jumptable address of PLOT_CHAR
	sta	PLOT_CHAR+1
	pla			; Get high part of address and store in stack again
	pha
	sbc	#$00		; Calculate high jumptable addr of PLOT_CHAR
	sta	PLOT_CHAR+2
	tya			; Get low part of address
	sec
	sbc	#(BORD-HLIN)+2	; Calculate low jumptable address of HLINE
	sta	HLINE+1
	pla			; Get high part of address and store in stack again
	pha
	sbc	#$00		; Calculate high jumptable addr of HLINE
	sta	HLINE+2
	tya			; Get low part of address
	sec
	sbc	#(BORD-VLIN)+2	; Calculate low jumptable address of VLINE
	sta	VLINE+1
	pla
	sbc	#$00
	sta	VLINE+2
	lda	#OP_JMP_ABS	; JMP absolute
	sta	PLOT_CHAR
	sta	HLINE
	sta	VLINE

	; Save initial position
	lda	VERA_ADDR_L	; X coordinate
	sta	@xcord
	lda	VERA_ADDR_M	; Y coordinate
	sta	@ycord
	ldy	@width		; width
	dey
	lda	@top_left
	jsr	PLOT_CHAR	; Top left corner
	dey
	lda	@top
	jsr	HLINE		; Top line

	lda	@top_right
	jsr	PLOT_CHAR	; Top right corner
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L
	inc	VERA_ADDR_M
	ldy	@height		;height
	dey
	dey
	lda	@right
	jsr	VLINE		; Right line
	; Restore initial VERA address
	lda	@xcord
	sta	VERA_ADDR_L
	lda	@ycord
	inc
	sta	VERA_ADDR_M
	ldy	@height		;height
	dey
	lda	@left
	jsr	VLINE		; Left line
	dec	VERA_ADDR_M
	lda	@bot_left
	jsr	PLOT_CHAR	; Bottom left corner
	ldy	@width
	dey
	lda	@bottom
	jsr	HLINE		; Bottom line
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L
	lda	@bot_right
	jmp	PLOT_CHAR	; Bottom right corner


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
vtui_save_rect:
@destram	= r1h
@width		= r1l
@height		= r2l
@destptr	= r0
	ldy	VERA_ADDR_L	; Save X coordinate for later
	sta	@destram	; Save destination RAM 0=sys $80=vram
	bit	@destram
	bpl	@skip_vram_prep
	lda	#1		; Set ADDRsel to 1
	sta	VERA_CTRL
	; Set stride and bank for VERA_DATA1
	lda	#$11		; Stride=1 & Bank = 1
	bcs	@storeval	; If C=1, store value
	lda	#$10		; Stride=1 & Bank = 0
@storeval:
	sta	VERA_ADDR_H
	; Set destination address for VERA_DATA1
	lda	@destptr
	sta	VERA_ADDR_L
	lda	@destptr+1
	sta	VERA_ADDR_M
	stz	VERA_CTRL	; Set ADDRsel back to 0
@skip_vram_prep:
	ldx	@width		; Load width
@loop:	lda	VERA_DATA0	; Load character
	bit	@destram
	bmi	@sto_char_vram
	sta	(@destptr)
	+INC16	@destptr
	bra	@get_col
@sto_char_vram:
	sta	VERA_DATA1
@get_col:
	lda	VERA_DATA0	; Load color code
	bit	@destram
	bmi	@sto_col_vram
	sta	(@destptr)
	+INC16	@destptr
	bra	@cont
@sto_col_vram:
	sta	VERA_DATA1
@cont:	dex
	bne	@loop
	ldx	@width		; Restore width
	sty	VERA_ADDR_L	; Restore X coordinate
	inc	VERA_ADDR_M
	dec	@height
	bne	@loop
	rts

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
vtui_rest_rect:
@srcram		= r1h
@width		= r1l
@height		= r2l
@srcptr		= r0
	ldy	VERA_ADDR_L	; Save X coordinate for later
	sta	@srcram		; Save source RAM 0=sys $80=vram
	bit	@srcram
	bpl	@skip_vram_prep
	lda	#1		; Set ADDRsel to 1
	sta	VERA_CTRL
	; Set stride and bank for VERA_DATA1
	lda	#$11		; Stride=1 & Bank = 1
	bcs	@storeval	; If C=1, store value
	lda	#$10		; Stride=1 & Bank = 0
@storeval:
	sta	VERA_ADDR_H
	; Set source address for VERA_DATA1
	lda	@srcptr
	sta	VERA_ADDR_L
	lda	@srcptr+1
	sta	VERA_ADDR_M
	stz	VERA_CTRL	; Set ADDRsel back to 0
@skip_vram_prep:
	ldx	@width		; Load width
@loop:	bit	@srcram
	bmi	@cpy_char_vram
	lda	(@srcptr)	; Copy char from sysram
	+INC16	@srcptr
	bra	@sto_char
@cpy_char_vram:
	lda	VERA_DATA1	; Copy char from VRAM
@sto_char:
	sta	VERA_DATA0	; Store char to screen
	bit	@srcram
	bmi	@cpy_col_vram
	lda	(@srcptr)	; Copy color from sysram
	+INC16	@srcptr
	bra	@sto_col
@cpy_col_vram:
	lda	VERA_DATA1	; Copy color from VRAM
@sto_col:
	sta	VERA_DATA0	; Store color to screen
@cont:	dex
	bne	@loop
	ldx	@width		; Restore width
	sty	VERA_ADDR_L	; Restore X coordinate
	inc	VERA_ADDR_M
	dec	@height
	bne	@loop
	rts

; *****************************************************************************
; Show a cursor and get a string input from keyboard.
; *****************************************************************************
; INPUTS:	r0 = pointer to buffer to hold string (must be pre-allocated)
;		.Y = maximum length of string
;		.X = color information for input characters
; OUPUTS:	.Y = actual length of input
; USES:		.A & r1l
; *****************************************************************************
vtui_input_str:
@ptr	= r0
@length	= r1l

	sty	@length		; Store maximum length

	lda	#$A0		; Show a "cursor"
	sta	VERA_DATA0
	stx	VERA_DATA0
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L

	ldy	#0
@inputloop:
	phx
	phy
	jsr	$FFE4		; Read keyboard input
	ply
	plx

	cmp	#$0D		; If RETURN has been pressed, we exit
	beq	@end
	cmp	#$14		; We need to handle backspace
	bne	@istext
	cpy	#0		; If .Y is 0, we can not delete
	beq	@inputloop
	; Here we need to handle backspace
	dey
	lda	#' '		; Delete cursor
	sta	VERA_DATA0

	lda	VERA_ADDR_L	; Go 2 chars back = 4 bytes
	sbc	#3
	sta	VERA_ADDR_L

	lda	#$A0		; Overwrite last char with cursor
	sta	VERA_DATA0

	dec	VERA_ADDR_L
	bra	@inputloop
@istext:
	cpy	@length
	beq	@inputloop	; If .Y = @length, we can not add character

	sta	(@ptr),y	; Store char in buffer
	cmp	#$20		; If < $20, we can not use it
	bcc	@inputloop
	cmp	#$40		; If < $40 & >= $20, screencode is equal to petscii
	bcc	@stvera
	cmp	#$60		; If > $60, we can not use it
	bcs	@inputloop
	sbc	#$3F		; When .A >= $40 & < $60, subtract $3F to get screencode
@stvera:
	sta	VERA_DATA0	; Write char to screen with colorcode
	stx	VERA_DATA0

	lda	#$A0		; Write cursor
	sta	VERA_DATA0
	stx	VERA_DATA0

	dec	VERA_ADDR_L	; Set VERA to point at cursor
	dec	VERA_ADDR_L
	iny			; Inc .Y to show a char has been added
	bra	@inputloop

@end:	lda	#' '
	sta	VERA_DATA0

	rts
