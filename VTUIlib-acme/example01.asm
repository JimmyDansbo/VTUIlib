!cpu w65c02
*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810
!src "vtuilib-acme.inc"

main:
	stz	xcord
	stz	ycord
	stz	newx
	stz	newy

	+VTUI_SET_BANK 1
	stz	$9F20
	lda	#1
	sta	$9F21
	ldx	#64
	lda	#' '
	ldy	#$61
-	sta	$9F23
	sty	$9F23
	dex
	bne	-
	+VTUI_SET_BANK 0

	+VTUI_GOTOXY ~xcord, ~ycord
	+VTUI_SAVE_RECT $10000, 7, 7, 1

	+VTUI_GOTOXY 32, 30
	+VTUI_PRINT_STR my_str, $61

-	jsr	$FFE4
	beq	-
	cmp	#$9D		; Left arrow
	bne	@isup
	lda	newx
	beq	-
	dec	newx
	jsr	move_logo
	bra	-
@isup:	cmp	#$91		; Up arrow
	bne	@isright
	lda	newy
	beq	-
	dec	newy
	jsr	move_logo
	bra	-
@isright:
	cmp	#$1D		; Right arrow
	bne	@isdown
	lda	newx
	cmp	#73
	beq	-
	inc	newx
	jsr	move_logo
	bra	-
@isdown:
	cmp	#$11		; Down arrow
	bne	@end
	lda	newy
	cmp	#53
	beq	-
	inc	newy
	jsr	move_logo
	bra	-
@end:
	rts

move_logo:
	+VTUI_GOTOXY ~xcord, ~ycord
	+VTUI_REST_RECT $10100, 7, 7, 1		; Restore screen

	+VTUI_GOTOXY ~newx, ~newy
	+VTUI_SAVE_RECT $10100, 7, 7, 1		; Save new screen

	+VTUI_GOTOXY ~newx, ~newy
	+VTUI_REST_RECT $10000, 7, 7, 1		; Overwrite new with logo

	lda	newx
	sta	xcord
	lda	newy
	sta	ycord
	rts

my_str	!text	"USE ARROW KEYS!",0
xcord	!byte	0
ycord	!byte	0
newx	!byte	0
newy	!byte	0
