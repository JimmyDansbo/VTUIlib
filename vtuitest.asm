*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810

LIBSTART=$0400

VTUI_initialize	= LIBSTART
VTUI_screen_set	= LIBSTART+2
VTUI_clear	= VTUI_screen_set+3
VTUI_set_stride	= VTUI_clear+3
VTUI_set_decr	= VTUI_set_stride+3
VTUI_hline	= VTUI_set_decr+3
VTUI_vline	= VTUI_hline+3
VTUI_gotoxy	= VTUI_vline+3
VTUI_print_str	= VTUI_gotoxy+3
VTUI_fill_box	= VTUI_print_str+3

main:
	jsr	load_library	; Load the library

	jsr	VTUI_initialize	; Initialize jumptable in library

	lda	#$10		; White background / black text
	jsr	VTUI_clear	; Clear screen
	rts

load_library:
	lda	#1		; Logical file number (must be unique)
	ldx	#8		; Device number (8 local filesystem)
	ldy	#0		; Secondary command 0 = dont use addr in file
	jsr	$FFBA		; SETLFS
	lda	#(End_fname-Fname)	; Length of filename
	ldx	#<Fname		; Address of filename
	ldy	#>Fname
	jsr	$FFBD		; SETNAM
	lda	#0		; 0=load, 1=verify
	ldx	#<LIBSTART	; Load file to LIBSTART
	ldy	#>LIBSTART
	jsr	$FFD5		; LOAD
	rts

Fname	!text	"VTUI.BIN"
End_fname
