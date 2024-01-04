#ifndef _vtuilib-cc65_h_
#define _vtuilib-cc65_h_

// Constants meant to be used in VTUI functions
#define VIDRAM 0x80
#define SYSRAM 0x00
#define VRAMBANK1 1
#define VRAMBANK0 0
#define PETSCII_TRUE  0x00
#define PETSCII_FALSE 0x80
#define VTUI_INC 0
#define VTUI_DEC 1
// Screen modes defined for VERA, VTUI calls kernal som same works for VTUI
#define	SCRMODE80X60 0x00
#define SCRMODE80X30 0x01
#define SCRMODE40X60 0x02
#define SCRMODE40X30 0x03
#define SCRMODE40X15 0x04
#define SCRMODE20X30 0x05
#define SCRMODE20X15 0x06
#define SCRMODESWAP  0xFF
// VERA color constants
#define BLACK		0
#define WHITE		1
#define RED		2
#define CYAN		3
#define PURPLE		4
#define GREEN		5
#define BLUE		6
#define YELLOW		7
#define ORANGE		8
#define BROWN		9
#define LIGHTRED	10
#define DARKGRAY	11
#define MIDGRAY		12
#define LIGHTGREEEN	13
#define LIGHTBLUE	14
#define LIGHTGRAY	15

// Load a file identified by filename from device 8 to address given in addr,
// returns true on success
extern char	__fastcall__ vtui_load(char *addr, char *filename);
// Initialize the rest of the library, defining the address where VTUI is loaded
extern void	__fastcall__ vtui_initialize(char *addr);
// Set screen mode to one of VERA's supported modes, returns true on success
extern char	__fastcall__ vtui_screen_set(char mode);
// Set high bit (bank) of VERA address without touching anything else in VERA_ADDR_H
extern void	__fastcall__ vtui_set_bank(char bank);
extern char	__fastcall__ vtui_get_bank();
// Set stride value without touching anything else in VERA_ADDR_H
extern void	__fastcall__ vtui_set_stride(char stride);
extern char	__fastcall__ vtui_get_stride();
// Set Decrement bit in VERA_ADDR_H without touching anything else, use the defined
// VTUI_INC and VTUI_DEC constants for clarity
extern void	__fastcall__ vtui_set_decr(char decr);
extern char	__fastcall__ vtui_get_decr();
// Fill screen with "fillchar" characters and set their color to "color"
extern void	__fastcall__ vtui_clr_scr(char fillchar, char color);
// Set VERA address registers to point to X,Y coordinates (max 79,59). Assumes bank=1
extern void	__fastcall__ vtui_gotoxy(char x, char y);
// Write ch character to VERA_DATA0, if stride=1 and decrement=0, write color
extern void	__fastcall__ vtui_plot_char(char ch, char color);
// Read character from VERA_DATA0, if stride=1 and decrement=0, read color as well
// Char and color are returned in a singel 16bit value, low byte = char.
extern unsigned	__fastcall__ vtui_scan_char();
// Create a horizontal line of ch character that is length with color
extern void	__fastcall__ vtui_hline(char ch, char length, char color);
// Create a vertical line of ch character that is length with color
extern void	__fastcall__ vtui_vline(char ch, char length, char color);
// Print a string pointet to by str with length and color.
// If ispetscii = PETSCII_TRUE, characters will be converted to screencodes
extern void	__fastcall__ vtui_print_str(char *str, char length, char color, char ispetscii);
// Create a box filled, width wide and height high filled with ch and color
extern void	__fastcall__ vtui_fill_box(char ch, char width, char height, char color);
// Convert ch from PETSCII to VERA screencode, converted character is returned
extern char	__fastcall__ vtui_pet2scr(char ch);
// Convert ch from VERA screencode to PETSCII, converted character is returned
extern char	__fastcall__ vtui_scr2pet(char ch);
// Create a border, width wide and height high, of border characters with color
extern void	__fastcall__ vtui_border(char border, char width, char height, char color);
// Copy contents of screen from current position to other memory in sys- or V-RAM
// destram is either SYSRAM or VIDRAM, if VIDRAM then vrambank must be 0 or 1
extern void	__fastcall__ vtui_save_rect(unsigned destaddr, char destram, char width, char height, char vrambank);
// Restore contents of screen from other memory area in sys- og V-RAM.
// srcram is either SYSRAM or VIDRAM, if VIDRAM then vrambank must be 0 or 1
extern void	__fastcall__ vtui_rest_rect(unsigned srcaddr, char srcram, char width, char height, char vrambank);
// Show a cursor and get at string input from keyboard
// str = pre-allocated buffer to contain the typed string.
// maxlen is the maximum length allowed for the string.
// Typed characters will have 'color'
// High-byte = last key pressed (ENTER or ESC), low-byte = actual length
extern unsigned	__fastcall__ vtui_input_str(char *str, char maxlen, char color);

#endif