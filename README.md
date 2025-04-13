# castle-runner

This is my hobby project in making a 6502 based 8-bit computer from scratch. The end goal is to have it run Castle Runner, a game I made many moons ago.

# CR Assembler

Castle Runner is built from scratch, including the assembler (new version, written in C). I created my own assembly language dialect to assemble the machine language code. That ML can then be written to a EEPROM. For which I built my own EEPROM programmer.

## Layout

By default, the RESET vector is set to $8000, which is the start of the ROM in the CR001 computer. The VirtualCR001 uses that as well by default. When programming the EEPROM, this vector is sent as the first two bytes and stored in $FFFC/$FFFD in the ROM.

Note that the assembler can have a DATA section at the start, which will cause to use another RESET vector to be written at the start of the .hex file.

