CFILES=generated/parser.c generated/lex.c identifier.c main.c
HFILES=identifier.h
CC=gcc
TARGET=as6502

all: $(CFILES) $(HFILES)
	$(CC) $(CFILES) -lfl -o $(TARGET)
	
generated/lex.c: lex.l
	flex -o generated/lex.c lex.l

generated/parser.c: parser.y
	bison -d -o  generated/parser.c parser.y -Dparse.trace -Wcounterexamples

clean:
	rm generated/*
