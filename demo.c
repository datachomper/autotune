#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>
#include "scales.h"

#define FREQ 8000.0	// Hz
#define WINDOW 50	// ms
#define N 512		// # samples

void dump_buffer(void *, size_t);
void dump_double_buffer(void *, size_t);
void dump_half_complex(void *buf, size_t size);
void print_freq(void *buf, size_t size);

int main(int argc, char *argv[]) {
	FILE *fp;
	void *buf;
	unsigned long flen;
	short int *index;
	size_t num;

	double *in, *out;
	fftw_plan p;

	fp = fopen(argv[1], "r");

	if(!fp) {
		fprintf(stderr, "Can't open file %s\n", argv[1]);
		return;
	}

	printf("Frequency of C0 is %1.2f\n", C.freq[0]);

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
	num = fread(buf, 2, flen, fp);
	fclose(fp);
	printf("Read %ld samples. %1.2f sec of audio\n", (unsigned long)num, num/FREQ);
	
	index = buf;

	//dump_buffer(buf, num);

	in  = (double *) fftw_malloc(sizeof(double)*N);
	out = (double *) fftw_malloc(sizeof(double)*N);
	p = fftw_plan_r2r_1d(N, in, out, FFTW_R2HC, FFTW_ESTIMATE);

	int i,j,y;
	for(i=0, j=0; i<num; i++, j++) {

		if((num-i) < N) {
			printf("Dumping short sample\n");
			break;
		}

		in[j] = index[i];
		//printf("%lf\n", in[j]);

		//dump_double_buffer(in, N);
		if(j == N) {
			printf("sample %d\n", i);
			fftw_execute(p);
			// Normalize output
			for(y=0; y<N; y++) {
				out[y] = out[y]/(2*N);
			}
			//dump_double_buffer(out, N);
			//dump_half_complex(out, N);
			print_freq(out, N);
			j=0;
		}
	
	}
		
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
		//printf("%lf\n", (double) *index++);
		printf("[%dHz]\t%1.7f\n", (int)(i/(N/FREQ)), index[i]);
	}		
}

void dump_half_complex(void *buf, size_t size) {
	double *index = buf;
	double x;
	size_t n = 1;
	for(n; n<size/2; n++) {
		x = sqrt(index[n]*index[n] + index[size - n]*index[size -n]);
		//printf("[%dHz]\t\t(%1.5f %1.5f)\t%1.5f\n", (int)(n/(N/FREQ)), index[n], index[size-n], x);
		printf("%d,%1.2f\n", (int)(n/(N/FREQ)), x);
	}
}

void print_freq(void *buf, size_t size) {
	double *index = buf;
	double x[size];
	size_t n = 1;
	size_t max = 1;
	//note note;
	//scale *cmaj = &CMajor;

	for(n; n<size/2; n++) {
		x[n] = sqrt(index[n]*index[n] + index[size - n]*index[size -n]);
		if(x[n] > x[max]) {
			max = n;
		}
	}
	//note = freq2note(max/(N/FREQ));
//	printf("Detected frequency %d(%1.2f); Closest note %s\n",
//				(int)(max/(N/FREQ)), x[max],
//				note.desc);
}
