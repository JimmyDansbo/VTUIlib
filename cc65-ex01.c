#include "vtuilib-cc65.h"

int main() {

	// Load VTUI library into pre-allocated memory and initialize it
	// ensure defined memory is large enough to hold VTUI library
//	char	vtui[1023];
//	vtui_load(vtui, "vtui1.0.bin");
//	vtui_initialize(vtui);

	// Load VTUI library to address 0x0400 and initialize it
	vtui_load((char*)0x0400, "vtui1.0.bin");
	vtui_initialize((char*)0x0400);

	// Switch to standard PETSCII character set
	__asm__ ("lda #$8E");
	__asm__ ("jsr $FFD2");

	if (vtui_screen_set(SCRMODE40X30)) {
		if (vtui_get_bank()!=1)
			vtui_set_bank(1);
		if (vtui_get_stride()!=1)
			vtui_set_stride(1);
		if (vtui_get_decr()!=0)
			vtui_set_decr(0);
		vtui_clr_scr(vtui_pet2scr(' '), (LIGHTGRAY<<4)+BLACK);

		vtui_gotoxy(11, 2);
		// Write blue hearts to screen
		vtui_plot_char(0x53, (LIGHTGRAY<<4)+BLUE);
		vtui_gotoxy(28, 2);
		vtui_plot_char(0x53, (LIGHTGRAY<<4)+BLUE);
		vtui_gotoxy(14, 3);
		// Write library name with black text
		vtui_print_str("vtui library", 12, (LIGHTGRAY<<4)+BLACK, PETSCII_TRUE);
		vtui_gotoxy(11, 5);
		// Write a horizontal line with Red background and black characters
		vtui_hline(0x3D, 18, (RED<<4)+BLACK);
		vtui_gotoxy(6, 7);
		// Let people know about the new C compatibility
		vtui_print_str("now with c header and library", 29, (LIGHTGRAY<<4)+BLACK, PETSCII_TRUE);
		vtui_gotoxy(3, 9);
		// Draw a border and then fill the box inside
		vtui_border(3, 34, 18, (YELLOW<<4)+PURPLE);
		vtui_gotoxy(4, 10);
		vtui_fill_box(' ', 32, 16, (YELLOW<<4)+PURPLE);
		vtui_gotoxy(13, 9);
		// Write a character to show header starts
		vtui_plot_char(0x73, 0x74);
		// Write box header
		vtui_print_str("version 1.0", 11, (YELLOW<<4)+PURPLE, PETSCII_TRUE);
		// Write a character to show header ends
		vtui_plot_char(0x6B, 0x74);
		vtui_gotoxy(3, 17);
		// Write a character to connect a horizontal line with the box
		vtui_plot_char(0x6B, 0x74);
		vtui_hline(0x43, 32, 0x74);
		// Write a character on the other side of the box to connect line
		vtui_plot_char(0x73, 0x74);
		// Write text to screen
		vtui_gotoxy(5, 11);
		vtui_print_str("boxes with or without borders", 29, 0x74, PETSCII_TRUE);
		vtui_gotoxy(5, 13);
		vtui_print_str("horizontal lines", 16, 0x74, PETSCII_TRUE);
		vtui_gotoxy(5, 15);
		vtui_print_str("vertical lines", 14, 0x74, PETSCII_TRUE);
		vtui_gotoxy(5, 19);
		vtui_print_str("plot or scan characters", 23, 0x74, PETSCII_TRUE);
		vtui_gotoxy(5, 21);
		vtui_print_str("directly to/from screen ram", 27, 0x74, PETSCII_TRUE);
		vtui_gotoxy(5, 23);
		vtui_set_stride(2);
		vtui_print_str("*** now with cc65 support ***", 29, 0xFF, PETSCII_TRUE);
	}

	return 0;
}