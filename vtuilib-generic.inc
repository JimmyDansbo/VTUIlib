!cpu w65c02
*=0
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

	bra	screen_set
	bra	set_stride
	bra	set_decr
	bra	hline
	bra	vline
	bra	fill_box

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

fill_box:
	rts
