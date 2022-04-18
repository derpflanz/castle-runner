#include <stdio.h>
#include "fake6502.h"
#include "memory.h"
#include "ui.h"
#include "lcd.h"

// base for the mem-view
#define MEMORY_BASE 0x0000

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <hexfile>\n", argv[0]);
        return -1;
    }

    mem_init();
    if (mem_readfile(argv[1]) != 0) {
        return -1;
    }

    reset6502();
    ui_init();
    ui_init_lcd();
    lcd_init();

    ui_update_ram(MEMORY_BASE);

    int ch;
    int running = FALSE;
    while ((ch = getch()) != KEY_F(8)) {
        switch (ch) {
        case KEY_F(10):
            step6502();
            ui_update_ram(MEMORY_BASE);
            break;
        case KEY_F(5):
            running = !running;
            break;
        case KEY_F(6):
            reset6502();
            ui_update_ram(MEMORY_BASE);
            break;
        default:
            break;
        }

        if (running) {
            step6502();
            napms(1);
            ui_update_ram(MEMORY_BASE);

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