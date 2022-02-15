#include "ui.h"

int pointer;

void lcd_init() {
    pointer = 0;
}

void lcd_set_pointer(unsigned int newvalue) {
    // pointer can only between 0 and 79 as we only have 80 (4x20) characters
    // we do not wrap, just clip
    pointer = newvalue > 79 ? 79 : newvalue;
}

void lcd_put_character(char character) {
    // we need to translate the pointer to X, Y
    // idx  0 - 19 is line 0
    // idx 20 - 39 is line 2
    // idx 40 - 59 is line 1
    // idx 60 - 79 is line 4
    int line, column;
    if (pointer >=  0 && pointer <= 19) {
        line = 0;
        column = pointer;
    }
    if (pointer >= 20 && pointer <= 39) {
        line = 2;
        column = pointer - 20;
    }
    if (pointer >= 40 && pointer <= 59) {
        line = 1;
        column = pointer - 40;
    }
    if (pointer >= 60 && pointer <= 79) {
        line = 3;
        column = pointer - 60;
    }

    lcd_set_pointer(pointer+1);
    ui_print_lcd(character, line, column);
}
