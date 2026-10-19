chip8: src/main.c
	gcc -Wall -Wextra -g -o chip8 src/main.c

clean:
	rm -f chip8
