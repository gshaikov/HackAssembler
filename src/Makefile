.PHONY: clean

warnings = -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

hack_assembler: *.o
	cc $(warnings) -o hack_assembler hack_assembler.o

*.o:
	cc $(warnings) -c -o hack_assembler.o hack_assembler.c

clean:
	rm -rf hack_assembler.o hack_assembler
