ASM_FILES=castlerunner.asm cr002-rom.asm
ASM_TEMP=./generated/castlerunner_combined.asm
ASM=../AsmNG/as6502
HEX_OUT=./generated/castlerunner.hex

all: $(ASM_FILES)
	cat $(ASM_FILES) > $(ASM_TEMP)
	$(ASM) -i $(ASM_TEMP) -o $(HEX_OUT)