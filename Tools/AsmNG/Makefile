CFILES=generated/parser.c generated/lex.c identifier.c main.c opcode.c tree.c ignores.c
HFILES=identifier.h opcode.h tree.h ignores.h
CC=gcc
TARGET=as6502
CCFLAGS=-ggdb -Werror

GAMECODE=../GameCodeNG/castlerunner.asm
CR002ROM=../GameCodeNG/cr002-rom.asm
FULLGAME=../GameCodeNG/gamecode.asm
GAMEHEX=../GameCodeNG/gamecode.hex

all: $(CFILES) $(HFILES)
	$(CC) $(CFILES) -lfl -o $(TARGET) $(CCFLAGS)
	
generated/lex.c: lex.l
	flex -o generated/lex.c lex.l

generated/parser.c: parser.y
	bison -d -o  generated/parser.c parser.y -Dparse.trace -Wcounterexamples

clean:
	rm -f generated/*
	rm -f $(TARGET)

game: $(TARGET)
	cat $(GAMECODE) $(CR002ROM) > $(FULLGAME)
	./$(TARGET) -i $(FULLGAME) -o $(GAMEHEX)