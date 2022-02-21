#include <ncurses.h>
#include "ui.h"
#include <stdarg.h>

WINDOW *memory_log, *io_log;
WINDOW *lcd;

WINDOW *_create_newwin(int height, int width, int starty, int startx)
{	
    WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters */
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

char __bin__buf__[9];
char *_binary(uint8_t value) {
    char mask = 1;
    for (int i = 7; i >= 0; i--) {
        __bin__buf__[i] = (value & mask)?'1':'0';
        mask <<= 1;
    }
    __bin__buf__[8] = '\0';
    return __bin__buf__;
}

void _mem_wshow(WINDOW *win, uint8_t *mem, uint16_t base_address, uint16_t highlight) {
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
            if (pointer == highlight) {
                wbkgdset(win, COLOR_PAIR(1));
            }
            wprintw(win, "%02x", mem[pointer]);
            if (pointer == highlight) {
                wbkgdset(win, COLOR_PAIR(2));
            }
            wprintw(win, " ");
            pointer++;
        }
        lines++;
        wprintw(win, "\n");
    }   
}

void _init_io_log() {
    io_log = _create_newwin(LINES - 10, COLS / 4, 1, COLS / 4);
    scrollok(io_log, TRUE);
    wprintw(io_log, "\n");
    ui_writelog(IOLOG, "IO log ready\n");
    wrefresh(io_log);
}

void _init_memory_log() {
    memory_log = _create_newwin(LINES - 10, COLS / 4, 1, 0);
    scrollok(memory_log, TRUE);
    wprintw(memory_log, "\n");
    ui_writelog(MEMLOG, "Memory log ready\n");
    wrefresh(memory_log);
}

void ui_print_lcd(char character, int row, int column) {
    mvwaddch(lcd, row, column, character);
    wrefresh(lcd);
}

void ui_init_lcd() {
    lcd = newwin(4, 20, LINES - 8, 1);
    wbkgd(lcd, COLOR_PAIR(1));
    wrefresh(lcd);
}

void ui_init() {
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    printw("F5: start/stop; F6: reset; F8: exit; F10: step");
    refresh();

    _init_memory_log();
    _init_io_log();
}

void ui_writelog(int target, const char *format, ...) {
    WINDOW *win = target == IOLOG?io_log:memory_log;

    char buffer[1024];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, 1024, format, ap);
    va_end(ap);

    wprintw(win, " %s", buffer);
    box(win, 0, 0);
    wrefresh(win);
}

void ui_update_ram(uint16_t base_address) {
    int stack_height = 10;
    int rom_height = 15;

    WINDOW *memory_win;
    memory_win = _create_newwin(LINES - 10 - rom_height - stack_height, COLS / 2, 1, COLS / 2);
    _mem_wshow(memory_win, ram, base_address, 0);
    box(memory_win, 0, 0);
    mvwprintw(memory_win, 0, 0, "[MEMORY]");
    wrefresh(memory_win);

   WINDOW *rom_win;
    rom_win = _create_newwin(rom_height, COLS / 2, LINES - 9 - stack_height - rom_height, COLS / 2);
    _mem_wshow(rom_win, ram, 0x8000, pc);
    box(rom_win, 0, 0);
    mvwprintw(rom_win, 0, 0, "[ROM PC=%04x A=%02x X=%02x Y=%02x STATUS=%s #=%d TICKS=%d]", 
        pc, a, x, y, _binary(status), instructions, clockticks6502);
    wrefresh(rom_win);

    WINDOW *stack_win;
    stack_win = _create_newwin(stack_height, COLS / 2, LINES - 9 - stack_height, COLS / 2);
    _mem_wshow(stack_win, ram, 0x0100, (0x0100 | sp));
    box(stack_win, 0, 0);
    mvwprintw(stack_win, 0, 0, "[STACK SP=01%02x]", sp);
    wrefresh(stack_win);
}