#include <ncurses.h>
#include "ui.h"
#include <stdarg.h>
#include <stdlib.h>
#include "debug.h"
#include "generic.h"
#include "fake6502.h"
#include "memory.h"
#include <ctype.h>

WINDOW *memory_log, *io_log;
WINDOW *lcd;
WINDOW *memory_win, *rom_win, *stack_win, *video_win;

WINDOW *_create_newwin(int height, int width, int starty, int startx)
{	
    WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters */
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

char __flag_buf[9];
char *_flags(uint8_t value) {
    unsigned char mask = 1;
    char set, c;
    for (int i = 7; i >= 0; i--) {
        set = (value & mask);
        switch (mask) {
            case FLAG_CARRY:
                c = set?'C':'c';
                break;
            case FLAG_ZERO:
                c = set?'Z':'z';
                break;
            case FLAG_INTERRUPT:
                c = set?'I':'i';
                break;
            case FLAG_DECIMAL:
                c = set?'D':'d';
                break;
            case FLAG_BREAK:
                c = set?'B':'b';
                break;
            case FLAG_CONSTANT:
                c = set?'T':'t';
                break;
            case FLAG_OVERFLOW:
                c = set?'O':'o';
                break;
            case FLAG_SIGN:
                c = set?'S':'s';
                break;
        }

        __flag_buf[i] = c;
        mask <<= 1;
    }
    __flag_buf[8] = '\0';
    return __flag_buf;
}

/* Shows memory in a window, "highlight" will be set blue, the list of addresses in 
   "extra_highlight" will be set red
*/
void _mem_wshow(WINDOW *win, uint8_t *mem, uint16_t base_address, uint16_t highlight, uint16_t *extra_highlights, int stack, int printable) {
    int width, height;
    int direction = stack?-1:1;

    wmove(win, 0, 0);
    getmaxyx(win, height, width);
    uint16_t pointer = base_address;
    int lines = 0;

    wprintw(win, "\n");
    while (lines < (height - 2)) {
        wprintw(win, " [%04x] ", pointer);

        int printable_bytes = (width - 9) / 3;
        for (int i = 0; i < printable_bytes; i++) {
            if (array_contains(pointer, extra_highlights)) {
                wbkgdset(win, COLOR_PAIR(3));
            }
            if (pointer == highlight) {
                wbkgdset(win, COLOR_PAIR(1));
            }
            if (printable == 1 && isprint(mem[pointer])) {
                wprintw(win, "%c ", mem[pointer]);
            } else {
                wprintw(win, "%02x", mem[pointer]);
            }
            wbkgdset(win, COLOR_PAIR(2));
            wprintw(win, " ");
            pointer += direction;
        }
        lines++;
        wprintw(win, "\n");
    }  
}

void _init_io_log() {
    io_log = _create_newwin(LINES - 10, COLS / 4, 1, COLS / 4);
    scrollok(io_log, TRUE);
    wprintw(io_log, "\n");
    ui_writelog(IOLOG, "Opcode log ready\n");
    wrefresh(io_log);
}

void _init_memory_log() {
    memory_log = _create_newwin(LINES - 10, COLS / 4, 1, 0);
    scrollok(memory_log, TRUE);
    wprintw(memory_log, "\n");
    ui_writelog(MEMLOG, "Memory log ready\n");
    wrefresh(memory_log);
}

void _init_memory_windows() {
    int video_height = 10, stack_height = 5;
    int rom_height = 10;

    memory_win = _create_newwin(LINES - 10 - rom_height - stack_height - video_height, COLS / 2, 1, COLS / 2);
    rom_win = _create_newwin(rom_height, COLS / 2, LINES - 9 - stack_height - rom_height - video_height, COLS / 2);
    stack_win = _create_newwin(stack_height, COLS / 2, LINES - 9 - stack_height - video_height, COLS / 2);
    video_win = _create_newwin(video_height, COLS / 2, LINES - 9 - video_height, COLS / 2);
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
    init_pair(3, COLOR_WHITE, COLOR_RED);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    printw("F5: start/stop; F6: reset; F7: IRQ; F8: exit; F10: step");
    refresh();

    _init_memory_log();
    _init_io_log();
    _init_memory_windows();
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

extern uint16_t ea;
void ui_update_ram(uint16_t base_address) {
    uint16_t epoints[2];
    epoints[0] = ea;
    epoints[1] = 0;

    _mem_wshow(memory_win, ram, base_address, 0, epoints, 0, 0);
    box(memory_win, 0, 0);
    mvwprintw(memory_win, 0, 0, "[MEMORY 0x4000=%02x]", read6502(0x4000));
    wrefresh(memory_win);

    _mem_wshow(rom_win, ram, 0x8000, pc, breakpoints, 0, 0);
    box(rom_win, 0, 0);
    mvwprintw(rom_win, 0, 0, "[ROM PC=%04x A=%02x X=%02x Y=%02x STATUS=%s #=%d TICKS=%d]", 
        pc, a, x, y, _flags(status), instructions, clockticks6502);
    wrefresh(rom_win);

    _mem_wshow(stack_win, ram, 0x01ff, (0x0100 | sp), NULL, 1, 0);
    box(stack_win, 0, 0);
    mvwprintw(stack_win, 0, 0, "[STACK SP=01%02x]", sp);
    wrefresh(stack_win);

    _mem_wshow(video_win, ram, 0x3000, 0x0000, epoints, 0, 1);
    box(video_win, 0, 0);
    mvwprintw(video_win, 0, 0, "[VIDEO]");
    wrefresh(video_win);
}