!cpu w65c02
*=$0000
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

	bra	initialize
	jmp	screen_set
	jmp	clear
	jmp	set_stride
	jmp	set_decr
	jmp	hline
	jmp	vline
	jmp	gotoxy
	jmp	print_str
	jmp	fill_box
	jmp	$0000		; Show that there are no more jumps

; *****************************************************************************
; Initialize the jumptable with correct address calculated from the address
; where this code is loaded.
; *****************************************************************************
; USES:		.A, .X & .Y
;		x16, x17, x18 & x19 (ZP addresses $22-$29)
; *****************************************************************************
initialize:
	lda	#$BA		; TSX
	sta	x16
	lda	#$BD		; LDA absolute,x
	sta	x16+1
	lda	#$01		; $0101
	sta	x16+2
	sta	x16+3
	lda	#$BC		; LDY absolute,x
	sta	x16+4
	lda	#$02		; $0102
	sta	x16+5
	lda	#$01
	sta	x16+6
	lda	#$60		; RTS
	sta	x16+7
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
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	x16l	= X coordinate
;		.A	= Y coordinate
;		x16h	= Character to use for drawing the line
;		.Y	= Length of the line
;		.X	= bg- & fg-color
; *****************************************************************************
hline:
	sta	VERA_ADDR_M	; Set y coordinate
	lda	x16l		; Load x coordinate
	asl			; Multiply by 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	lda	x16h
@loop:	sta	VERA_DATA0	; Write character
	stx	VERA_DATA0	; Write bg-/fg-color
	dey
	bne	@loop
	rts

; *****************************************************************************
; Create a vertical line going from top to bottom.
; *****************************************************************************
; INPUTS:	x16l	= X coordinate
;		.A	= Y coordinate
;		x16h	= Character to use for drawing the line
;		.Y	= Height of the line
;		.X	= bg- & fg-color
; *****************************************************************************
vline:
	sta	VERA_ADDR_M	; Set y coordinate
	lda	x16l		; Load x coordinate
	asl			; Multiply by 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	lda	x16h
@loop:	sta	VERA_DATA0	; Write character
	stx	VERA_DATA0	; Write bg-/fg-color
	dec	VERA_ADDR_L	; Return to original X coordinate
	dec	VERA_ADDR_L
	inc	VERA_ADDR_M	; Increment Y coordinate
	dey
	bne	@loop
	rts

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	.A = x coordinate
;		.Y = y coordinate
; *****************************************************************************
gotoxy:
	sty	VERA_ADDR_M	; Set y coordinate
	asl			; Multiply x coord with 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	rts

; *****************************************************************************
; Print a 0 terminated string
; *****************************************************************************
; INPUTS	x16 = pointer to string
;		.Y  = bg-/fg color
; USES:		.X
; *****************************************************************************
print_str:
	lda	(x16),y		; Load character
	beq	@end		; If 0, we are done
	sta	VERA_DATA0	; Write character
	sty	VERA_DATA0	; Write bg-/fg-color
	iny
	bne	print_str	; Get next character
@end:	rts

; *****************************************************************************
; Create a filled box drawn from top left to bottom right
; *****************************************************************************
; INPUTS:	x16l	= X coordinate of upper left corner
;		.A	= Y coordinate of upper left corner
;		x16h	= Character to use for drawing the line
;		x17l	= Width of box
;		x17h	= Height of box
;		.X	= bg- & fg-color
; *****************************************************************************
fill_box:
	sta	VERA_ADDR_M	; Set y coordinate
@vloop:	lda	x16l		; Load x coordinate
	asl			; Multiply by 2 for correct coordinate
	sta	VERA_ADDR_L	; Set x coordinate
	lda	x16h
	ldy	x17l
@hloop:	sta	VERA_DATA0	; Write character
	stx	VERA_DATA0	; Write bg-/fg-color
	dey
	bne	@hloop
	dec	x17h
	bne	@vloop
	rts
