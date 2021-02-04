*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810
!src	"vtuilib-acme.inc"

;LIBSTART=$0400
LIBSTART=PRELIB+2

VTUI_initialize	= LIBSTART
VTUI_screen_set	= VTUI_initialize+2
VTUI_clear	= VTUI_screen_set+3
VTUI_set_stride	= VTUI_clear+3
VTUI_set_decr	= VTUI_set_stride+3
VTUI_gotoxy	= VTUI_set_decr+3
VTUI_plot_char	= VTUI_gotoxy+3
VTUI_scan_char	= VTUI_plot_char+3
VTUI_hline	= VTUI_scan_char+3
VTUI_vline	= VTUI_hline+3
VTUI_print_str	= VTUI_vline+3
VTUI_fill_box	= VTUI_print_str+3
VTUI_pet2scr	= VTUI_fill_box+3
VTUI_scr2pet	= VTUI_pet2scr+3
VTUI_border	= VTUI_scr2pet+3

main:
;	jsr	load_library	; Load the library

	jsr	VTUI_initialize	; Initialize jumptable in library

	lda	#$10
	jsr	VTUI_clear

	ldy	#10
	lda	#10
	jsr	VTUI_gotoxy

	lda	#4
	sta	x17l
	sta	x17h
	ldx	#$12
	lda	#5
	jsr	VTUI_border

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
	!byte	0

PRELIB	!bin	"VTUI.BIN"
