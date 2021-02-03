!cpu w65c02
; Program counter is set to 0 to make it easier to calculate the addresses
; in the jumptable as all that needs to be done is add the actual offset.
*=$0000

; ******************************* Jumptable *******************************
	bra	initialize	; No inputs
	jmp	screen_set	; .A = Screenmode ($00, $02 or $FF)
	jmp	clear		; .A = bg-/fg-color
	jmp	set_stride	; .A = Stride value
	jmp	set_decr	; .A (1 = decrement, 0 = increment)
	jmp	gotoxy		; .A = x coordinate, .Y = y coordinate
	jmp	plot_char	; .A = character, .X = bg-/fg-color
	jmp	scan_char	; like plot_char
	jmp	hline		; x16l	= Character, .Y = length, .X = color
	jmp	vline		; x16l	= Character, .Y = height, .X = color
	jmp	print_str	; x16 = pointer to string, .X = color
	jmp	fill_box	; x16h=Char,x17l=width,x17h=height,.X=color
	jmp	pet2scr		; .A = character to convert to screencode
	jmp	scr2pet		; .A = character to convert to petscii
	jmp	border		; .A=border,x17l=width,x17h=height,.X=color
;	jmp	save_rect	;
;	jmp	rest_rect	;
	jmp	$0000		; Show that there are no more jumps

; ******************************* Constants *******************************
VERA_ADDR_L		= $9F20
VERA_ADDR_M		= $9F21
VERA_ADDR_H		= $9F22
VERA_DATA0		= $9F23
VERA_DATA1		= $9F24
VERA_CTRL		= $9F25
VERA_IEN		= $9F26
VERA_ISR		= $9F27
VERA_IRQLINE_L		= $9F28
VERA_DC_VIDEO		= $9F29		; DCSEL=0
VERA_DC_HSCALE		= $9F2A		; DCSEL=0
VERA_DC_VSCALE		= $9F2B		; DCSEL=0
VERA_DC_BORDER		= $9F2C		; DCSEL=0
VERA_DC_HSTART		= $9F29		; DCSEL=1
VERA_DC_VSTART		= $9F2A		; DCSEL=1
VERA_DC_HSTOP		= $9F2B		; DCSEL=1
VERA_DC_VSTOP		= $9F2C		; DCSEL=1
VERA_L0_CONFIG		= $9F2D
VERA_L0_MAPBASE		= $9F2E
VERA_L0_TILEBASE	= $9F2F
VERA_L0_HSCROLL_L	= $9F30
VERA_L0_HSCROLL_H	= $9F31
VERA_L0_VSCROLL_L	= $9F32
VERA_L0_VSCROLL_H	= $9F33
VERA_L1_CONFIG		= $9F34
VERA_L1_MAPBASE		= $9F35
VERA_L1_TILEBASE	= $9F36
VERA_L1_HSCROLL_L	= $9F37
VERA_L1_HSCROLL_H	= $9F38
VERA_L1_VSCROLL_L	= $9F39
VERA_L1_VSCROLL_H	= $9F3A

x16		= $22
x16l		= x16
x16h		= x16+1
x17		= $24
x17l		= x17
x17h		= x17+1
x18		= $26
x18l		= x18
x18h		= x18+1
x19		= $28
x19l		= x19
x19h		= x19+1

; ******************************* Macros *******************************

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	.A = x coordinate
;		.Y = y coordinate
; *****************************************************************************
!macro GOTOXY {
	sty	VERA_ADDR_M	; Set y coordinate
	asl			; Multiply x coord with 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
}
; *****************************************************************************
; Convert PETSCII codes between $20 and $5F to screencodes.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $56 if invalid input
; *****************************************************************************
!macro PET2SCR {
	cmp	#$20
	bcc	.nonprintable	; .A < $20
	cmp	#$40
	bcc	.end		; .A < $40 means screen code is the same
	; .A >= $40 - might be letter
	cmp	#$60
	bcs	.nonprintable	; .A < $60 so it is a letter, subtract ($3F+1)
	sbc	#$3F		; to convert to screencode
	bra	.end
.nonprintable:
	lda	#$56
.end:
}
; *****************************************************************************
; Convert screencodes between $00 and $3F to PETSCII.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $76 if invalid input
; *****************************************************************************
!macro SCR2PET {
	cmp	#$40
	bcs	.nonprintable	; .A >= $40
	cmp	#$20
	bcs	.end		; .A >=$20 & < $40 means petscii is the same
	; .A < $20 and is a letter
	adc	#$40
	bra	.end
.nonprintable:
	lda	#$76
.end:
}
; *****************************************************************************
; Create a vertical line going from top to bottom.
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Height of the line
;		.X	= bg- & fg-color
; *****************************************************************************
!macro VLINE {
.loop:	+PLOT_CHAR
	dec	VERA_ADDR_L	; Return to original X coordinate
	dec	VERA_ADDR_L
	inc	VERA_ADDR_M	; Increment Y coordinate
	dey
	bne	.loop
}
; *****************************************************************************
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	.A	= Character to use for drawing the line
;		.Y	= Length of the line
;		.X	= bg- & fg-color
; *****************************************************************************
!macro HLINE {
.loop:	+PLOT_CHAR
	dey
	bne	.loop
}
; *****************************************************************************
; Write character and color to current VERA address
; Function assumes that stride is set to 1 and decrement set to 0
; *****************************************************************************
; INPUTS:	.A = character
;		.X = bg-/fg-color
; *****************************************************************************
!macro PLOT_CHAR {
	sta	VERA_DATA0
	stx	VERA_DATA0
}
; *****************************************************************************
; Read character and color from current VERA address
; Function assumes that stride is set to 1 and decrement set to 0
; *****************************************************************************
; OUTPUS:	.A = character
;		.X = bg-/fg-color
; *****************************************************************************
!macro SCAN_CHAR {
	lda	VERA_DATA0
	ldx	VERA_DATA0
}

; ******************************* Functions *******************************

; *****************************************************************************
; Initialize the jumptable with correct addresses calculated from the address
; where this code is loaded.
; *****************************************************************************
; USES:		.A, .X & .Y
;		x16, x17, x18 & x19 (ZP addresses $22-$29)
; *****************************************************************************
initialize:
	; Write code to ZP to figure out where the library is loaded.
	; This is done by jsr'ing to the code in ZP which in turn reads the
	; return address from the stack.
	lda	#$BA		; TSX
	sta	x16
	lda	#$BD		; LDA absolute,x
	sta	x16+1
	lda	#$01		; $0101
	sta	x16+2
	sta	x16+3
	sta	x16+6
	lda	#$BC		; LDY absolute,x
	sta	x16+4
	lda	#$02		; $0102
	sta	x16+5
	lda	#$60		; RTS
	sta	x16+7
	; Jump to the code in ZP that was just copied there by the code above.
	; This is to get the return address stored on stack
	jsr	x16		; Get current PC value
	sec
	sbc	#*-2		; Calculate start of our program
	sta	x16		; And store it in x16
	tya
	sbc	#$00
	sta	x16+1
	lda	x16		; Calculate location of first address in
	clc			; jump table
	adc	#$03
	sta	x17
	lda	x16+1
	adc	#$00
	sta	x17+1
	ldy	#$01		; .Y used for indexing high byte of pointers
@loop:	clc
	lda	(x17)		; Low part of jumptable address
	beq	@mightend	; If it is zero, we might have reaced the end of jumptable
	adc	x16l		; Add start address of our program to the jumptable address
	sta	(x17)
	lda	(x17),y
	adc	x16h
	sta	(x17),y
	bra	@prepnext
@mightend:
	adc	x16l		; Add start address of our program to the jumptable address
	sta	(x17)
	lda	(x17),y		; High part of jumptable address
	beq	@end		; If it is zero, we have reaced end of jumptable
	adc	x16h
	sta	(x17),y
@prepnext:			; Prepare x17 pointer for next entry in jumptable
	clc			; (add 3 to current value)
	lda	x17l
	adc	#$03
	sta	x17l
	lda	x17h
	adc	#$00
	sta	x17h
	bra	@loop
@end:	rts

; *****************************************************************************
; Use KERNAL API to set screen to 80x60 or 40x30 or swap between them.
; *****************************************************************************
; INPUT:		.A = Screenmode ($00, $02 or $FF)
; USES:			.A, .X & ,Y
; RETURNS:		.C = 1 in case of error.
; *****************************************************************************
screen_set:
	beq	@doset		; If 0, we can set mode
	cmp	#$02
	beq	@doset		; If 2, we can set mode
	cmp	#$FF
	bne	@end		; If $FF, we can set mode
@doset:	jsr	$FF5F
@end:	rts

; *****************************************************************************
; Clear the screen with certain bg-/fg-color
; Assumes stride is 1 and decr is 0
; *****************************************************************************
; INPUTS:	.A = bg-/fg-color
; USES:		.X & .Y
; *****************************************************************************
clear:
	ldy	#60		; 60 lines is the maximum
	sty	VERA_ADDR_M
	ldy	#' '
@yloop:	ldx	#80		; 80 columns is maximum
	stz	VERA_ADDR_L
@xloop:	sty	VERA_DATA0	; - Character
	sta	VERA_DATA0	; - bg-/fg-color
	dex
	bne	@xloop
	dec	VERA_ADDR_M
	bpl	@yloop
	rts

; *****************************************************************************
; Set the stride without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.A = Stride value
; USES:			.A
; *****************************************************************************
set_stride:
	asl			; Stride is stored in upper nibble
	asl
	asl
	asl
	tax
	lda	VERA_ADDR_H	; Set stride value to 0 in VERA_ADDR_H
	and	#$0F
	sta	VERA_ADDR_H
	txa
	ora	VERA_ADDR_H	; Set the correct stride value
	sta	VERA_ADDR_H
	rts

; *****************************************************************************
; Set the decrement value without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUT:		.A (1 = decrement, 0 = increment)
; USES:			.A
; *****************************************************************************
set_decr:
	beq	@setnul
	lda	VERA_ADDR_H
	ora	#%00001000
	bra	@end
@setnul:
	lda	VERA_ADDR_H
	and	#%11110111
@end:	sta	VERA_ADDR_H
	rts

; *****************************************************************************
; Write character and color to current VERA address
; Function assumes that stride is set to 1 and decrement set to 0
; *****************************************************************************
; INPUTS:	.A = character
;		.X = bg-/fg-color
; *****************************************************************************
plot_char:
	+PLOT_CHAR
	rts

; *****************************************************************************
; Read character and color from current VERA address
; Function assumes that stride is set to 1 and decrement set to 0
; *****************************************************************************
; OUTPUS:	.A = character
;		.X = bg-/fg-color
; *****************************************************************************
scan_char:
	+SCAN_CHAR
	rts

; *****************************************************************************
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	x16l	= Character to use for drawing the line
;		.Y	= Length of the line
;		.X	= bg- & fg-color
; *****************************************************************************
hline:
	lda	x16l
	+HLINE
	rts

; *****************************************************************************
; Create a vertical line going from top to bottom.
; *****************************************************************************
; INPUTS:	x16l	= Character to use for drawing the line
;		.Y	= Height of the line
;		.X	= bg- & fg-color
; *****************************************************************************
vline:
	lda	x16l
	+VLINE
	rts

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	.A = x coordinate
;		.Y = y coordinate
; *****************************************************************************
gotoxy:
	+GOTOXY
	rts

; *****************************************************************************
; Convert PETSCII codes between $20 and $5F to screencodes.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $56 if invalid input
; *****************************************************************************
pet2scr:
	+PET2SCR
	rts

; *****************************************************************************
; Convert screencodes between $00 and $3F to PETSCII.
; *****************************************************************************
; INPUTS:	.A = character to convert
; OUTPUS:	.A = converted character or $76 if invalid input
; *****************************************************************************
scr2pet:
	+SCR2PET
	rts

; *****************************************************************************
; Print a 0 terminated string PETSCII string
; *****************************************************************************
; INPUTS	x16 = pointer to string
;		.X  = bg-/fg color
; USES:		.Y
; *****************************************************************************
print_str:
	ldy	#0
@loop:	lda	(x16),y		; Load character
	beq	@end		; If 0, we are done
	+PET2SCR
	+PLOT_CHAR
	iny
	bne	@loop		; Get next character
@end:	rts

; *****************************************************************************
; Create a filled box drawn from top left to bottom right
; *****************************************************************************
; INPUTS:	x16h	= Character to use for drawing the line
;		x17l	= Width of box
;		x17h	= Height of box
;		.X	= bg- & fg-color
; *****************************************************************************
fill_box:
	lda	VERA_ADDR_L
	sta	x16l
@vloop:	lda	x16l		; Load x coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	lda	x16h
	ldy	x17l
@hloop:	+PLOT_CHAR
	dey
	bne	@hloop
	inc	VERA_ADDR_M
	dec	x17h
	bne	@vloop
	rts

; *****************************************************************************
; Create a box with a specific border
; *****************************************************************************
; INPUTS:	.A	= Border mode (0-5) any other will default to mode 0
;		x17l	= width
;		x17h	= height
;		.X	= bg-/fg-color
; USES		.Y, x16l & x16h
; *****************************************************************************
border:
	; Define local variable names for ZP variables
	; Makes the source a bit more readable
@top_right=x18l
@top_left =x18h
@bot_right=x19l
@bot_left =x19h
@top	  =x19h+1		; z20l
@bottom   =x19h+2		; z20h
@left	  =x19h+3		; z21l
@right	  =x19h+4		; z21h

	; Set the border drawing characters according to the border mode in .A
@mode1: cmp	#1
	bne	@mode2
	lda	#$66
	bra	@def
@mode2: cmp	#2
	bne	@mode3
	lda	#$6E
	sta	@top_right
	lda	#$70
	sta	@top_left
	lda	#$7D
	sta	@bot_right
	lda	#$6D
	sta	@bot_left
@clines	lda	#$40		; centered lines
	sta	@top
	sta	@bottom
	lda	#$42
	sta	@left
	sta	@right
	bra	@dodraw
@mode3	cmp	#3
	bne	@mode4
	lda	#$49
	sta	@top_right
	lda	#$55
	sta	@top_left
	lda	#$4B
	sta	@bot_right
	lda	#$4A
	sta	@bot_left
	bra	@clines
@mode4	cmp	#4
	bne	@mode5
	lda	#$50
	sta	@top_right
	lda	#$4F
	sta	@top_left
	lda	#$7A
	sta	@bot_right
	lda	#$4C
	sta	@bot_left
@elines	lda	#$77		; lines on edges
	sta	@top
	lda	#$6F
	sta	@bottom
	lda	#$74
	sta	@left
	lda	#$6A
	sta	@right
	bra	@dodraw
@mode5	cmp	#5
	bne	@default
	lda	#$5F
	sta	@top_right
	lda	#$69
	sta	@top_left
	lda	#$E9
	sta	@bot_right
	lda	#$DF
	sta	@bot_left
	bra	@elines
@default:
	lda	#$20
@def	sta	@top_right
	sta	@top_left
	sta	@bot_right
	sta	@bot_left
	sta	@top
	sta	@bottom
	sta	@left
	sta	@right
@dodraw:
	; Save initial position
	lda	VERA_ADDR_L
	sta	x16l
	lda	VERA_ADDR_M
	sta	x16h

	ldy	x17l		; width
	dey
	lda	@top_left
	+PLOT_CHAR		; Top left corner
	dey
	lda	@top
	+HLINE			; Top line
	lda	@top_right
	+PLOT_CHAR		; Top right corner
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L
	inc	VERA_ADDR_M
	ldy	x17h		;height
	dey
	dey
	lda	@right
	+VLINE			; Right line
	; Restore initial VERA address
	lda	x16l
	sta	VERA_ADDR_L
	lda	x16h
	sta	VERA_ADDR_M
	inc	VERA_ADDR_M
	ldy	x17h		;height
	dey
	lda	@left
	+VLINE			; Left line
	dec	VERA_ADDR_M
	lda	@bot_left
	+PLOT_CHAR		; Bottom left corner
	ldy	x17l
	dey
	lda	@bottom
	+HLINE			; Bottom line
	dec	VERA_ADDR_L
	dec	VERA_ADDR_L
	lda	@bot_right
	+PLOT_CHAR		; Bottom right corner
	rts
