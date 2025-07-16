.import popa
;.import sp			; Keeping these here for reference if I ever need them.
;.import sreg

.export _vtui_load
.export _vtui_initialize
.export _vtui_screen_set
.export _vtui_set_bank
.export _vtui_get_bank
.export _vtui_set_stride
.export _vtui_get_stride
.export _vtui_set_decr
.export _vtui_get_decr
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

r0	= $02
r0l	= r0
r0h	= r0+1
r1	= $04
r1l	= r1
r1h	= r1+1
r2	= $06
r2l	= r2
r2h	= r2+1
r3	= $08
r3l	= r3
r3h	= r3+1
r4	= $0A
r4l	= r4
r4h	= r4+1
r5	= $0C
r5l	= r5
r5h	= r5+1
r6	= $0E
r6l	= r6
r6h	= r6+1
r7	= $10
r7l	= r7
r7h	= r7+1
r8	= $12
r8l	= r8
r8h	= r8+1
r9	= $14
r9l	= r9
r9h	= r9+1
r10	= $16
r10l	= r10
r10h	= r10+1
r11	= $18
r11l	= r11
r11h	= r11+1
r12	= $1A
r12l	= r12
r12h	= r12+1

.segment "CODE"

; *****************************************************************************
; Use KERNAL functions to load a file from device 8 to memory at
; specific address
; *****************************************************************************
_vtui_load:
	sta	r0l			; Save high-byte of filename-address
	stx	r0h			; Save low-byte of filename-address

	lda	#1			; Logical file number (must be unique)
	ldx	#8			; Device number (8 local filesystem)
	ldy	#0			; Secondary command 0 = dont use addr in file
	jsr	$FFBA			; SETLFS

	; Find length of string by seeking to first 0-byte
	ldy	#0
:	lda	(r0),y
	beq	:+
	iny
	bra	:-

:	tya				; Length of filename in A
	ldx	r0			; Start address of filename
	ldy	r0+1
	jsr	$FFBD			; SETNAM
	jsr	popa			; Get load address into X and Y
	tax
	jsr	popa
	tay
	lda	#0			; 0=load, 1=verify
	jsr	$FFD5			; LOAD
	ldx	#0			; Must return 16bits even though return
					; type is only 8 bits.
	lda	#0			; Move Carry bit to acumulator and
	rol				; invert it to correspond to C-style
	eor	#1			; true=1 or false=0
	rts

; *****************************************************************************
; Initialize internal jump-table with starting address given as argument.
; Then call the normal vtui_initialize function
; *****************************************************************************
_vtui_initialize:
	; Initialize the jump-table at the end of the file with correct addresses
	sta	vtui_initialize+1
	stx	vtui_initialize+2
	clc				; Add 2 to jump-address as first one in library
	adc	#2			; is branch
	bcc	:+
	inx
:	ldy	#3			; Use .Y for indexing into jump table below
:	sta	vtui_initialize+1,y	; store low value
	txa
	sta	vtui_initialize+2,y	; store high value
	lda	vtui_initialize+1,y	; read low value into .A again
	clc				; Add 3 to jump-address
	adc	#3
	bcc	:+
	inx
:	iny				; Add 3 to .Y indexing
	iny
	iny
	cpy	#66			; Ensure we have gone through all addresses
	bne	:--			; below

	; Library is designed to work with standard PETSCII character set, but
	; CC65 switches to lower case character set.
	; The two lines below would switch back to standard PETSCII
;	lda	#$8E			; Uppercase
;	jsr	$FFD2
	jmp	vtui_initialize

; *****************************************************************************
; Set screen mode according to argument at return .C inverted
; *****************************************************************************
_vtui_screen_set:
	jsr	vtui_screen_set
	ldx	#0
	lda	#0
	rol
	eor	#1
	rts

; *****************************************************************************
; Move bank number into .C and call vtui_set_bank
; *****************************************************************************
_vtui_set_bank:
	ror
	jmp	vtui_set_bank

; *****************************************************************************
; Return current bank number
; *****************************************************************************
_vtui_get_bank:
	ldx	#0
	jsr	vtui_get_bank
	lda	#0
	rol
	rts

; *****************************************************************************
; Set VERA stride, value passed as argument and available in A
; *****************************************************************************
_vtui_set_stride:
	jmp	vtui_set_stride

; *****************************************************************************
; Return current stride value
; *****************************************************************************
_vtui_get_stride:
	ldx	#0
	jmp	vtui_get_stride

; *****************************************************************************
; Move inc/dec value into .C and call vtui_set_decr
; *****************************************************************************
_vtui_set_decr:
	ror
	jmp	vtui_set_decr

; *****************************************************************************
; Return current decrement value
; *****************************************************************************
_vtui_get_decr:
	ldx	#0
	jsr	vtui_get_decr
	lda	#0
	rol
	rts

; *****************************************************************************
; Get char and color arguments into registers and call vtui_clr_scr
; *****************************************************************************
_vtui_clr_scr:
	tax
	jsr	popa
	jmp	vtui_clr_scr

; *****************************************************************************
; Get x and y coordinates into registers and call vtui_gotoxy
; *****************************************************************************
_vtui_gotoxy:
	pha
	jsr	popa		; popa clobbers .Y
	ply
	jmp	vtui_gotoxy

; *****************************************************************************
; Get char and color arguments into registers and call vtui_plot_char
; *****************************************************************************
_vtui_plot_char:
	tax
	jsr	popa
	jmp	vtui_plot_char

; *****************************************************************************
; Call vtui_scan_char, return character and colorcode as 16 bit value.
; Low byte is the character
; High byte is the colorcode if VERA stride = 1 and VERA increment = 0
; *****************************************************************************
_vtui_scan_char:
	jmp	vtui_scan_char

; *****************************************************************************
; Get char, length and color arguments into registers and call vtui_hline
; *****************************************************************************
_vtui_hline:
	tax
	jsr	popa
	pha
	jsr	popa		; popa clobbers .Y
	ply
	jmp	vtui_hline

; *****************************************************************************
; Get char, length and color arguments into registers and call vtui_vline
; *****************************************************************************
_vtui_vline:
	tax
	jsr	popa
	pha
	jsr	popa		; popa clobbers .Y
	ply
	jmp	vtui_vline

; *****************************************************************************
; Get arguments into correct registers and ZP variables before calling 
; vtui_print_str
; *****************************************************************************
_vtui_print_str:
	pha				; Store PETSCII conversion value on 
					; stack while fetching other parameters
	jsr	popa
	tax				; Color
	jsr	popa
	pha				; Length
	jsr	popa
	sta	r0			; String pointer low byte
	jsr	popa
	sta	r0+1			; String pointer high byte
	ply
	pla				; Conversion value from normal stack
	tya
	jmp	vtui_print_str

; *****************************************************************************
; Get arguments into correct registers and ZP locations before calling
; vtui_fill_box
; *****************************************************************************
_vtui_fill_box:
	tax
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	jmp	vtui_fill_box

; *****************************************************************************
; Call vtui_pet2scr and ensure that function returns value to C caller
; *****************************************************************************
_vtui_pet2scr:
	jsr	vtui_pet2scr
	ldx	#0
	rts

; *****************************************************************************
; Call vtui_scr2pet and ensure that function returns value to C caller
; *****************************************************************************
_vtui_scr2pet:
	jsr	vtui_scr2pet
	ldx	#0
	rts

; *****************************************************************************
; Get arguments into correct registers and ZP locations before calling
; vtui_border
; *****************************************************************************
_vtui_border:
	tax
	jsr	popa
	sta	r2l
	jsr	popa
	sta	r1l
	jsr	popa
	jmp	vtui_border

; *****************************************************************************
; Get arguments into correct registers and ZP locations before calling
; vtui_save_rect
; *****************************************************************************
_vtui_save_rect:
	pha				; VRAM bank
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
	ply
	pla
	ror				; Set .C = VRAM bank
	tya
	jmp	vtui_save_rect

; *****************************************************************************
; Get arguments into correct registers and ZP locations before calling
; vtui_rest_rect
; *****************************************************************************
_vtui_rest_rect:
	pha				; VRAM bank
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
	ply
	pla
	ror				; Set .C = VRAM bank
	tya
	jmp	vtui_rest_rect

; *****************************************************************************
; Get arguments into correct registers before calling vtui_input_str
; then ensure function returns correct value to C caller
; *****************************************************************************
_vtui_input_str:
	tax
	jsr	popa
	pha
	jsr	popa
	sta	r0l
	jsr	popa
	sta	r0h
	ply
	jsr	vtui_input_str
	tax				; Last key in high-byte
	tya				; Actual length in low-byte
	rts

; Jump table into jumptable in actual VTUI library :(
vtui_initialize:
	jmp	$0000
vtui_screen_set:
	jmp	$0000
vtui_set_bank:
	jmp	$0000
vtui_set_stride:
	jmp	$0000
vtui_set_decr:
	jmp	$0000
vtui_clr_scr:
	jmp	$0000
vtui_gotoxy:
	jmp	$0000
vtui_plot_char:
	jmp	$0000
vtui_scan_char:
	jmp	$0000
vtui_hline:
	jmp	$0000
vtui_vline:
	jmp	$0000
vtui_print_str:
	jmp	$0000
vtui_fill_box:
	jmp	$0000
vtui_pet2scr:
	jmp	$0000
vtui_scr2pet:
	jmp	$0000
vtui_border:
	jmp	$0000
vtui_save_rect:
	jmp	$0000
vtui_rest_rect:
	jmp	$0000
vtui_input_str:
	jmp	$0000
vtui_get_bank:
	jmp	$0000
vtui_get_stride:
	jmp	$0000
vtui_get_decr:
	jmp	$0000
