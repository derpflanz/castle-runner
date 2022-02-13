#include <ncurses.h>
#include "ui.h"
#include <stdarg.h>

WINDOW *run;
WINDOW *_create_newwin(int height, int width, int starty, int startx);

void initui() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    printw("F7: refresh memory view; F8: exit; F10: step");
    refresh();

    run = _create_newwin(LINES - 10, COLS / 2, 1, 0);
    scrollok(run, TRUE);
    wprintw(run, "\n");
    writelog("6502 emulator ready\n");
    wrefresh(run);
}

WINDOW *_create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void writelog(const char *format, ...) {
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

void update_ram_window(uint16_t base_address) {
    WINDOW *memory_win;
    memory_win = _create_newwin(LINES - 10, COLS / 2, 1, COLS / 2);
    _wshowmem(memory_win, ram, base_address);
    box(memory_win, 0, 0);
    wrefresh(memory_win);
}