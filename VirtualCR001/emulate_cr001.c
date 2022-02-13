#include <stdio.h>
#include "fake6502.h"
#include "memory.h"
#include "ui.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <hexfile>\n", argv[0]);
        return -1;
    }

    initialisememory();
    if (readfile(argv[1]) != 0) {
        return -1;
    }

    reset6502();
    initui();    
    update_ram_window(0x0000);

    int ch;
    while ((ch = getch()) != KEY_F(8)) {
        switch (ch) {
        case KEY_F(7):
            update_ram_window(0x0000);
            break;
        case KEY_F(10):
            step6502();
            update_ram_window(0x0000);
            break;
        default:
            break;
        }
    }

    endwin();
    return 0;
}