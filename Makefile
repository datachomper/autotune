auto:
	cc demo.c -o demo -lfftw3 -lm -Wall
debug:
	cc demo.c -o demo -lfftw3 -lm -Wall -ggdb
