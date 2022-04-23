ASM_BASE=../Assembler/
VM_BASE=../VirtualCR001/

ASSEMBLER=python $(ASM_BASE)assembler.py
VM_EXEC=virtual_cr001

OUTPUT=castlerunner.hex
ASM_FILES=castlerunner.asm cr001-rom.asm 
DEBUGFILE=castlerunner.debug

ASM_FLAGS=-s8000 --opcodefile $(ASM_BASE)w65c02s_opcodes.txt --debuginfo=$(DEBUGFILE)

# testing stuff (a scratch file)
TEST_HEX=test.hex
TEST_ASM=test.asm

TMP_FILE=/tmp/__program__.asm

all: $(ASM_FILES)
	cat $(ASM_FILES) > $(TMP_FILE)
	$(ASSEMBLER) $(ASM_FLAGS) $(TMP_FILE) $(OUTPUT)

$(OUTPUT): all

run: $(OUTPUT)
	$(VM_BASE)$(VM_EXEC) $(OUTPUT) $(DEBUGFILE)

test: $(TEST_ASM)
	$(ASSEMBLER) $(ASM_FLAGS) $(TEST_ASM) $(TEST_HEX)
	$(VM_BASE)$(VM_EXEC) $(TEST_HEX)
