*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810

;!src	"vera-tui-acme.inc"

main:!byte $db
	lda	#2
	jsr	$0402		;set stride
	lda	#1
	jsr	$0402		;set stride
	lda	#1
	jsr	$0404		;set decr
	lda	#0
	jsr	$0404		;set decr

	lda	#5
	sta	$22		; x coordinate
	lda	#'.'
	sta	$23		; character
	lda	#2		; y coordinate
	ldy	#10		;length
	ldx	#$01		; bg-/fg color
	jsr	$0408		; vline
;	jsr	$0406		; hline
	rts
