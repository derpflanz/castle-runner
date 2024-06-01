# Tools root makefile.
# You can make everything from here

game:
	make -C GameCodeNG

vm:
	make -C VirtualCR001

run: vm game
	make -C GameCode run

test: vm
	make -C GameCodeNG emulate
	
program: game
	make -C GameCodeNG program