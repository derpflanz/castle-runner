#ifndef __LCD_H
#define __LCD_H

void lcd_set_pointer(unsigned int newvalue);
void lcd_put_character(char character);
void lcd_init();

void lcd_io_write(uint8_t value);
void lcd_ctrl_write(uint8_t value);

#endif