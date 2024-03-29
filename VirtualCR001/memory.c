#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "ui.h"
#include "lcd.h"

uint8_t *ram;
uint8_t io_port0;

uint16_t _getvector(FILE *f) {
    int low = fgetc(f);
    int hi  = fgetc(f);
    return hi << 8 | low;
}

long _getsize(FILE *f) {
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); 

    return fsize;
}

void setresb(uint16_t address) {
    // write the address into 0xfffc and 0xfffd
    ram[0xfffc] = (uint8_t) (address & 0x00ff);
    ram[0xfffd] = (uint8_t) (address >> 8);
}

void setirq(uint16_t address) {
    // write the address into 0xfffe and 0xffff
    ram[0xfffe] = (uint8_t) (address & 0x00ff);
    ram[0xffff] = (uint8_t) (address >> 8);
}


void mem_init() {    
    ram = malloc(sizeof(uint8_t) * _64K);
}

void mem_show(uint16_t base_address, uint16_t length) {
    int oneshot = 0;
    for (int i = 0; i < length; i++) {
        if (i % 10 == 0) {
            printf("%s[%4x] ", oneshot==1?"\n":"", base_address + i);
        }

        printf ("%02x ", ram[base_address + i]);
        oneshot = 1;
    }

    printf("\n");
}

int mem_readfile(const char *hexfilename) {
    FILE *f = fopen(hexfilename, "rb");
    long image_size = _getsize(f) - 2;           // minus 2 to accomodate for the RESB vector the image starts with
    uint16_t reset_vector = _getvector(f);
    uint16_t irq_vector = _getvector(f);                              // read and ignore for now (VM has no IRQ support yet)
    uint16_t base_address = 0x8000;             // CR001 start of ROM
    setresb(reset_vector);
    setirq(irq_vector);

    long available_size = _64K - base_address;

    if (available_size < image_size) {
        printf("FATAL: Size of %s (image size %ld bytes) is larger than available memory area (%ld bytes; %x - %x)\n", 
            hexfilename, image_size, available_size, base_address, _64K-1);
        return 1;
    }     

    printf("Reading %s (%ld bytes) into memory, starting at address %x\n", hexfilename, image_size, base_address);
    fread(ram + base_address, 1, image_size, f);
    fclose(f);

    return 0;
}

uint8_t read6502(uint16_t address) {
    uint8_t value = ram[address];
    ui_writelog(MEMLOG, "%06d %06d READ   address %04x: %02x\n", instructions, clockticks6502, address, value);

    if (address == 0x4000) {
        value = io_port0;
    }

    return value;
}

void write6502(uint16_t address, uint8_t value) {
    ui_writelog(MEMLOG, "%06d %06d WRITE  address %04x: %02x\n", instructions, clockticks6502, address, value);

    // connect the 0x4000 and 0x4001 addresses to the LCD
    if (address == 0x4000) {
        lcd_io_write(value);
    } else if (address == 0x4001) {
        lcd_ctrl_write(value);
    } else {
        ram[address] = value;
    }
}

void set_io(int port, uint8_t value) {
    io_port0 = value;
}