CFILES=generated/parser.c generated/lex.c main.c
CC=gcc
TARGET=as6502

all: $(CFILES)
	$(CC) $(CFILES) -lfl -o $(TARGET)
	
generated/lex.c: lex.l
	flex -o generated/lex.c lex.l

generated/parser.c: parser.y
	bison -d -o  generated/parser.c parser.y 

