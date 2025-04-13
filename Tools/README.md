# castle-runner

This is my hobby project in making a 6502 based 8-bit computer from scratch. The end goal is to have it run Castle Runner, a game I made many moons ago.

# CR Assembler

Castle Runner is built from scratch, including the assembler (writting in Python). I created my own assembly language dialect to assemble the machine language code. That ML can then be written to a EEPROM. For which I built my own EEPROM programmer.

## Layout

By default, the RESET vector is set to $8000, which is the start of the ROM in the CR001 computer. The VirtualCR001 uses that as well by default. When programming the EEPROM, this vector is sent as the first two bytes and stored in $FFFC/$FFFD in the ROM.

Note that the assembler can have a DATA section at the start, which will cause to use another RESET vector to be written at the start of the .hex file.

## Assembler Dialect

### Opcodes
Opcodes are written as upper case mnemonics, as found in the documentation, followed by an optional address. 

### Addressing modes
Addressing modes are used as in the documentation, addresses and values are always hexadecimcal, prefixed by $.

Examples:
MODE          SYNTAX       
Immediate     <opcde> #$<value>
Zero Page     <opcde> $<1 byte address>
Zero Page,X   <opcde> $<1 byte address>,X
Absolute      <opcde> $<2 byte address>
Absolute,X    <opcde> $<2 byte address>,X
Absolute,Y    <opcde> $<2 byte address>,Y
Indirect,X    <opcde> ($<1 byte address>,X)
Indirect,Y    <opcde> ($<1 byte address>),Y

### Special Notations
The assembler understands the following extra notations:

#### String Constants (@)
Example:
@HELLO      "Hello World"

You can then use @HELLO as address.

#### HI/LO bytes
Example:
LDA LO(@HELLO)
LDA HI(@HELLO)

Useful to get the high or low byte of an address, to load vectors.

#### Labels (:)
Example:
:loop
JSR :loop

The label can be used to create a point in the code, and in the jump opcodes.

#### Comments (;)
Example:
LDA #$01            ; 1 is the initial value of our score board

All text after ; on a line will be ignored.

#### External Directives (!)
!break

These can be used to give the VirtualCR001 some extra directives on how to run this. They will not result in any ML code in the .hex file.

The following directives are implemented:
!break      Set breakpoint. The VM will stop running if this address is hit.

