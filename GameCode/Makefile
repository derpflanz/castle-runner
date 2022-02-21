ASM_BASE=../Assembler/
VM_BASE=../VirtualCR001/

ASSEMBLER=python $(ASM_BASE)/assembler.py
VM_EXEC=virtual_cr001

ASM_FLAGS=-s8000 --opcodefile $(ASM_BASE)/w65c02s_opcodes.txt

OUTPUT=castlerunner.hex
ASM_FILES=castlerunner.asm cr001-rom.asm 

TMP_FILE=/tmp/__program__.asm

all: $(ASM_FILES)
	cat $(ASM_FILES) > $(TMP_FILE)
	$(ASSEMBLER) $(ASM_FLAGS) $(TMP_FILE) $(OUTPUT)

run: $(OUTPUT)
	$(VM_BASE)$(VM_EXEC) $(OUTPUT)

