# assembler stuff
ASM_BASE=../Assembler/
ASM1_FILES=castlerunner.asm cr001-rom.asm 
ASM_FILES=castlerunner.asm cr002-rom.asm
ASM3_FILES=io_test.asm 
DEBUGFILE=castlerunner.debug
PROGDATA=castlerunner-data.hex
PROGDATA_START=4000
ASM_FLAGS=-s8000 --opcodefile $(ASM_BASE)w65c02s_opcodes.txt --debuginfo=$(DEBUGFILE) 
ASSEMBLER=python $(ASM_BASE)assembler.py
OUTPUT=castlerunner.hex

# vm stuff
VM_BASE=../VirtualCR001/
VM_EXEC=virtual_cr001

# eeprom stuff
PROG_BASE=../EepromProgrammer/
PROGRAMMER=python $(PROG_BASE)eeprom-program.py 
BOARD=arduino:megaavr:nona4809
PORT=$(shell arduino-cli board list | grep $(BOARD) | awk '{ print $$1; }')

# testing stuff (a scratch file)
TEST_HEX=test.hex
TEST_ASM=test.asm

TMP_FILE=/tmp/__program__.asm

all: $(ASM_FILES)
	cat $(ASM_FILES) > $(TMP_FILE)
	$(ASSEMBLER) $(ASM_FLAGS) $(TMP_FILE) $(OUTPUT)

cr002: $(ASM2_FILES)
	cat $(ASM2_FILES) > $(TMP_FILE)
	$(ASSEMBLER) $(ASM_FLAGS) $(TMP_FILE) $(OUTPUT)

$(OUTPUT): all

run: $(OUTPUT)
	$(VM_BASE)$(VM_EXEC) $(OUTPUT) $(DEBUGFILE)

test: $(TEST_ASM)
	$(ASSEMBLER) $(ASM_FLAGS) $(TEST_ASM) $(TEST_HEX)
	$(VM_BASE)$(VM_EXEC) $(TEST_HEX)

program: $(OUTPUT)
	$(PROGRAMMER) write-code -f $(OUTPUT) -p $(PORT)

program-data: $(PROGDATA)
	$(PROGRAMMER) 
