.ifndef VTUILIB_CA65_INC
VTUILIB_CA65_INC=1

VERA_ADDR_L		= $9F20
VERA_ADDR_M		= $9F21
VERA_ADDR_H		= $9F22
VERA_DATA0		= $9F23
VERA_DATA1		= $9F24
VERA_CTRL		= $9F25

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

BLACK		= 0
WHITE		= 1
RED		= 2
CYAN		= 3
PURPLE		= 4
GREEN		= 5
BLUE		= 6
YELLOW		= 7
ORANGE		= 8
BROWN		= 9
LIGHTRED	= 10
DARKGRAY	= 11
MIDGRAY		= 12
LIGHTGREEEN	= 13
LIGHTBLUE	= 14
LIGHTGRAY	= 15

; *****************************************************************************
; Use KERNAL API to set screen to 80x60, 40x30 or swap between them.
; (Modes: $00, $02 and $FF). If any other mode is specified, the macro
; will just fail silently.
; *****************************************************************************
; INPUTS:	.A or #mode = Screenmode ($00, $02 or $FF). If mode is used
;		it will produce smaller code as the decision is done at
;		compile time as opposed to runtime.
; *****************************************************************************
.macro VTUI_SCREEN_SET mode
.scope
	.ifnblank mode
		val = (.right (.tcount ({mode})-1, {mode}))
		.if val = $00
			lda	#$00
			jsr	$FF5F
		.elseif val = $02
			lda	#$02
			jsr	$FF5F
		.elseif val = $FF
			lda	#$FF
			jsr	$FF5F
		.endif
	.else
		beq	doset		; If 0, we can set mode
		cmp	#$02
		beq	doset		; If 2, we can set mode
		cmp	#$FF
		bne	end		; If $FF, we can set mode
doset:		jsr	$FF5F
end:
	.endif
.endscope
.endmacro

; *****************************************************************************
; Clear the screen with certain bg-/fg-color
; *****************************************************************************
; INPUTS:	If called without arguments, .A must contain color
; OPTIONAL:	(#)color | ((#)bgcolor,(#)fgcolor)
; USES:		.X & .Y
; *****************************************************************************
.macro VTUI_CLEAR color, fgcolor
.scope
	.if .paramcount > 0
	.ifblank fgcolor
	; color-code
		.if (.match (.left (1, {color}), #))
		; Immediate
			colcode = (.right (.tcount ({color})-1, {color}))
			lda	#colcode
		.else
		; Reference
			lda	color
		.endif
	.else
	; bg-&fg-color
		.if (.match (.left (1, {color}), #))
		; Immediate
			bg = (.right (.tcount ({color})-1, {color}))
			fg = (.right (.tcount ({fgcolor})-1, {fgcolor}))
			lda	#(bg<<4)|(fg&$0F)
		.else
		; Reference
			lda	color
			asl
			asl
			asl
			asl
			ora	fgcolor
		.endif
	.endif
	.endif
		ldy	#60		; 60 lines is the maximum
		sty	VERA_ADDR_M
		ldy	#' '
yloop:		ldx	#80		; 80 columns is maximum
		stz	VERA_ADDR_L
xloop:		sty	VERA_DATA0	; - Character
		sta	VERA_DATA0	; - bg-/fg-color
		dex
		bne	xloop
		dec	VERA_ADDR_M
		bpl	yloop
.endscope
.endmacro

; *****************************************************************************
; Set the stride without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUTS:	If called without arguments, .A must contain stride value
; OPTIONAL:	(#)stride
; USES:		.X (only when called without arguments)
; *****************************************************************************
.macro VTUI_SET_STRIDE stride
.scope
	.if .paramcount > 0
		.if (.match (.left (1, {stride}), #))
		; Immediate
			val = (.right (.tcount ({stride})-1, {stride}))
			lda	VERA_ADDR_H
			and	#$0F
			ora	#(val<<4)
			sta	VERA_ADDR_H
		.else
			lda	VERA_ADDR_H	; Set stride value to 0 in VERA_ADDR_H
			and	#$0F
			sta	VERA_ADDR_H
			lda	stride
			asl
			asl
			asl
			asl
			ora	VERA_ADDR_H
			sta	VERA_ADDR_H
		.endif
	.else
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
	.endif
.endscope
.endmacro

; *****************************************************************************
; Set the decrement value without changing other values in VERA_ADDR_H
; *****************************************************************************
; INPUTS:	If called without arguments, .C must contain decrement value
; OPTIONAL:	#decrement
; USES:		.A
; *****************************************************************************
.macro VTUI_SET_DECR decrement
.scope
	.if .paramcount > 0
		val = (.right (.tcount ({decrement})-1, {decrement}))
		lda	VERA_ADDR_H
		.if val = 0
			and	#%11110111
		.else
			ora	#%00001000
		.endif
		sta	VERA_ADDR_H
	.else
		lda	VERA_ADDR_H
		bcc	setnul
		ora	#%00001000
		bra	end
setnul:		and	#%11110111
end:		sta	VERA_ADDR_H
	.endif
.endscope
.endmacro

; *****************************************************************************
; Set VERA address to point to specific point on screen
; *****************************************************************************
; INPUTS:	If no arguments are given, .A = x coordinate & .Y = y coodinate
; OPTIONAL:	(#)xcord, (#)ycord
; USES:		.A
; *****************************************************************************
.macro VTUI_GOTOXY xcord, ycord
.scope
	.if .paramcount = 2
		.if (.match (.left (1, {xcord}), #))
			xval = (.right (.tcount ({xcord})-1, {xcord}))
			yval = (.right (.tcount ({ycord})-1, {ycord}))
			lda	#yval
			sta	VERA_ADDR_M
			lda	#(xval*2)
			sta	VERA_ADDR_L
		.else
			lda	ycord
			sta	VERA_ADDR_M
			lda	xcord
			asl
			sta	VERA_ADDR_L
		.endif
	.else
		sty	VERA_ADDR_M
		asl
		sta	VERA_ADDR_L
	.endif
.endscope
.endmacro

; *****************************************************************************
; Write character and color to current VERA address
; *****************************************************************************
; INPUTS:	If no arguments, .A = character & .X = color
; OPTIONAL:	(#)char,(#)color|((#)bgcolor,(#)fgcolor)
; USES:		.A
; *****************************************************************************
.macro VTUI_PLOT_CHAR char, color, fgcolor
.scope
	.if .paramcount = 3
		.if (.match (.left (1, {char}), #))
			lda	#(.right (.tcount ({char})-1, {char}))
			sta	VERA_DATA0
		.else
			lda	char
			sta	VERA_DATA0
		.endif
		.if (.match (.left (1, {color}), #))
			bg = (.right (.tcount ({color})-1, {color}))
			fg = (.right (.tcount ({fgcolor})-1, {fgcolor}))
			lda	#(bg<<4)|(fg&$0F)
			sta	VERA_DATA0
		.else
			lda	color
			asl
			asl
			asl
			asl
			ora	fgcolor
			sta	VERA_DATA0
		.endif
	.elseif .paramcount = 2
		.if (.match (.left (1, {char}), #))
			lda	#(.right (.tcount ({char})-1, {char}))
			sta	VERA_DATA0
		.else
			lda	char
			sta	VERA_DATA0
		.endif
		.if (.match (.left (1, {color}), #))
			lda	#(.right (.tcount ({color})-1, {color}))
			sta	VERA_DATA0
		.else
			lda	color
			sta	VERA_DATA0
		.endif
	.elseif .paramcount = 1
		.if (.match (.left (1, {char}), #))
			lda	#(.right (.tcount ({char})-1, {char}))
			sta	VERA_DATA0
		.else
			lda	char
			sta	VERA_DATA0
		.endif
	.else
		sta	VERA_DATA0
		stx	VERA_DATA0
	.endif
.endscope
.endmacro

; *****************************************************************************
; Read character and color from current VERA address
; *****************************************************************************
; OUTPUTS:	If called with no arguments, .A = character & .X = color
; OPTIONAL:	char, color | char | #nocol
; USES:		.A
; *****************************************************************************
.macro VTUI_SCAN_CHAR char, color
.scope
	.if .paramcount = 2
		lda	VERA_DATA0
		sta	char
		lda	VERA_DATA0
		sta	color
	.elseif .paramcount = 1
		.if (.match (.left (1, {char}), #))
			lda	VERA_DATA0
		.else
			lda	VERA_DATA0
			sta	char
		.endif
	.else
		lda	VERA_DATA0
		ldx	VERA_DATA0
	.endif
.endscope
.endmacro

; *****************************************************************************
; Create a horizontal line going from left to right.
; *****************************************************************************
; INPUTS:	If no arguments, .A=character, .X=color & .Y=length
; OPTIONAL:	(#)char,(#)length,((#)color|((#)bgcolor,(#)fgcolor))
; USES:		.A & (.Y or length)
; *****************************************************************************
.macro VTUI_HLINE char, length, color, fgcolor
.scope
	.if .paramcount = 4
		.if (.match (.left (1, {length}), #))
			ldy	#(.right (.tcount ({length})-1, {length}))
:			VTUI_PLOT_CHAR char, color, fgcolor
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR, char, color, fgcolor
			dec	length
			bne	:-
		.endif
	.elseif .paramcount = 3
		.if (.match (.left (1, {length}), #))
			ldy	#(.right (.tcount ({length})-1, {length}))
:			VTUI_PLOT_CHAR char, color
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR char, color
			dec	length
			bne	:-
		.endif
	.elseif .paramcount = 2
		.if (.match (.left (1, {length}), #))
			ldy	#(.right (.tcount ({length})-1, {length}))
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0
			dec	length
			bne	:-
		.endif
	.elseif .paramcount = 1
:		sta	VERA_DATA0
		ldx	VERA_DATA0	; skip colorinformation
		dey
		bne	:-
	.else
:		VTUI_PLOT_CHAR
		dey
		bne	:-
	.endif
.endscope
.endmacro

; *****************************************************************************
; Create a vertical line going from top to bottom.
; *****************************************************************************
; INPUTS:	If no arguments, .A=character, .X=color & .Y=height
; OPTIONAL:	(#)char,(#)height,(#)color|((#)bgcolor,(#)fgcolor)
; USES:		.A & (.Y or ~.height)
; *****************************************************************************
.macro VTUI_VLINE char, height, color, fgcolor
.scope
	.if .paramcount = 4
		.if (.match (.left (1, {height}), #))
			ldy	#(.right (.tcount ({height})-1, {height}))
:			VTUI_PLOT_CHAR char, color, fgcolor
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR, char, color, fgcolor
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dec	height
			bne	:-
		.endif
	.elseif .paramcount = 3
		.if (.match (.left (1, {height}), #))
			ldy	#(.right (.tcount ({height})-1, {height}))
:			VTUI_PLOT_CHAR char, color
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR, char, color
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dec	height
			bne	:-
		.endif
	.elseif .paramcount = 2
		.if (.match (.left (1, {height}), #))
			ldy	#(.right (.tcount ({height})-1, {height}))
:			VTUI_PLOT_CHAR char
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dey
			bne	:-
		.else
:			VTUI_PLOT_CHAR, char
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			dec	height
			bne	:-
		.endif
	.elseif .paramcount = 1
:		sta	VERA_DATA0
		dec	VERA_ADDR_L
		inc	VERA_ADDR_M
		dey
		bne	:-
	.else
:		VTUI_PLOT_CHAR
		dec	VERA_ADDR_L	; Return to original X coordinate
		dec	VERA_ADDR_L
		inc	VERA_ADDR_M	; Increment Y coordinate
		dey
		bne	:-
	.endif
.endscope
.endmacro

; *****************************************************************************
; Print a 0 terminated string PETSCII string
; *****************************************************************************
; INPUTS	If no arguments, x16 = pointer to string, .X  = bg-/fg color
; OPTIONAL	addr,(#)color|((#)bgcolor,(#)fgcolor)
; USES:		.A & .Y
; *****************************************************************************
.macro VTUI_PRINT_STR addr, color, fgcolor
.scope
	.if .paramcount = 3
		.if (.match (.left (1, {color}), #))
			bg = (.right (.tcount ({color})-1, {color}))
			fg = (.right (.tcount ({fgcolor})-1, {fgcolor}))
			ldx	#(bg<<4)|(fg&$0F)
		.else
			lda	color
			asl
			asl
			asl
			asl
			ora	fgcolor
			tax
		.endif
		ldy	#0
:		lda	addr,y
		beq	:+
		VTUI_PET2SCR
		VTUI_PLOT_CHAR
		iny
		bne	:-
:
	.elseif .paramcount = 2
		.if (.match (.left (1, {color}), #))
			ldx	#(.right (.tcount ({color})-1, {color}))
		.else
			ldx	color
		.endif
		ldy	#0
:		lda	addr,y
		beq	:+
		VTUI_PET2SCR
		VTUI_PLOT_CHAR
		iny
		bne	:-
:
	.elseif .paramcount = 1
		ldy	#0
:		lda	addr,y
		beq	:+
		VTUI_PET2SCR
		sta	VERA_DATA0
		lda	VERA_DATA0	; Skip color information
		iny
		bne	:-
:
	.else
		ldy	#0
:		lda	(x16),y		; Load character
		beq	:+		; If 0, we are done
		VTUI_PET2SCR
		VTUI_PLOT_CHAR
		iny
		bne	:-		; Get next character
:
	.endif
.endscope
.endmacro

; *****************************************************************************
; Create a filled box drawn from top left to bottom right
; *****************************************************************************
; INPUTS:	If no arguments, x16h=character,x17l=width,x17h=height,.X=color
; OPTIONAL:	(#)char,(#)width,(#)height,(#)color|((#)bgcolor,(#)fgcolor)
; USES:		.A, .X, .Y
; *****************************************************************************
.macro VTUI_FILL_BOX char, width, height, color, fgcolor
.scope
	.if .paramcount = 5
		.if ((.match (.left (1, {width}), #)) & (.match (.left (1, {height}), #)))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color, fgcolor
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {width}), #))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color, fgcolor
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {height}), #))
			ldy	width
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color, fgcolor
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.else
			ldy	width
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color, fgcolor
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.endif
	.elseif .paramcount = 4
		.if ((.match (.left (1, {width}), #)) & (.match (.left (1, {height}), #)))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {width}), #))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {height}), #))
			ldy	width
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.else
			ldy	width
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char, color
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.endif
	.elseif .paramcount = 3
		.if ((.match (.left (1, {width}), #)) & (.match (.left (1, {height}), #)))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0	; Skip color information
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {width}), #))
			ldy	#(.right (.tcount ({width})-1, {width}))
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0	; Skip color information
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	#(.right (.tcount ({width})-1, {width}))
			dex
			bne	:--
		.elseif (.match (.left (1, {height}), #))
			ldy	width
			ldx	#(.right (.tcount ({height})-1, {height}))
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0	; Skip color information
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.else
			ldy	width
			ldx	height
			lda	VERA_ADDR_L
			sta	x16l
:			lda	x16l		; Load x coordinate
			sta	VERA_ADDR_L	; Set x coordinate
:			VTUI_PLOT_CHAR char
			lda	VERA_DATA0	; Skip color information
			dey
			bne	:-
			inc	VERA_ADDR_M
			ldy	width
			dex
			bne	:--
		.endif
	.else
		lda	VERA_ADDR_L
		sta	x16l
:		lda	x16l		; Load x coordinate
		sta	VERA_ADDR_L	; Set x coordinate
		lda	x16h
		ldy	x17l
:		VTUI_PLOT_CHAR
		dey
		bne	:-
		inc	VERA_ADDR_M
		dec	x17h
		bne	:--
	.endif
.endscope
.endmacro

; *****************************************************************************
; Convert PETSCII codes between $20 and $5F to screencodes.
; *****************************************************************************
; INPUTS:	.A or (#)code = character to convert
; OUTPUS:	.A or code = converted character or $56 if invalid input
; *****************************************************************************
.macro VTUI_PET2SCR code
.scope
	.if .paramcount = 1
		.if (.match (.left (1, {code}), #))
			lda	#(.right (.tcount ({code})-1, {code}))
		.else
			lda	code
		.endif
	.endif
	cmp	#$20
	bcc	nonprintable	; .A < $20
	cmp	#$40
	bcc	end		; .A < $40 means screen code is the same
	; .A >= $40 - might be letter
	cmp	#$60
	bcs	nonprintable	; .A < $60 so it is a letter, subtract ($3F+1)
	sbc	#$3F		; to convert to screencode
	bra	end
nonprintable:
	lda	#$56
end:
	.if .paramcount = 1
		.if !(.match (.left (1, {code}), #))
			sta	code
		.endif
	.endif
.endscope
.endmacro

; *****************************************************************************
; Convert screencodes between $00 and $3F to PETSCII.
; *****************************************************************************
; INPUTS:	.A or (#)code = character to convert
; OUTPUS:	.A or code = converted character or $76 if invalid input
; *****************************************************************************
.macro VTUI_SCR2PET code
.scope
	.if .paramcount = 1
		.if (.match (.left (1, {code}), #))
			lda 	#(.right (.tcount ({code})-1, {code}))
		.else
			lda	code
		.endif
	.endif
	cmp	#$40
	bcs	nonprintable	; .A >= $40
	cmp	#$20
	bcs	end		; .A >=$20 & < $40 means petscii is the same
	; .A < $20 and is a letter
	adc	#$40
	bra	end
nonprintable:
	lda	#$76
end:
	.if .paramcount = 1
		.if !(.match (.left (1, {code}), #))
			sta	code
		.endif
	.endif
.endscope
.endmacro

; *****************************************************************************
; Set border characters in ZP variables according to the mode chosen
; *****************************************************************************
; INPUTS:	mode = The chosen bordermode
; USES:		.A
; *****************************************************************************
.macro VTUI_BORDER_MODE mde
.scope
	; Define local variable names for ZP variables
	; Makes the source a bit more readable
top_right=x18l
top_left =x18h
bot_right=x19l
bot_left =x19h
top	  =x19h+1		; z20l
bottom   =x19h+2		; z20h
left	  =x19h+3		; z21l
right	  =x19h+4		; z21h
	.if (.match (.left (1, {mde}), #))
		mode = (.right (.tcount ({mde})-1, {mde}))
	.else
		mode = mde
	.endif
	.if mode = 1
		lda	#$66
		sta	top_right
		sta	top_left
		sta	bot_right
		sta	bot_left
		sta	top
		sta	bottom
		sta	left
		sta	right
	.elseif mode = 2
		lda	#$6E
		sta	top_right
		lda	#$70
		sta	top_left
		lda	#$7D
		sta	bot_right
		lda	#$6D
		sta	bot_left
		lda	#$40		; centered lines
		sta	top
		sta	bottom
		lda	#$42
		sta	left
		sta	right
	.elseif mode = 3
		lda	#$49
		sta	top_right
		lda	#$55
		sta	top_left
		lda	#$4B
		sta	bot_right
		lda	#$4A
		sta	bot_left
		lda	#$40		; centered lines
		sta	top
		sta	bottom
		lda	#$42
		sta	left
		sta	right
	.elseif mode = 4
		lda	#$50
		sta	top_right
		lda	#$4F
		sta	top_left
		lda	#$7A
		sta	bot_right
		lda	#$4C
		sta	bot_left
		lda	#$77		; lines on edges
		sta	top
		lda	#$6F
		sta	bottom
		lda	#$74
		sta	left
		lda	#$6A
		sta	right
	.elseif mode = 5
		lda	#$5F
		sta	top_right
		lda	#$69
		sta	top_left
		lda	#$E9
		sta	bot_right
		lda	#$DF
		sta	bot_left
		lda	#$77		; lines on edges
		sta	top
		lda	#$6F
		sta	bottom
		lda	#$74
		sta	left
		lda	#$6A
		sta	right
	.else
		lda	#$20
		sta	top_right
		sta	top_left
		sta	bot_right
		sta	bot_left
		sta	top
		sta	bottom
		sta	left
		sta	right
	.endif
.endscope
.endmacro

; *****************************************************************************
; Create a box with a specific border
; *****************************************************************************
; INPUTS:	.A or (#)mode = Border mode (0-5) defaults to mode 0
;		(#)width or x17l  = width
;		(#)height or x17h = height
;		.X or (#)color|((#)bgcolor,(#)fgcolor) = bg-/fg-color
; USES		.Y, x16l & x16h
; *****************************************************************************
.macro VTUI_BORDER border, width, height, color, fgcolor
.scope
	; Define local variable names for ZP variables
	; Makes the source a bit more readable
top_right=x18l
top_left =x18h
bot_right=x19l
bot_left =x19h
top	 =x19h+1		; z20l
bottom   =x19h+2		; z20h
left	 =x19h+3		; z21l
right	 =x19h+4		; z21h
	.if .paramcount = 5
		VTUI_BORDER_MODE border
		; Save Initial position
		lda	VERA_ADDR_L
		sta	x16l
		lda	VERA_ADDR_M
		sta	x16h
		.if (.match (.left (1, {color}), #))
			bg = (.right (.tcount ({color})-1, {color}))
			fg = (.right (.tcount ({fgcolor})-1, {fgcolor}))
			ldx	#(bg<<4)|(fg&$0F)
		.else
			lda	color
			asl
			asl
			asl
			asl
			ora	fgcolor
			tax
		.endif
		.if (.match (.left (1, {width}), #))
			wdth = (.right (.tcount ({width})-1, {width}))
			hght = (.right (.tcount ({height})-1, {height}))
			ldy	#wdth-2		; width
			lda	top_left
			VTUI_PLOT_CHAR		; top left corner
			lda	top
			VTUI_HLINE		; top line
			lda	top_right
			VTUI_PLOT_CHAR		; top right corner
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	#hght-2		; height
			lda	right
			VTUI_VLINE		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	#hght-1		; height
			lda	left
			VTUI_VLINE		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			VTUI_PLOT_CHAR		; bottom left corner
			ldy	#wdth-1
			lda	bottom
			VTUI_HLINE		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			VTUI_PLOT_CHAR		; bottom right corner
		.else
			ldy	width		; width
			dey
			dey
			lda	top_left
			VTUI_PLOT_CHAR		; top left corner
			lda	top
			VTUI_HLINE		; top line
			lda	top_right
			VTUI_PLOT_CHAR		; top right corner
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			dey
			lda	right
			VTUI_VLINE		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			lda	left
			VTUI_VLINE		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			VTUI_PLOT_CHAR		; bottom left corner
			ldy	width
			dey
			lda	bottom
			VTUI_HLINE		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			VTUI_PLOT_CHAR		; bottom right corner
		.endif
	.elseif .paramcount = 4
		VTUI_BORDER_MODE border
		; Save Initial position
		lda	VERA_ADDR_L
		sta	x16l
		lda	VERA_ADDR_M
		sta	x16h
		.if (.match (.left (1, {color}), #))
			ldx	#(.right (.tcount ({color})-1, {color}))
		.else
			ldx	color
		.endif
		.if (.match (.left (1, {width}), #))
			wdth = (.right (.tcount ({width})-1, {width}))
			hght = (.right (.tcount ({height})-1, {height}))
			ldy	#wdth-2		; width
			lda	top_left
			VTUI_PLOT_CHAR		; top left corner
			lda	top
			VTUI_HLINE		; top line
			lda	top_right
			VTUI_PLOT_CHAR		; top right corner
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	#hght-2		; height
			lda	right
			VTUI_VLINE		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	#hght-1		; height
			lda	left
			VTUI_VLINE		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			VTUI_PLOT_CHAR		; bottom left corner
			ldy	#wdth-1
			lda	bottom
			VTUI_HLINE		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			VTUI_PLOT_CHAR		; bottom right corner
		.else
			ldy	width		; width
			dey
			dey
			lda	top_left
			VTUI_PLOT_CHAR		; top left corner
			lda	top
			VTUI_HLINE		; top line
			lda	top_right
			VTUI_PLOT_CHAR		; top right corner
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			dey
			lda	right
			VTUI_VLINE		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			lda	left
			VTUI_VLINE		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			VTUI_PLOT_CHAR		; bottom left corner
			ldy	width
			dey
			lda	bottom
			VTUI_HLINE		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			VTUI_PLOT_CHAR		; bottom right corner
		.endif
	.elseif .paramcount = 3
		VTUI_BORDER_MODE border
		; Save Initial position
		lda	VERA_ADDR_L
		sta	x16l
		lda	VERA_ADDR_M
		sta	x16h
		.if (.match (.left (1, {width}), #))
			wdth = (.right (.tcount ({width})-1, {width}))
			hght = (.right (.tcount ({height})-1, {height}))
			ldy	#wdth-2		; width
			lda	top_left
			sta	VERA_DATA0	; top left corner
			lda	VERA_DATA0	; Skip colorinformation
			lda	top
			VTUI_HLINE 0		; top line
			lda	top_right
			sta	VERA_DATA0	; top right corner
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	#hght-2		; height
			lda	right
			VTUI_VLINE 0		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	#hght-1		; height
			lda	left
			VTUI_VLINE 0		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			sta	VERA_DATA0	; bottom left corner
			lda	VERA_DATA0	; Skip colorinformation
			ldy	#wdth-1
			lda	bottom
			VTUI_HLINE 0		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			sta	VERA_DATA0	; bottom right corner
		.else
			ldy	width		; width
			dey
			dey
			lda	top_left
			sta	VERA_DATA0	; top left corner
			lda	VERA_DATA0	; Skip colorinformation
			lda	top
			VTUI_HLINE 0		; top line
			lda	top_right
			sta	VERA_DATA0	; top right corner
			dec	VERA_ADDR_L
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			dey
			lda	right
			VTUI_VLINE 0		; right line
			; Restore initial VERA address
			lda	x16l
			sta	VERA_ADDR_L
			lda	x16h
			sta	VERA_ADDR_M
			inc	VERA_ADDR_M
			ldy	height		; height
			dey
			lda	left
			VTUI_VLINE 0		; left line
			dec	VERA_ADDR_M
			lda	bot_left
			sta	VERA_DATA0	; bottom left corner
			lda	VERA_DATA0	; Skip colorinformation
			ldy	width
			dey
			lda	bottom
			VTUI_HLINE 0		; bottom line
			dec	VERA_ADDR_L
			dec	VERA_ADDR_L
			lda	bot_right
			sta	VERA_DATA0	; bottom right corner
		.endif
	.else
	; Set the border drawing characters according to the border mode in .A
mode1:		cmp	#1
		bne	mode2
		lda	#$66
		bra	def
mode2:		cmp	#2
		bne	mode3
		lda	#$6E
		sta	top_right
		lda	#$70
		sta	top_left
		lda	#$7D
		sta	bot_right
		lda	#$6D
		sta	bot_left
clines:
		lda	#$40		; centered lines
		sta	top
		sta	bottom
		lda	#$42
		sta	left
		sta	right
		bra	dodraw
mode3:		cmp	#3
		bne	mode4
		lda	#$49
		sta	top_right
		lda	#$55
		sta	top_left
		lda	#$4B
		sta	bot_right
		lda	#$4A
		sta	bot_left
		bra	clines
mode4:		cmp	#4
		bne	mode5
		lda	#$50
		sta	top_right
		lda	#$4F
		sta	top_left
		lda	#$7A
		sta	bot_right
		lda	#$4C
		sta	bot_left
elines:
		lda	#$77		; lines on edges
		sta	top
		lda	#$6F
		sta	bottom
		lda	#$74
		sta	left
		lda	#$6A
		sta	right
		bra	dodraw
mode5:		cmp	#5
		bne	default
		lda	#$5F
		sta	top_right
		lda	#$69
		sta	top_left
		lda	#$E9
		sta	bot_right
		lda	#$DF
		sta	bot_left
		bra	elines
default:
		lda	#$20
def:		sta	top_right
		sta	top_left
		sta	bot_right
		sta	bot_left
		sta	top
		sta	bottom
		sta	left
		sta	right
dodraw:		; Save initial position
		lda	VERA_ADDR_L
		sta	x16l
		lda	VERA_ADDR_M
		sta	x16h
		ldy	x17l		; width
		dey
		lda	top_left
		VTUI_PLOT_CHAR		; Top left corner
		dey
		lda	top
		VTUI_HLINE			; Top line
		lda	top_right
		VTUI_PLOT_CHAR		; Top right corner
		dec	VERA_ADDR_L
		dec	VERA_ADDR_L
		inc	VERA_ADDR_M
		ldy	x17h		;height
		dey
		dey
		lda	right
		VTUI_VLINE			; Right line
		; Restore initial VERA address
		lda	x16l
		sta	VERA_ADDR_L
		lda	x16h
		sta	VERA_ADDR_M
		inc	VERA_ADDR_M
		ldy	x17h		;height
		dey
		lda	left
		VTUI_VLINE			; Left line
		dec	VERA_ADDR_M
		lda	bot_left
		VTUI_PLOT_CHAR		; Bottom left corner
		ldy	x17l
		dey
		lda	bottom
		VTUI_HLINE			; Bottom line
		dec	VERA_ADDR_L
		dec	VERA_ADDR_L
		lda	bot_right
		VTUI_PLOT_CHAR		; Bottom right corner
	.endif
.endscope
.endmacro

; *****************************************************************************
; Increment 16 bit value at .addr
; *****************************************************************************
; INPUTS:	.addr = address of low byte of the 16 bit number
; *****************************************************************************
.macro VTUI_INC16 addr
	inc	addr
	bne	:+
	inc	addr+1
:
.endmacro

; *****************************************************************************
; Set VERA bank (High memory) without touching anything else
; *****************************************************************************
; INPUTS:	.A or (#)bank = Bank number, 0 or 1
; USES:		.A
; *****************************************************************************
.macro VTUI_SET_BANK bank
.scope
	.if .paramcount = 1
		.if (.match (.left (1, {bank}), #))
			num = (.right (.tcount ({bank})-1, {bank}))
		.else
			num = bank
		.endif
		lda	VERA_ADDR_H
		.if num = 0
			and	#$FE
		.else
			ora	#$01
		.endif
		sta	VERA_ADDR_H
	.else
		cmp	#0
		beq	setzero
		; Bank = 1
		lda	VERA_ADDR_H
		ora	#$01
		sta	VERA_ADDR_H
		bra	end
setzero:
		; Bank = 0
		lda	VERA_ADDR_H
		and	#$FE
		sta	VERA_ADDR_H
end:
	.endif
.endscope
.endmacro

; *****************************************************************************
; Copy contents of screen from current position to other memory area in
; either system RAM or VRAM
; *****************************************************************************
; INPUTS:	.C or destram	= Destination RAM (0=system RAM, 1=VRAM)
;		.A		= VRAM Bank (0 or 1) if .C|.destram=1
;		x16 | destaddr	= Destination address (17bit for VRAM)
;		x17l|(#)width	= width
;		x17h|(#)height	= height
; *****************************************************************************
.macro VTUI_SAVE_RECT destaddr, width, height, destram
.scope
	.if .paramcount >= 3
		.ifblank destram		; System RAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<destaddr
				sta	x16l
				lda	#>destaddr
				sta	x16h
				ldx	#wd		; Load width
:				lda	VERA_DATA0	; Copy character
				sta	(x16)
				VTUI_INC16 x16		; Increment dest address
				lda	VERA_DATA0	; Copy colorcode
				sta	(x16)
				VTUI_INC16 x16		; Increment dest address
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore x coordinate
				inc	VERA_ADDR_M
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<destaddr
				sta	x16l
				lda	#>destaddr
				sta	x16h
				ldx	width
:				lda	VERA_DATA0
				sta	(x16)
				VTUI_INC16 x16
				lda	VERA_DATA0
				sta	(x16)
				VTUI_INC16 x16
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.elseif destram = 0		; System RAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<destaddr
				sta	x16l
				lda	#>destaddr
				sta	x16h
				ldx	#wd		; Load width
:				lda	VERA_DATA0	; Copy character
				sta	(x16)
				VTUI_INC16 x16		; Increment dest address
				lda	VERA_DATA0	; Copy colorcode
				sta	(x16)
				VTUI_INC16 x16		; Increment dest address
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore x coordinate
				inc	VERA_ADDR_M
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<destaddr
				sta	x16l
				lda	#>destaddr
				sta	x16h
				ldx	width
:				lda	VERA_DATA0
				sta	(x16)
				VTUI_INC16 x16
				lda	VERA_DATA0
				sta	(x16)
				VTUI_INC16 x16
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.else			; VRAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg		; Save height for decrementing
				sta	x17h
				ldy	VERA_ADDR_L	; Save X coordinate
				ldx	#1		; Set ADDRsel to 1
				stx	VERA_CTRL
				VTUI_SET_BANK #^destaddr
				VTUI_SET_STRIDE #1
				lda	#<destaddr	; Set destination address
				sta	VERA_ADDR_L
				lda	#>destaddr
				sta	VERA_ADDR_M
				stz	VERA_CTRL	; Set ADDRsel back to 0
				ldx	#wd
:				lda	VERA_DATA0	; Copy character
				sta	VERA_DATA1
				lda	VERA_DATA0	; Copy colorcode
				sta	VERA_DATA1
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore X coordinate
				inc	VERA_ADDR_M	; Increment Y coordinate
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h
				ldy	VERA_ADDR_L
				ldx	#1
				stx	VERA_CTRL
				VTUI_SET_BANK #^destaddr
				VTUI_SET_STRIDE #1
				lda	#<destaddr
				sta	VERA_ADDR_L
				lda	#>destaddr
				sta	VERA_ADDR_M
				stz	VERA_CTRL
				ldx	width
:				lda	VERA_DATA0
				sta	VERA_DATA1
				lda	VERA_DATA0
				sta	VERA_DATA1
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.endif
	.else
		ldy	VERA_ADDR_L	; Save X coordinate for later
		bcc	sysram
		; VRAM
		ldx	#1		; Set ADDRsel to 1
		stx	VERA_CTRL
		VTUI_SET_BANK
		lda	#1
		VTUI_SET_STRIDE
		lda	x16l		; Set destination address
		sta	VERA_ADDR_L
		lda	x16h
		sta	VERA_ADDR_M
		stz	VERA_CTRL	; Set ADDRsel back to 0
		ldx	x17l		; Load width
vloop:		lda	VERA_DATA0	; Copy Character
		sta	VERA_DATA1
		lda	VERA_DATA0	; Copy Color Code
		sta	VERA_DATA1
		dex
		bne	vloop
		ldx	x17l		; Restore width
		sty	VERA_ADDR_L	; Restore X coordinate
		inc	VERA_ADDR_M	; Increment Y coordinate
		dec	x17h
		bne	vloop
		bra	end
sysram:
		; System RAM
		ldx	x17l		; Load width
sloop:		lda	VERA_DATA0	; Copy Character
		sta	(x16)
		VTUI_INC16 x16		; Increment destination address
		lda	VERA_DATA0	; Copy Color Code
		sta	(x16)
		VTUI_INC16 x16		; Increment destination address
		dex
		bne	sloop
		ldx	x17l		; Restore width
		sty	VERA_ADDR_L	; Restore X coordinate
		inc	VERA_ADDR_M
		dec	x17h
		bne	sloop
end:
	.endif
.endscope
.endmacro

; *****************************************************************************
; Restore contents of screen from other memory area in either system RAM
; or VRAM starting at current position
; *****************************************************************************
; INPUTS:	.C or srcram	= Source RAM (0=system RAM, 1=VRAM)
;		.A		= VRAM Bank (0 or 1) if .C|.srcram=1
;		x16 | srcaddr 	= Source address
;		x17l|(#)width	= width
;		x17h|(#)height	= height
; *****************************************************************************
.macro VTUI_REST_RECT srcaddr, width, height, srcram
.scope
	.if .paramcount >= 3
		.ifblank srcram		; System RAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<srcaddr
				sta	x16l
				lda	#>srcaddr
				sta	x16h
				ldx	#wd		; Load width
:				lda	(x16)		; Copy character
				sta	VERA_DATA0
				VTUI_INC16 x16		; Increment dest address
				lda	(x16)		; Copy colorcode
				sta	VERA_DATA0
				VTUI_INC16 x16		; Increment dest address
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore x coordinate
				inc	VERA_ADDR_M
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<srcaddr
				sta	x16l
				lda	#>srcaddr
				sta	x16h
				ldx	width
:				lda	(x16)
				sta	VERA_DATA0
				VTUI_INC16 x16
				lda	(x16)
				sta	VERA_DATA0
				VTUI_INC16 x16
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.elseif srcram = 0		; System RAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<srcaddr
				sta	x16l
				lda	#>srcaddr
				sta	x16h
				ldx	#wd		; Load width
:				lda	(x16)		; Copy character
				sta	VERA_DATA0
				VTUI_INC16 x16		; Increment dest address
				lda	(x16)		; Copy colorcode
				sta	VERA_DATA0
				VTUI_INC16 x16		; Increment dest address
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore x coordinate
				inc	VERA_ADDR_M
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h		; Save height
				ldy	VERA_ADDR_L	; Save X coordinate
				lda	#<srcaddr
				sta	x16l
				lda	#>srcaddr
				sta	x16h
				ldx	width
:				lda	(x16)
				sta	VERA_DATA0
				VTUI_INC16 x16
				lda	(x16)
				sta	VERA_DATA0
				VTUI_INC16 x16
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.else			; VRAM
			.if (.match (.left (1, {width}), #))
				wd = (.right (.tcount ({width})-1, {width}))
				hg = (.right (.tcount ({height})-1, {height}))
				lda	#hg		; Save height for decrementing
				sta	x17h
				ldy	VERA_ADDR_L	; Save X coordinate
				ldx	#1		; Set ADDRsel to 1
				stx	VERA_CTRL
				VTUI_SET_BANK #^srcaddr
				VTUI_SET_STRIDE #1
				lda	#<srcaddr	; Set destination address
				sta	VERA_ADDR_L
				lda	#>srcaddr
				sta	VERA_ADDR_M
				stz	VERA_CTRL	; Set ADDRsel back to 0
				ldx	#wd
:				lda	VERA_DATA1	; Copy character
				sta	VERA_DATA0
				lda	VERA_DATA1	; Copy colorcode
				sta	VERA_DATA0
				dex
				bne	:-
				ldx	#wd		; Restore width
				sty	VERA_ADDR_L	; Restore X coordinate
				inc	VERA_ADDR_M	; Increment Y coordinate
				dec	x17h		; Decrement height
				bne	:-
			.else
				lda	height
				sta	x17h
				ldy	VERA_ADDR_L
				ldx	#1
				stx	VERA_CTRL
				VTUI_SET_BANK #^srcaddr
				VTUI_SET_STRIDE #1
				lda	#<srcaddr
				sta	VERA_ADDR_L
				lda	#>srcaddr
				sta	VERA_ADDR_M
				stz	VERA_CTRL
				ldx	width
:				lda	VERA_DATA1
				sta	VERA_DATA0
				lda	VERA_DATA1
				sta	VERA_DATA0
				dex
				bne	:-
				ldx	width
				sty	VERA_ADDR_L
				inc	VERA_ADDR_M
				dec	x17h
				bne	:-
			.endif
		.endif
	.else
		ldy	VERA_ADDR_L	; Save X coordinate for later
		bcc	sysram
		; VRAM
		ldx	#1		; Set ADDRsel to 1
		stx	VERA_CTRL
		VTUI_SET_BANK
		lda	#1
		VTUI_SET_STRIDE
		lda	x16l		; Set destination address
		sta	VERA_ADDR_L
		lda	x16h
		sta	VERA_ADDR_M
		stz	VERA_CTRL	; Set ADDRsel back to 0
		ldx	x17l		; Load width
vloop:		lda	VERA_DATA1	; Copy Character
		sta	VERA_DATA0
		lda	VERA_DATA1	; Copy Color Code
		sta	VERA_DATA0
		dex
		bne	vloop
		ldx	x17l		; Restore width
		sty	VERA_ADDR_L	; Restore X coordinate
		inc	VERA_ADDR_M	; Increment Y coordinate
		dec	x17h
		bne	vloop
		bra	end
sysram:
		; System RAM
		ldx	x17l		; Load width
sloop:		lda	(x16)		; Copy Character
		sta	VERA_DATA0
		VTUI_INC16 x16		; Increment destination address
		lda	(x16)		; Copy Color Code
		sta	VERA_DATA0
		VTUI_INC16 x16		; Increment destination address
		dex
		bne	sloop
		ldx	x17l		; Restore width
		sty	VERA_ADDR_L	; Restore X coordinate
		inc	VERA_ADDR_M
		dec	x17h
		bne	sloop
end:
	.endif
.endscope
.endmacro

.endif
