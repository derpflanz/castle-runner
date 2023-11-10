# Tools root makefile.
# You can make everything from here

game:
	make -C GameCode

vm:
	make -C VirtualCR001

run: vm game
	make -C GameCode run

test: vm
	make -C GameCode test
	
program: game
	make -C GameCode program