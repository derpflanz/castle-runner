ASM_FILES=castlerunner.asm cr002-rom.asm
ASM_TEMP=./generated/castlerunner_combined.asm
ASM=../AsmNG/as6502
HEX_OUT=./generated/castlerunner.hex

IGNORES=VIO_ResetDisplay,VIO_InitDisplay,VIO_ClearDisplay,InitVideoRam,LoadBackdrop,VIO_WriteGraphScreen,VIO_WriteCharScreen

all: $(ASM_FILES)
	cat $(ASM_FILES) > $(ASM_TEMP)
	$(ASM) -i $(ASM_TEMP) -o $(HEX_OUT) -a -s

emulate: $(ASM_FILES)
	cat $(ASM_FILES) > $(ASM_TEMP)
	$(ASM) -i $(ASM_TEMP) -o $(HEX_OUT) -a -s --ignore=$(IGNORES)
