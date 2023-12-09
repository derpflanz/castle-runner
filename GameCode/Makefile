# assembler stuff
ASM_BASE=../Assembler/
ASM_FILES=castlerunner.asm cr001-rom.asm 
ASM2_FILES=castlerunner.asm cr002-rom.asm
DEBUGFILE=castlerunner.debug
ASM_FLAGS=-s8000 --opcodefile $(ASM_BASE)w65c02s_opcodes.txt --debuginfo=$(DEBUGFILE) 
ASSEMBLER=python $(ASM_BASE)assembler.py
OUTPUT=castlerunner.hex

# vm stuff
VM_BASE=../VirtualCR001/
VM_EXEC=virtual_cr001

# eeprom stuff
PROG_BASE=../EepromProgrammer/
PROGRAMMER=python $(PROG_BASE)eeprom-program.py write 
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
	$(PROGRAMMER) -f $(OUTPUT) -p $(PORT)
