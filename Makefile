auto:
	cc demo.c -o demo -lfftw3 -lm
debug:
	cc demo.c -o demo -lfftw3 -lm -ggdb
