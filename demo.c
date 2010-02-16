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
	FILE *fp, *fpout;
	void *buf;
	short int *synth;
	unsigned long flen;
	short int *index;
	size_t num;

	double *in, *out;
	fftw_plan forward, reverse;

	fp = fopen(argv[1], "r");
	fpout = fopen(argv[2], "w");

	if(!fp) {
		fprintf(stderr, "Can't open file %s\n", argv[1]);
		return 0;
	}
	if(!fpout) {
		fprintf(stderr, "Can't open fil %s\n", argv[2]);
		return 0;
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
		return 0;
	}
	synth = malloc(N*2);
	if(!synth) {
		fprintf(stderr, "Unable to allocate %d bytes", N*2);
		fclose(fp);
		return 0;
	}

	// Fill buffer
	num = fread(buf, 2, flen, fp);
	fclose(fp);
	printf("Read %ld samples. %1.2f sec of audio\n", (unsigned long)num, num/FREQ);
	
	index = buf;

	in  = (double *) fftw_malloc(sizeof(double)*N);
	out = (double *) fftw_malloc(sizeof(double)*N);
	forward = fftw_plan_r2r_1d(N, in, out, FFTW_R2HC, FFTW_ESTIMATE);
	reverse = fftw_plan_r2r_1d(N, out, in, FFTW_HC2R, FFTW_ESTIMATE);

	int i,j,y;
	for(i=0, j=0; i<num; i++, j++) {

		if((num-i) < N) {
			printf("Dumping short sample\n");
			break;
		}

		in[j] = index[i];
		//printf("%lf\n", in[j]);

		if(j == N) {
			j=0;
			printf("sample %d\n", i);
			fftw_execute(forward);
			// Normalize output
			for(y=0; y<N; y++) {
				out[y] = out[y]/(N);
			}
			print_freq(out, N);

			// Synthesize back to real audio
			fftw_execute(reverse);
			for(y=0; y<N; y++) {
				//synth[y] = (short int)(in[y]/(2*N));
				synth[y] = (short int)in[y];
			}
			//dump_buffer(synth, N);
			fwrite(synth, 2, N, fpout);
		}
	
	}
		
	// Cleanup
	fclose(fpout);
	fftw_destroy_plan(forward);
	fftw_destroy_plan(reverse);
	fftw_free(in);
	fftw_free(out);
	free(buf);
	return 0;
}

void dump_buffer(void *buf, size_t size) {
	short int *index = buf;
	size_t i = 0;
	for(; i<size; i++) {
		printf("%hd ", *index++);
	}		
}

void dump_double_buffer(void *buf, size_t size) {
	double *index = buf;
	size_t i = 0;
	for(; i< size; i++) {
		//printf("%lf\n", (double) *index++);
		printf("[%dHz]\t%1.7f\n", (int)(i/(N/FREQ)), index[i]);
	}		
}

void dump_half_complex(void *buf, size_t size) {
	double *index = buf;
	double x;
	size_t n = 1;
	for(; n<size/2; n++) {
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
	struct note *note;
	//struct scale *cmaj = &CMajor;

	for(; n<size/2; n++) {
		x[n] = sqrt(index[n]*index[n] + index[size - n]*index[size -n]);
		if(x[n] > x[max]) {
			max = n;
		}
	}
	note = freq2note(max/(N/FREQ));
	printf("Detected frequency %d(%1.2f); Closest note %s; Octave: %d\n",
				(int)(max/(N/FREQ)), x[max],
				note->desc, (int)note->octave);
}
