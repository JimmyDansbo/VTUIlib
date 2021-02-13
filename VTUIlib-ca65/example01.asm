.org $080D
.segment "STARTUP"
.segment "INIT"
.segment "ONCE"
.segment "CODE"

.include "vtuilib-ca65.inc"

jmp	main

my_col:	.byte $62
my_bg:	.byte 6
my_fg:	.byte 1

width:	.byte 20
height:	.byte 10

my_char:	.byte 1

main:

	stz	xcord
	stz	ycord
	stz	newx
	stz	newy

;	lda	#1
	VTUI_SET_BANK #1
	stz	$9F20
	lda	#1
	sta	$9F21
	ldx	#64
	lda	#$20
	ldy	#$61
:	sta	$9F23
	sty	$9F23
	dex
	bne	:-
;	lda	#0
	VTUI_SET_BANK #0

;	lda	xcord
;	ldy	ycord
	VTUI_GOTOXY xcord, ycord
.byte $db
;	lda	#7
;	sta	x17l
;	sta	x17h
;	sec			; Use VRAM
;	lda	#1		; VRAM Bank 1
;	stz	x16l		; VRAM Addr $0000
;	stz	x16h
	VTUI_SAVE_RECT $10000, #7, #7, 1

;	lda	#32
;	ldy	#30
	VTUI_GOTOXY #32, #30

;	ldx	#$61
;	lda	#<my_str
;	sta	x16
;	lda	#>my_str
;	sta	x16+1
	VTUI_PRINT_STR my_str, #$61

:	jsr	$FFE4
	beq	:-
	cmp	#$9D		; Left arrow
	bne	@isup
	lda	newx
	beq	:-
	dec	newx
	jsr	move_logo
	bra	:-
@isup:	cmp	#$91		; Up arrow
	bne	@isright
	lda	newy
	beq	:-
	dec	newy
	jsr	move_logo
	bra	:-
@isright:
	cmp	#$1D		; Right arrow
	bne	@isdown
	lda	newx
	cmp	#73
	beq	:-
	inc	newx
	jsr	move_logo
	bra	:-
@isdown:
	cmp	#$11		; Down arrow
	bne	@end
	lda	newy
	cmp	#53
	beq	:-
	inc	newy
	jsr	move_logo
	bra	:-
@end:
	rts

move_logo:
;	lda	xcord
;	ldy	ycord
	VTUI_GOTOXY xcord, ycord

;	lda	#7
;	sta	x17l
;	sta	x17h
;	sec			; Use VRAM
;	lda	#1		; VRAM Bank 1
;	stz	x16l		; VRAM Addr $0100
;	sta	x16h
	VTUI_REST_RECT $10100, #7, #7, 1		; Restore screen

;	lda	newx
;	ldy	newy
	VTUI_GOTOXY newx, newy

;	lda	#7
;	sta	x17l
;	sta	x17h
;	sec			; Use VRAM
;	lda	#1		; VRAM Bank 1
;	stz	x16l		; VRAM Addr $0100
;	sta	x16h
	VTUI_SAVE_RECT	$10100, #7, #7, 1	; Save new screen

;	lda	newx
;	ldy	newy
	VTUI_GOTOXY newx, newy

;	lda	#7
;	sta	x17l
;	sta	x17h
;	sec			; Use VRAM
;	lda	#1		; VRAM Bank 1
;	stz	x16l		; VRAM Addr $0000
;	stz	x16h
	VTUI_REST_RECT $10000, #7, #7, 1		; Overwrite new with logo

	lda	newx
	sta	xcord
	lda	newy
	sta	ycord
	rts

my_str:	.byte	"use arrow keys!",0
xcord:	.byte	0
ycord:	.byte	0
newx:	.byte	0
newy:	.byte	0
