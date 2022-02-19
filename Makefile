# Tools root makefile.
# You can make everything from here

game:
	make -C GameCode

vm:
	make -C VirtualCR001

run: vm game
	make -C GameCode run