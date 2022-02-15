#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "ui.h"

uint8_t *ram;

void setresb(uint16_t address) {
    // write the address into 0xfffc and 0xfffd
    ram[0xfffc] = (uint8_t) (address & 0x00ff);
    ram[0xfffd] = (uint8_t) (address >> 8);
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

long _getsize(FILE *f) {
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); 

    return fsize;
}

uint16_t _getbaseaddress(FILE *f) {
    int resb_low = fgetc(f);
    int resb_hi  = fgetc(f);
    return resb_hi << 8 | resb_low;
}

int mem_readfile(const char *hexfilename) {
    FILE *f = fopen(hexfilename, "rb");
    long image_size = _getsize(f) - 2;           // minus 2 to accomodate for the RESB vector the image starts with
    uint16_t base_address = _getbaseaddress(f);
    setresb(base_address);

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
    ui_writelog("READ   address %04x: %02x\n", address, value);

    return value;
}

void write6502(uint16_t address, uint8_t value) {
    ui_writelog("WRITE  address %04x: %02x\n", address, value);
    ram[address] = value;
}
