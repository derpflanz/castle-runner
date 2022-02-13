#!/bin/bash

BOARD="arduino:megaavr:nona4809"

red() {
    echo -e "\e[31m$1\e[39m"
}

green() {
    echo -e "\e[32m$1\e[39m"
}

if [ "$1" == "" ]; then
    red "Usage: $0 <.asm file>";
    exit;
fi

if [ ! -f "$1" ]; then
    red "$1 does not exist";
    exit;
fi

if [ -f "$2" ]; then
    green "Appending $2 to end of $1 before assembly"
    cat $1 $2 > /tmp/__program__.asm
else
    green "Assembling $1..."
    cat $1 > /tmp/__program__.asm
fi

echo
CMD="python Assembler/assembler.py -s8000 --opcodefile Assembler/w65c02s_opcodes.txt  /tmp/__program__.asm $1.hex"
echo $CMD
$CMD

if [ $? -eq 1 ]; then
    echo
    red "Assembly failed."
    exit
fi

echo

LINE=`arduino-cli board list | grep $BOARD`
if [ "$LINE" = "" ]; then
    red "Board $BOARD not connected. Could not program EEPROM."
    green "Running HEX on virtual machine."
    VirtualCR001/Fake6502/fake6502 $1.hex    
    exit;
fi

PORT=`echo $LINE | awk '{ print $1; }'`
green "Found $BOARD on port $PORT, programming EEPROM..."
echo

CMD="python EepromProgrammer/eeprom-program.py write -f $1.hex -p $PORT"
echo $CMD
$CMD
