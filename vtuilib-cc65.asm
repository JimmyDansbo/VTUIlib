.import popa
;.import sp					; Keeping these here for reference if I ever need them.
;.import sreg

.export _vtui_load
.export _vtui_initialize
.export _vtui_screen_set
.export _vtui_set_bank
.export _vtui_set_stride
.export _vtui_set_decr
.export _vtui_clr_scr
.export _vtui_gotoxy
.export _vtui_plot_char
.export _vtui_scan_char
.export _vtui_hline
.export _vtui_vline
.export _vtui_print_str
.export _vtui_fill_box
.export _vtui_pet2scr
.export _vtui_scr2pet
.export _vtui_border
.export _vtui_save_rect
.export _vtui_rest_rect
.export _vtui_input_str

r0		= $02
r0l		= r0
r0h		= r0+1
r1		= $04
r1l		= r1
r1h		= r1+1
r2		= $06
r2l		= r2
r2h		= r2+1
r3		= $08
r3l		= r3
r3h		= r3+1
r4		= $0A
r4l		= r4
r4h		= r4+1
r5		= $0C
r5l		= r5
r5h		= r5+1
r6		= $0E
r6l		= r6
r6h		= r6+1
r7		= $10
r7l		= r7
r7h		= r7+1
r8		= $12
r8l		= r8
r8h		= r8+1
r9		= $14
r9l		= r9
r9h		= r9+1
r10		= $16
r10l		= r10
r10h		= r10+1
r11		= $18
r11l		= r11
r11h		= r11+1
r12		= $1A
r12l		= r12
r12h		= r12+1

.segment "CODE"

_vtui_load:
	; X = high and A = low part of address where library should be loaded
	; Initialize the jump-table at the end of the file with correct addresses
	sta	vtui_initialize+1
	stx	vtui_initialize+2
	clc				; Add 2 to jump-address as first one in library is branch
	adc	#2
	bcc :+
	inx
:	ldy	#3			; Use .Y for indexing into jump table below
:	sta	vtui_initialize+1,y	; store low value
	txa
	sta	vtui_initialize+2,y	; store high value
	lda	vtui_initialize+1,y	; read low value into .A again
	clc				; Add 3 to jump-address
	adc	#3
	bcc :+
	inx
:	iny				; Add 3 to .Y indexing
	iny
	iny
	cpy #57				; Ensure we have gone through all addresses below
	bne	:--

	; Now use kernal function to load the actual library to memory
	lda	#1			; Logical file number (must be unique)
	ldx	#8			; Device number (8 local filesystem)
	ldy	#0			; Secondary command 0 = dont use addr in file
	jsr	$FFBA			; SETLFS
	lda	#(End_fname-Fname)	; Length of filename
	ldx	#<Fname			; Address of filename
	ldy	#>Fname
	jsr	$FFBD			; SETNAM
	lda	#0			; 0=load, 1=verify
	ldx	vtui_initialize+1
	ldy	vtui_initialize+2
	jmp	$FFD5			; LOAD

_vtui_initialize:
	; Library is designed to work with standard PETSCII character set, but
	; CC65 switches to lower/upper case character set.
	; The two lines below would switch back to standard PETSCII
;	lda	#$8E			; Uppercase
;	jsr	$FFD2
	jmp	vtui_initialize
_vtui_screen_set:
	jsr	vtui_screen_set
	ldx	#0
	lda	#0
	rol
	eor	#1
	rts
_vtui_set_bank:
	ror
	jmp	vtui_set_bank
_vtui_set_stride:
	jmp	vtui_set_stride
_vtui_set_decr:
	ror
	jmp	vtui_set_decr
_vtui_clr_scr:
	tax
	jsr	popa
	jmp	vtui_clr_scr
_vtui_gotoxy:
	tay
	jsr	popa
	jmp	vtui_gotoxy
_vtui_plot_char:
	tax
	jsr	popa
	jmp	vtui_plot_char
_vtui_scan_char:
	jmp	vtui_scan_char
_vtui_hline:
	tax
	jsr	popa
	tay
	jsr	popa
	jmp	vtui_hline
_vtui_vline:
	tax
	jsr	popa
	tay
	jsr	popa
	jmp	vtui_vline
_vtui_print_str:
	pha				; Store value on stack while fetching other parameters
	jsr	popa
	tax				; Color
	jsr	popa
	tay				; Length
	jsr	popa
	sta	r0			; String pointer low byte
	jsr	popa
	sta	r0+1			; String pointer high byte
	pla				; Conversion value from normal stack
	jmp	vtui_print_str
_vtui_fill_box:
	tax
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	jmp	vtui_fill_box
_vtui_pet2scr:
	jsr	vtui_pet2scr
	ldx	#0
	rts
_vtui_scr2pet:
	jsr	vtui_scr2pet
	ldx	#0
	rts
_vtui_border:
	tax
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	jmp	vtui_border
_vtui_save_rect:
	tay				; VRAM bank
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	pha				; Destination RAM
	jsr	popa
	sta	r0l
	jsr	popa
	sta	r0h
	tya
	ror				; Set .C = VRAM bank
	pla
	jmp	vtui_save_rect
_vtui_rest_rect:
	tay				; VRAM bank
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	pha				; Destination RAM
	jsr	popa
	sta	r0l
	jsr	popa
	sta	r0h
	tya
	ror				; Set .C = VRAM bank
	pla
	jmp	vtui_rest_rect
_vtui_input_str:
	tax
	jsr	popa
	tay
	jsr	popa
	sta	r0l
	jsr	popa
	sta	r0h
	jsr	vtui_input_str
	tya
	ldx	#0
	rts

vtui_initialize:
	jmp	$0000		; +1
vtui_screen_set:
	jmp	$0000		; +4
vtui_set_bank:
	jmp	$0000		; +7
vtui_set_stride:
	jmp	$0000		; +10
vtui_set_decr:
	jmp	$0000		; +13
vtui_clr_scr:
	jmp	$0000		; +16
vtui_gotoxy:
	jmp	$0000		; +19
vtui_plot_char:
	jmp	$0000		; +22
vtui_scan_char:
	jmp	$0000		; +25
vtui_hline:
	jmp	$0000		; +28
vtui_vline:
	jmp	$0000		; +31
vtui_print_str:
	jmp	$0000		; +34
vtui_fill_box:
	jmp	$0000		; +37
vtui_pet2scr:
	jmp	$0000		; +40
vtui_scr2pet:
	jmp	$0000		; +43
vtui_border:
	jmp	$0000		; +46
vtui_save_rect:
	jmp	$0000		; +49
vtui_rest_rect:
	jmp	$0000		; +52
vtui_input_str:
	jmp	$0000		; +55

Fname:	.byte	"vtui0.9.bin"
End_fname:
