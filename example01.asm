*=$0801
!byte $0C,$08,$0A,$00,$9E,' ','2','0','6','4',$00,$00,$00
*=$0810
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

; Load library into Goldem RAM using standard CBM Kernal API
LIBSTART=$0400

; Library references
VTUI_initialize	= LIBSTART+0
VTUI_screen_set	= LIBSTART+2
VTUI_set_bank	= LIBSTART+5
VTUI_set_stride	= LIBSTART+8
VTUI_set_decr	= LIBSTART+11
VTUI_gotoxy	= LIBSTART+14
VTUI_plot_char	= LIBSTART+17
VTUI_scan_char	= LIBSTART+20
VTUI_hline	= LIBSTART+23
VTUI_vline	= LIBSTART+26
VTUI_print_str	= LIBSTART+29
VTUI_fill_box	= LIBSTART+32
VTUI_pet2scr	= LIBSTART+35
VTUI_scr2pet	= LIBSTART+38
VTUI_border	= LIBSTART+41

main:
	; Load the library using standard Kernal functions
	jsr	load_library
	; Initialize the library
	jsr	VTUI_initialize
	; Set screen mode to 40x30
	lda	#$00
	jsr	VTUI_screen_set
	; Set stride to 1, most functions rely on stride being 1
	lda	#$01
	jsr	VTUI_set_stride
	; Set decrement to 0, most functions rely on VERA incrementing
	clc
	jsr	VTUI_set_decr
	; Clear screen with white background and black foreground
	lda	#' '
	sta	x16h
	lda	#80
	sta	x17l
	lda	#60
	sta	x17h
	ldx	#$10
	jsr	VTUI_fill_box
	; gotoxy 11, 2
	lda	#11
	ldy	#2
	jsr	VTUI_gotoxy
	; Write a blue heart on screen
	ldx	#$16		; Blue on white
	lda	#$53		; Character to print
	jsr	VTUI_plot_char
	; gotoxy 28, 2
	lda	#28
	ldy	#2
	jsr	VTUI_gotoxy
	; Write a blue heart on screen
	ldx	#$16		; Blue on white
	lda	#$53		; Character to print
	jsr	VTUI_plot_char
	; gotoxy 14, 3
	lda	#14
	ldy	#3
	jsr	VTUI_gotoxy
	; Print string
	lda	#<Libname	; Low byte of string start address
	sta	x16l
	lda	#>Libname	; High byte of string start address
	sta	x16h
	ldx	#$10		; White background / black foreground
	jsr	VTUI_print_str
	; gotoxy 11, 5
	lda	#11
	ldy	#5
	jsr	VTUI_gotoxy
	; Draw horizontal line
	ldy	#18		; Line length
	ldx	#$20		; Red background / black foreground
	lda	#$3D		; Character to use for drawing line
	jsr	VTUI_hline
	; gotoxy 3, 9
	lda	#3
	ldy	#9
	jsr	VTUI_gotoxy
	; Draw a border
	lda	#34
	sta	x17l		; Width
	lda	#18
	sta	x17h		; Height
	lda	#3		; Bordermode
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_border
	; gotoxy 4, 10
	lda	#4
	ldy	#10
	jsr	VTUI_gotoxy
	; Draw a filled box
	lda	#32
	sta	x17l		; Width
	lda	#16
	sta	x17h		; Height
	ldx	#$74		; Yellow background, purple foreground
	lda	#' '
	sta	x16h		; Character used for filling box
	jsr	VTUI_fill_box
	; gotoxy 13, 9
	lda	#13
	ldy	#9
	jsr	VTUI_gotoxy
	; Plot a character to create a header for our box
	lda	#$73		; Character
	ldx	#$74		; Color
	jsr	VTUI_plot_char
	; Print string to create a header for our box
	lda	#<Verstr	; Low byte of string start address
	sta	x16l
	lda	#>Verstr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str
	; Plot a character to create a header for our box
	lda	#$6B		; Character
	ldx	#$74		; Color
	jsr	VTUI_plot_char
	; gotoxy 3, 17
	lda	#3
	ldy	#17
	jsr	VTUI_gotoxy
	; Plot a character to create a dividing line in the box
	lda	#$6B		; Character
	ldx	#$74		; Color
	jsr	VTUI_plot_char
	; Draw horizontal line to create a dividing line in the box
	ldy	#32		; Line length
	ldx	#$74		; Yellow background, purple foreground
	lda	#$43		; Character to use for drawing line
	jsr	VTUI_hline
	; Plot a character to create a dividing line in the box
	lda	#$73		; Character
	ldx	#$74		; Color
	jsr	VTUI_plot_char

	; gotoxy 5, 11
	lda	#5
	ldy	#11
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Boxstr	; Low byte of string start address
	sta	x16l
	lda	#>Boxstr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	; gotoxy 5, 13
	lda	#5
	ldy	#13
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Hlinestr	; Low byte of string start address
	sta	x16l
	lda	#>Hlinestr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	; gotoxy 5, 15
	lda	#5
	ldy	#15
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Vlinestr	; Low byte of string start address
	sta	x16l
	lda	#>Vlinestr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	; gotoxy 5, 19
	lda	#5
	ldy	#19
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Plotstr	; Low byte of string start address
	sta	x16l
	lda	#>Plotstr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	; gotoxy 5, 21
	lda	#5
	ldy	#21
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Dramstr	; Low byte of string start address
	sta	x16l
	lda	#>Dramstr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	; gotoxy 5, 23
	lda	#5
	ldy	#23
	jsr	VTUI_gotoxy
	; Print string with info about boxes
	lda	#<Morestr	; Low byte of string start address
	sta	x16l
	lda	#>Morestr	; High byte of string start address
	sta	x16h
	ldx	#$74		; Yellow background, purple foreground
	jsr	VTUI_print_str

	jsr	$FFCF		; Wait for enter key
	rts

; Load library using standard Kernal functions
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

Libname		!text	"VTUI LIBRARY",0
Verstr		!text	"VERSION 0.2",0
Boxstr		!text	"BOXES WITH OR WITHOUT BORDERS",0
Hlinestr	!text	"HORIZONTAL LINES",0
Vlinestr	!text	"VERTICAL LINES",0
Plotstr		!text	"PLOT OR SCAN CHARACTERS",0
Dramstr		!text	"DIRECTLY TO/FROM SCREEN RAM",0
Morestr		!text	"***MORE TO SEE AND TO COME***",0

Fname		!text	"VTUI0.2.BIN"
End_fname
