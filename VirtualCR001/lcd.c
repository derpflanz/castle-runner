#include "ui.h"

int pointer;
uint8_t io_buffer;
uint8_t data_register;
uint8_t instr_register;

void lcd_init() {
    pointer = 0;
    io_buffer = 0;
    data_register = 0;
    instr_register = 0;
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

void lcd_io_write(uint8_t value) {
    io_buffer = value;
}

void lcd_ctrl_write(uint8_t value) {
    // ctrl_buffer is not a real buffer, but three 
    // control lines E, RS and !RW
    // on the CR001, these are wired
    // ctrl[0] = ENABLE
    // ctrl[1] = !RW (read not supported)
    // ctrl[2] = REG_SEL

    int enable = (value & 0x01) > 0;
    int reg_sel = (value & 0x04) > 0;
    ui_writelog("CTRL; ENABLE=%d, REGSEL=%d\n", enable, reg_sel);

    if (enable == 1) {
        if (reg_sel == 0) {
            instr_register = io_buffer;
            if (instr_register & 0x80) {    // DB7 = 1
                pointer = instr_register & 0x7f;
            }
            ui_writelog("INSTR VAL=%02x; DATA=%02x\n", value, instr_register);
        }
        if (reg_sel == 1) {
            data_register = io_buffer;
            lcd_put_character(data_register);
            ui_writelog("DATA VAL=%02x; DATA=%02x\n", value, data_register);
        }

    }
}
