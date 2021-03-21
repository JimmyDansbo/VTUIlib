!cpu w65c02
*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810
!src "vtuilib-acme.inc"

	jmp	main

Buffer		!skip	10	; Buffer for 10 characters
Str		!pet	"name:"
StrLen		!byte	0

main:
	ldx	#(BLUE<<4)|WHITE	; Clear screen with blue background
	lda	#' '			; and white foreground
	jsr	vtui_clr_scr

	lda	#10			; Goto 10, 10
	ldy	#10
	+VTUI_GOTOXY

	lda	#18
	sta	r1l			; Width of box
	lda	#3
	sta	r2l			; Height of box
	lda	#3			; Border mode
	ldx	#(BLUE<<4)|WHITE	; Colorcode
	+VTUI_BORDER

	lda	#11			; Goto 11, 11
	ldy	#11
	jsr	vtui_gotoxy

	lda	#<Str			; Print Str
	sta	r0l
	lda	#>Str
	sta	r0h
	ldx	#(BLUE<<4)|WHITE	; Blue background, white foreground
	ldy	#StrLen-Str
	lda	#0			; Convert from PETSCII
	jsr	vtui_print_str

	ldy	#10			; Max length to get from user
	lda	#<Buffer		; Address of buffer to store string in
	sta	r0
	lda	#>Buffer
	sta	r0+1
	ldx	#(BLUE<<4)|WHITE	; Blue background, white foreground
	+VTUI_INPUT_STR			; Get a string from user
	sty	StrLen			; Store actual length of string

	lda	#20			; Goto 20, 20
	ldy	#20
	+VTUI_GOTOXY

	lda	StrLen			; Caculate box width from string length
	inc
	inc
	sta	r1l			; Store the width
	lda	#3
	sta	r2l			; Height of the box
	lda	#5
	ldx	#(BLUE<<4)|WHITE
	jsr	vtui_border		; Draw a border

	lda	#21			; Goto 21, 21 = inside the box
	ldy	#21
	+VTUI_GOTOXY

	lda	#<Buffer		; Print the string in the buffer
	sta	r0l
	lda	#>Buffer
	sta	r0h
	ldx	#(BLUE<<4)|WHITE	; Blue background, white foreground
	ldy	StrLen
	+VTUI_PRINT_STR

	rts
