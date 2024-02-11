#include "ui.h"
#include <ctype.h>

void lcd_update(uint16_t base_addr, uint8_t *mem) {
    int ptr = base_addr;

    for (int row = 0; row < 30; row++) {
        for (int col = 0; col < 40; col++) {
            char c = mem[ptr++];
            if (isprint(c)) {
                ui_print_lcd(c, row, col);
            }
        }
    }
}
