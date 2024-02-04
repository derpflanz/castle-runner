#include <stdio.h>
#include "fake6502.h"
#include "memory.h"
#include "ui.h"
#include "lcd.h"
#include "debug.h"
#include <curses.h>
#include "generic.h"

// base for the mem-view
#define VIDEO_BASE 0x0200

int main(int argc, char **argv) {
    uint16_t video_base = VIDEO_BASE;
    if (argc < 2) {
        printf("Usage: %s <hexfile> [<debugfile>]\n", argv[0]);
        return -1;
    }

    mem_init();

    if (mem_readfile(argv[1]) != 0) {
        return -1;
    }

    if (argc >= 3) {
        debug_init(argv[2]);
    }

    reset6502();    
    ui_init();
    ui_init_lcd();
    lcd_init();

    ui_update_ram(VIDEO_BASE);

    int ch;
    int running = FALSE;
    while ((ch = getch()) != KEY_F(8)) {
        switch (ch) {
        case KEY_F(10):
            step6502();
            ui_update_ram(video_base);
            break;
        case KEY_F(5):
            running = !running;
            break;
        case KEY_F(6):
            reset6502();
            ui_update_ram(video_base);
            break;
        case KEY_F(7):
            irq6502();
            break;
        case KEY_LEFT:
            set_io(0, JOY_LEFT);
            break;
        case KEY_RIGHT:
            set_io(0, JOY_RIGHT);
            break;
        case KEY_UP:
            set_io(0, JOY_UP);
            break;
        case KEY_DOWN:
            set_io(0, JOY_DOWN);
            break;
        case KEY_NPAGE:
            video_base += 0x0200;
            ui_update_ram(video_base);
            break;
        case KEY_PPAGE:
            video_base -= 0x0200;
            ui_update_ram(video_base);
            break;
        default:
            break;
        }

        if (running) {
            step6502();
            if (array_contains(pc, breakpoints)) {
                breakpoint_hit = TRUE;
            }
            napms(1);
            ui_update_ram(VIDEO_BASE);

            if (breakpoint_hit == TRUE) {
                running = FALSE;
                breakpoint_hit = FALSE;
            }
        } else {
            napms(100);
        }
    }

    endwin();
    return 0;
}