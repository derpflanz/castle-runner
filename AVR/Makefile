CFILES=main.c sound.c io.c
HEXFILE=sound.hex
BINARY=sound.o

CC=avr-gcc
CFLAGS=-O1 -Wall -mmcu=atmega328 -Werror
OBJCOPY=avr-objcopy
OBJCOPYFLAGS=-O ihex

PROGTYPE=stk500v2
PROGR=avrdude
#PROGFLAGS=-v -B250kHz 
PROGFLAGS=-v -B125kHz 
TARGET=m328
PORT=/dev/ttyACM0

all: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(BINARY)

$(HEXFILE): $(BINARY)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BINARY) $(HEXFILE)

program: $(HEXFILE)
	$(PROGR) -p$(TARGET) -c$(PROGTYPE) -P$(PORT) -U flash:w:$(HEXFILE) $(PROGFLAGS)

clean:
	rm *.o *.hex
