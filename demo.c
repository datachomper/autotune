#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

#define FREQ 8000	// Hz
#define WINDOW 50	// ms
#define N 1000		// # samples

void dump_buffer(void *, size_t);
void dump_double_buffer(void *, size_t);

int main(int argc, char *argv[]) {
	FILE *fp;
	void *buf;
	unsigned long flen;
	short int *index;
	size_t num;

	double *in;
	fftw_complex *out;
	fftw_plan p;

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

	//dump_buffer(buf, num);

	in  = (double *) fftw_malloc(sizeof(double)*N);
	out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex)*N);
	int i = 0;
	for(i; i<N; i++) {
		in[i] = *index++;
		//printf("%lf\n", in[i]);
	}
	//dump_double_buffer(in, N);
	p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
	fftw_execute(p);

	dump_double_buffer(out, N);
		
	// Cleanup
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
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

void dump_double_buffer(void *buf, size_t size) {
	double *index = buf;
	size_t i = 0;
	for(i; i< size; i++) {
		printf("%lf\n", (double) *index++);
	}		
}
