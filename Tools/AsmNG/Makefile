# This builds the assembler, use the Makefile in ../../Source to build the game

CFILES=generated/parser.c generated/lex.c identifier.c main.c opcode.c tree.c ignores.c
HFILES=identifier.h opcode.h tree.h ignores.h
CC=gcc
TARGET=as6502
CCFLAGS=-ggdb -Werror

all: $(CFILES) $(HFILES)
	$(CC) $(CFILES) -lfl -o $(TARGET) $(CCFLAGS)
	
generated/lex.c: lex.l
	flex -o generated/lex.c lex.l

generated/parser.c: parser.y
	bison -d -o  generated/parser.c parser.y -Dparse.trace -Wcounterexamples

clean:
	rm -f generated/*
	rm -f $(TARGET)
