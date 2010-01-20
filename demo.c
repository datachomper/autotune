#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>

#define FREQ 8000	// Hz
#define WINDOW 50	// ms

void dump_buffer(void *, size_t);

int main(int argc, char *argv[]) {
	FILE *fp;
	void *buf;
	unsigned long flen;
	short int *index;
	size_t num;

	fp = fopen(argv[1], "r");

	if(!fp) {
		fprintf(stderr, "Can't open file %s\n", argv[1]);
		return;
	}

	// Get file length
	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Allocate Heap
	buf = malloc(flen+1);
	if(!buf) {
		fprintf(stderr, "Unable to allocate %ld bytes", flen);
		fclose(fp);
		return;
	}

	// Fill buffer
	num = fread(buf, 1, flen, fp);
	fclose(fp);
	printf("Read %ld bytes\n", (unsigned long)num);
	
	index = buf;

	dump_buffer(buf, num);
	
	free(buf);
	return 0;
}

void dump_buffer(void *buf, size_t size) {
	short int *index = buf;
	size_t i = 0;
	for(i; i< size; i++) {
		printf("%hd ", *index++);
	}		
}
