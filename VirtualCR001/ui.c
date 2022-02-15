#include <ncurses.h>
#include "ui.h"
#include <stdarg.h>

WINDOW *run;
WINDOW *lcd;
WINDOW *_create_newwin(int height, int width, int starty, int startx);

void ui_init_lcd() {
    lcd = newwin(4, 20, LINES - 8, 1);
    wbkgd(lcd, COLOR_PAIR(1));

    wprintw(lcd, "test");

    wrefresh(lcd);
}

void ui_init() {
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    cbreak();
    keypad(stdscr, TRUE);

    printw("F7: refresh memory view; F8: exit; F10: step");
    refresh();

    run = _create_newwin(LINES - 10, COLS / 2, 1, 0);
    scrollok(run, TRUE);
    wprintw(run, "\n");
    ui_writelog("6502 emulator ready\n");
    wrefresh(run);
}

WINDOW *_create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters */
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void ui_writelog(const char *format, ...) {
    char buffer[1024];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, 1024, format, ap);
    va_end(ap);

    wprintw(run, " %s", buffer);
    box(run, 0, 0);
    wrefresh(run);
}

void _wshowmem(WINDOW *win, uint8_t *mem, uint16_t base_address) {    
    int width, height;
    wclear(win);
    getmaxyx(win, height, width);
    uint16_t pointer = base_address;
    int lines = 0;

    wprintw(win, "\n");
    while (lines < (height - 2)) {
        wprintw(win, " [%04x] ", pointer);

        int printable_bytes = (width - 9) / 3;
        for (int i = 0; i < printable_bytes; i++) {
            wprintw(win, "%02x ", mem[pointer]);
            pointer++;
        }
        lines++;
        wprintw(win, "\n");
    }   
}

void ui_update_ram(uint16_t base_address) {
    WINDOW *memory_win;
    memory_win = _create_newwin(LINES - 10, COLS / 2, 1, COLS / 2);
    _wshowmem(memory_win, ram, base_address);
    box(memory_win, 0, 0);
    wrefresh(memory_win);
}