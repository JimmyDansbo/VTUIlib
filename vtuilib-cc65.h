#ifndef _library_h_
#define _library_h_

#define VIDRAM 0x80
#define SYSRAM 0x00
#define VRAMBANK1 1
#define VRAMBANK0 0

#define	SCRMODE80X60 0x00
#define SCRMODE80X30 0x01
#define SCRMODE40X60 0x02
#define SCRMODE40X30 0x03
#define SCRMODE40X15 0x04
#define SCRMODE20X30 0x05
#define SCRMODE20X15 0x06
#define SCRMODESWAP  0xFF

#define BLACK		0
#define WHITE		1
#define RED			2
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

extern void		__fastcall__ vtui_load(unsigned addr);
extern void		__fastcall__ vtui_initialize();
extern char		__fastcall__ vtui_screen_set(char mode);
extern void		__fastcall__ vtui_set_bank(char bank);
extern void		__fastcall__ vtui_set_stride(char stride);
extern void		__fastcall__ vtui_set_decr(char decr);
extern void		__fastcall__ vtui_clr_scr(char fillchar, char color);
extern void		__fastcall__ vtui_gotoxy(char x, char y);
extern void		__fastcall__ vtui_plot_char(char ch, char color);
extern unsigned	__fastcall__ vtui_scan_char();
extern void		__fastcall__ vtui_hline(char ch, char length, char color);
extern void		__fastcall__ vtui_vline(char ch, char length, char color);
extern void		__fastcall__ vtui_print_str(char *str, char length, char color, char ispetscii);
extern void		__fastcall__ vtui_fill_box(char ch, char width, char height, char color);
extern char		__fastcall__ vtui_pet2scr(char ch);
extern char		__fastcall__ vtui_scr2pet(char ch);
extern void		__fastcall__ vtui_border(char border, char width, char height, char color);
extern void		__fastcall__ vtui_save_rect(unsigned destaddr, char destram, char width, char height, char vrambank);
extern void		__fastcall__ vtui_rest_rect(unsigned srcaddr, char srcram, char width, char height, char vrambank);
extern char		__fastcall__ vtui_input_str(char *str, char maxlen, char color);

#endif