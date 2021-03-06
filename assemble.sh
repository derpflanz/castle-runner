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

green "Assembling $1..."
echo
CMD="python Assembler/assembler.py $1 $1.hex"
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
    exit;
fi

PORT=`echo $LINE | awk '{ print $1; }'`
green "Found $BOARD on port $PORT, programming EEPROM..."
echo

CMD="python EepromProgrammer/eeprom-program.py write -f $1.hex -p $PORT"
echo $CMD
$CMD
